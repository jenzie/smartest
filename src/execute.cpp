/**
 * execute.C
 *
 * Instruction execution routine for the dummest little computer.
 */

#include <cstdio>
#include "includes.h"

/**
 * Moves instructions down through the pipeline.
 */
void assemble() {
	long opc;

	// The opc returned is from the previous cycle of execution.
	// Execute the first tick for each stage.
	fetch();
	printf("%-16s", inst_str);
	sprintf(inst_total_output, "%-16s", inst_output);
	opc = decode();
	printf("%-16s", inst_str);
	sprintf(inst_total_output, "%s%-16s", inst_total_output, inst_output);
	opc = execute( opc );
	printf("%-16s", inst_str);
	sprintf(inst_total_output, "%s%-16s", inst_total_output, inst_output);
	opc = memory( opc );
	printf("%-16s", inst_str);
	sprintf(inst_total_output, "%s%-16s", inst_total_output, inst_output);
	writeback( opc );
	printf("%s\n", inst_str);
	printf("%s%s\n", inst_total_output, inst_output);
	Clock::tick();
	
	// Execute the second tick for each stage.
	fetch_second();
	sprintf(inst_total_output, "%-16s", inst_output);
	decode_second();
	sprintf(inst_total_output, "%s%-16s", inst_total_output, inst_output);
	execute_second();
	sprintf(inst_total_output, "%s%-16s", inst_total_output, inst_output);
	memory_second();
	sprintf(inst_total_output, "%s%-16s", inst_total_output, inst_output);
	writeback_second();
	printf("%s%s\n", inst_total_output, inst_output);
	Clock::tick();
	
	if( reg_changed ){
		printf("Registers %02lx %02lx %02lx %02lx\n",reg_file[0]->value(),
			reg_file[1]->value(), reg_file[2]->value(), reg_file[3]->value());
	}
	
	// Print BPT table
	if( halt_inst == true ){
		printf("\nBPT Statistics\n==============\n");
		int total_queries = predictions + failures;
		printf("Predictions:\t%02d\nFailures:\t%02d\n", predictions, failures);
		printf("Accuracy:\t%02d%%\n",
			total_queries == 0 ? 0 : predictions / total_queries);
		printf("Branch Swapped: %02d\nSaturation:\t%02.2f\n",
			total_branch_swapped,
			total_branch_swapped == 0 ? 
				0 : total_branch_swapped / (double) MAX_BPT );
	}
	
	cout << endl;
}

void fetch(){
	sprintf(inst_str, "Read PC(%02lx)", pc.value());
	pc_bus.IN().pullFrom( pc );
	inst_mem.MAR().latchFrom( pc_bus.OUT() );
	incr_override = false;
	
	int index = check_entry( pc.value() );
	if( index != -1 ){
		print_prediction( index );	
	}
	else{
		sprintf(inst_output, "No BPT Hit");
	}
}

long decode(){

	make_inst_str(fd_ir);
	long opc = fd_ir(DATA_BITS - 1, DATA_BITS - 4);
	d_curr_opc = opc; // for second clock tick
	
	// Extract the packed bits
	int reg_rs = fd_ir(DATA_BITS - 5, DATA_BITS - 6);
	int reg_rt = fd_ir(DATA_BITS - 7, DATA_BITS - 8);
	int reg_rd = fd_ir(DATA_BITS - 9, DATA_BITS - 10);
	long small_imm = fd_ir(DATA_BITS - 9, DATA_BITS - 12);
	long large_imm = fd_ir(DATA_BITS - 5, DATA_BITS - 12);
	
	switch( opc ) {
		case 0: // NOP
			sprintf(inst_output, "-"); break;
		case 1: // ADD
			decode_R_TYPE(); break;
		case 2: // ADDI
			decode_I_TYPE(); break;
		case 3: // SUB
			decode_R_TYPE(); break;
		case 4: // AND
			decode_R_TYPE(); break;
		case 5: // OR
			decode_R_TYPE(); break;
		case 6: // XOR
			decode_R_TYPE(); break;
		case 7: // SLL
			decode_R_TYPE(); break;
		case 8: // SRL
			decode_R_TYPE(); break;
		case 9: // SRA
			decode_R_TYPE(); break;
		case 10: // SB
			decode_I_TYPE(); break;
		case 11: // LB
			decode_I_TYPE(); break;
		case 12: // BEZ
			dx_bus[0]->IN().pullFrom(*reg_file[reg_rs]);
			dx_a.latchFrom(dx_bus[0]->OUT());
			sprintf(inst_output, "a<-R%02lx b<-0",
				reg_file[reg_rs]->value());
		case 13: // BNE
			dx_bus[0]->IN().pullFrom(*reg_file[reg_rs]);
			dx_a.latchFrom(dx_bus[0]->OUT());
			sprintf(inst_output, "a<-R%02lx b<-0",
				reg_file[reg_rs]->value());
		case 14: // Jump
			dx_imm.latchFrom( large_bus.OUT() );
			large_bus.IN().pullFrom( fd_ir );
			sprintf(inst_output, "imm<-%02lx", large_imm);
			incr_override = true;
			break;
		case 15: // Halt
			sprintf(inst_output, "-");
			break;
		default:
			sprintf(inst_output, "-");
	}

	// swap out the previous opc and return it for the next stage.
	long old_opc = d_prev_opc;
	d_prev_opc = opc;
	return old_opc;
}

