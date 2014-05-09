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
	
	cout << endl;
}

void fetch(){
	sprintf(inst_str, "Read PC(%02lx)", pc.value());
	pc_bus.IN().pullFrom( pc );
	inst_mem.MAR().latchFrom( pc_bus.OUT() );
	
	// TODO: check BPT for hits
	if(false){
	
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
	long big_imm = fd_ir(DATA_BITS - 5, DATA_BITS - 12);
	
	switch( opc ) {
		case 0: // NOP
			sprintf(inst_output, "-");
			break;
		case 1: // Add
			dx_bus[0]->IN().pullFrom(*reg_file[reg_rs]);
			dx_bus[1]->IN().pullFrom(*reg_file[reg_rt]);
			dx_a.latchFrom(dx_bus[0]->OUT());
			dx_b.latchFrom(dx_bus[1]->OUT());
			sprintf(inst_output, "a<-R%02lx b<-R%02lx",
				reg_file[reg_rs]->value(), reg_file[reg_rt]->value());
			break;
		case 10:
			dx_bus[0]->IN().pullFrom(*reg_file[reg_rt]);
			small_bus.IN().pullFrom(fd_ir);
			dx_b.latchFrom(dx_bus[0]->OUT());
			dx_imm.latchFrom(small_bus.OUT());
			sprintf(inst_output, "b<-R%02lx imm<-%02lx",
				reg_file[reg_rt]->value(), small_imm);
			break;
		case 15:
			sprintf(inst_output, "-");
			break;
	}

	// swap out the previous opc and return it for the next stage.
	long old_opc = d_prev_opc;
	d_prev_opc = opc;
	return old_opc;
}

long execute( long opc ){

	//cout << "X\t" << opc << endl;
	x_curr_opc = opc; // for second clock tick
	make_inst_str(dx_ir);
	
	switch( opc ) {
		case 0: // NOP
			sprintf(inst_output, "-");
			break;
		case 1: // Add
			exec_alu.OP1().pullFrom(dx_a);
			exec_alu.OP2().pullFrom(dx_b);
			exec_alu.perform(BusALU::op_add);
			xm_alu_out.latchFrom(exec_alu.OUT());
			sprintf(inst_output, "A:%02lx; B:%02lx", dx_a.value(),
				dx_b.value());
			break;
		case 10:
			exec_alu.OP1().pullFrom(dx_b);
			exec_alu.OP2().pullFrom(dx_imm);
			exec_alu.perform(BusALU::op_add);
			xm_alu_out.latchFrom(exec_alu.OUT());
			sprintf(inst_output, "B:%02lx; IMM:%02lx", dx_b.value(),
				dx_imm.value());
			break;
	}
	
	// swap out the previous opc and return it for the next stage.
	long old_opc = x_prev_opc;
	x_prev_opc = opc;
	return old_opc;
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
	}
	
	// swap out the previous opc and return it for the next stage.
	long old_opc = m_prev_opc;
	m_prev_opc = opc;
	return old_opc;
}

void writeback( long opc ){

	//cout << "W\t" << opc << endl;
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
	}
}

void fetch_second(){

	//cout << "F2\t";
	inst_mem.read();
	fd_ir.latchFrom( inst_mem.READ() );
	pc.incr();
	
	// TODO modify BPT?
	if(false){
	
	}
	else{
		sprintf(inst_output, "No BPT Update");
	}
}

void decode_second(){
	
	//cout << "D2\t";

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
		case 15:
			sprintf(inst_output, "-");
			break;
	}
	dx_bus[2]->IN().pullFrom(fd_ir);
	dx_ir.latchFrom(dx_bus[2]->OUT());
}

void execute_second(){

	//cout << "X2\t";
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
	}
	
	xm_bus[0]->IN().pullFrom(dx_ir);
	xm_ir.latchFrom(xm_bus[0]->OUT());

}

void memory_second(){

	//cout << "M2\t";
	
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
	}
	
	mw_bus[1]->IN().pullFrom(xm_ir);
	mw_ir.latchFrom(mw_bus[1]->OUT());

}

void writeback_second(){

	//cout << "W2\t";
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
	}
}