void decode_R_TYPE(){
	dx_bus[0]->IN().pullFrom(*reg_file[parse_rs(fd_ir)]);
	dx_bus[1]->IN().pullFrom(*reg_file[parse_rt(fd_ir)]);
	dx_a.latchFrom(dx_bus[0]->OUT());
	dx_b.latchFrom(dx_bus[1]->OUT());
	sprintf(inst_output, "a<-R%02lx b<-R%02lx",
		reg_file[parse_rs(fd_ir)]->value(), 
		reg_file[parse_rt(fd_ir)]->value());
}

void decode_I_TYPE(){
	dx_bus[0]->IN().pullFrom(*reg_file[parse_rt(fd_ir)]);
	small_bus.IN().pullFrom(fd_ir);
	dx_a.latchFrom(dx_bus[0]->OUT());
	dx_imm.latchFrom(small_bus.OUT());
	sprintf(inst_output, "a<-R%02lx imm<-%02lx",
		reg_file[parse_rt(fd_ir)]->value(), parse_imm(fd_ir, true));
}

long execute( long opc ){

	//cout << "X\t" << opc << endl;
	x_curr_opc = opc; // for second clock tick
	make_inst_str(dx_ir);
	
	switch( opc ) {
		case 0: // NOP
			sprintf(inst_output, "-");
			break;
		case 1: // ADD
			execute_R_TYPE( BusALU::op_add ); break;
		case 2: // ADDI
			execute_I_TYPE(); break;
		case 3: // SUB
			execute_R_TYPE( BusALU::op_sub ); break;
		case 4: // AND
			execute_R_TYPE(BusALU::op_and ); break;
		case 5: // OR
			execute_R_TYPE(BusALU::op_or ); break;
		case 6: // XOR
			execute_R_TYPE(BusALU::op_xor ); break;
		case 7: // SLL
			execute_R_TYPE(BusALU::op_lshift ); break;
		case 8: // SRL
			execute_R_TYPE(BusALU::op_rshift ); break;
		case 9: // SRA
			execute_R_TYPE(BusALU::op_rashift ); break;
		case 10: // SB
			execute_I_TYPE(); break;
		case 11: // LB
			execute_I_TYPE(); break;
		default:
			sprintf(inst_output, "-");
	}
	
	// swap out the previous opc and return it for the next stage.
	long old_opc = x_prev_opc;
	x_prev_opc = opc;
	return old_opc;
}

void execute_R_TYPE( BusALU::Operation op ){
	exec_alu.OP1().pullFrom(dx_a);
	exec_alu.OP2().pullFrom(dx_b);
	exec_alu.perform(op);
	xm_alu_out.latchFrom(exec_alu.OUT());
	sprintf(inst_output, "A:%02lx; B:%02lx", dx_a.value(), dx_b.value());
}

void execute_I_TYPE(){
	exec_alu.OP1().pullFrom(dx_a);
	exec_alu.OP2().pullFrom(dx_imm);
	exec_alu.perform(BusALU::op_add);
	xm_alu_out.latchFrom(exec_alu.OUT());
	sprintf(inst_output, "A:%02lx; IMM:%02lx", dx_a.value(), dx_imm.value());
}

long memory( long opc ){

	//cout << "M\t" << opc << endl;
	m_curr_opc = opc; // for second clock tick
	make_inst_str(xm_ir);
	
	switch( opc ) {
		case 0: // NOP
			sprintf(inst_output, "-");
			break;
		case 1: // Add
			mw_bus[0]->IN().pullFrom(xm_alu_out);
			mw_alu_out.latchFrom(mw_bus[0]->OUT());
			sprintf(inst_output, "passthru");
			break;
		case 10:
			sprintf(inst_output, "MEM[ALU_OUT]");
			mw_bus[0]->IN().pullFrom(xm_alu_out);
			data_mem.MAR().latchFrom(mw_bus[0]->OUT());
			break;
		default:
			sprintf(inst_output, "-");
	}
	
	// swap out the previous opc and return it for the next stage.
	long old_opc = m_prev_opc;
	m_prev_opc = opc;
	return old_opc;
}

void writeback( long opc ){

	w_curr_opc = opc; // for second clock tick
	make_inst_str(mw_ir);
	
	switch( opc ) {
		case 0: // NOP
			sprintf(inst_output, "-");
			break;
		case 1: // Add
			sprintf(inst_output, "-");
			break;
		case 10:
			sprintf(inst_output, "-");
			break;
		default:
			sprintf(inst_output, "-");
	}
}

void fetch_second(){

	// Actually fetch the next instruction.
	inst_mem.read();
	fd_ir.latchFrom( inst_mem.READ() );

	// The decode stage may override our next PC value.
	if( !incr_override ){
		pc.incr();
	}
	
	// TODO modify BPT?
	if(false){
	
	}
	else{
		sprintf(inst_output, "No BPT Update");
	}
}

void decode_second(){

	int reg_rt = fd_ir(DATA_BITS - 7, DATA_BITS - 8);
	
	switch( d_curr_opc ) {
		case 0: // NOP
			sprintf(inst_output, "-");
			break;
		case 1: // Add
			sprintf(inst_output, "-");
			break;
		case 10:
			sprintf(inst_output, "-");
			break;
		case 14:
			pc.latchFrom( decode_branch_bus.OUT() );
			decode_branch_bus.IN().pullFrom( dx_imm );
			sprintf(inst_output, "pc<-%02lx", dx_imm.value());
			break;
		case 15:
			sprintf(inst_output, "-");
			break;
		default:
			sprintf(inst_output, "-");
	}
	dx_bus[2]->IN().pullFrom(fd_ir);
	dx_ir.latchFrom(dx_bus[2]->OUT());
}

void execute_second(){

	switch(x_curr_opc){
		case 0:
			sprintf(inst_output, "-");
			break;
		case 1:
			sprintf(inst_output, "ALU_OUT<-%02lx", xm_alu_out.value());
			break;
		case 10:
			sprintf(inst_output, "ALU_OUT<-%02lx", xm_alu_out.value() );
			break;
		default:
			sprintf(inst_output, "-");
	}
	
	xm_bus[0]->IN().pullFrom(dx_ir);
	xm_ir.latchFrom(xm_bus[0]->OUT());

}

void memory_second(){

	switch( m_curr_opc ) {
		case 0: // NOP
			sprintf(inst_output, "-");
			break;
		case 1: // Add
			sprintf(inst_output, "-");
			break;
		case 10:
			mw_mdr.latchFrom(data_mem.READ());
			data_mem.read();
			sprintf(inst_output, "MDR<-MEM[%02lx]", xm_alu_out.value());
			break;
		default:
			sprintf(inst_output, "-");
	}
	
	mw_bus[1]->IN().pullFrom(xm_ir);
	mw_ir.latchFrom(mw_bus[1]->OUT());

}

void writeback_second(){

	reg_changed = false;
	
	switch( w_curr_opc ) {
		case 0: // NOP
			sprintf(inst_output, "-");
			break;
		case 1: // Add
			sprintf(inst_output, "R%lx<-%02lx",
				mw_ir(DATA_BITS - 9, DATA_BITS - 10), mw_alu_out.value());
			wd_bus.IN().pullFrom(mw_alu_out);
			reg_file[mw_ir(DATA_BITS - 9, DATA_BITS - 10)]->latchFrom(wd_bus.OUT());
			reg_changed = true;
			break;
		case 10:
			sprintf(inst_output, "R%lx<-%02lx",
				mw_ir(DATA_BITS - 5, DATA_BITS - 6), mw_mdr.value());
			wd_bus.IN().pullFrom(mw_mdr);
			reg_file[mw_ir(DATA_BITS - 5, DATA_BITS - 6)]->latchFrom(wd_bus.OUT());
			reg_changed = true;
			break;
		case 15:
			sprintf(inst_output, "Smartest Off");
			halt_inst = true;
			done = true;
			break;
		default:
			sprintf(inst_output, "-");
	}
}
