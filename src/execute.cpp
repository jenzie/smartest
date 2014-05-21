/**
 * execute.C
 *
 * Instruction execution routine for the dummest little computer.
 */

#include <cstdio>
#include "includes.h"

#define DEBUG_

/**
 * Moves instructions down through the pipeline.
 */
void assemble() {
	long opc;
	
	debug_msg[0] = 0;

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
	
	#ifdef DEBUG
	printf("PC:%02lx\n", pc.value() );
	#endif
	
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
	
	#ifdef DEBUG
	printf("PC:%02lx\n", pc.value() );
	
	if(debug_msg[0] != 0)
		printf("%s\n", debug_msg);
	#endif
	
	if( reg_changed ){
		printf("Registers %02lx %02lx %02lx %02lx\n",reg_file[0]->value(),
			reg_file[1]->value(), reg_file[2]->value(), reg_file[3]->value());
	}
	
	#ifdef DEBUG
	if( bpt_update || bpt_d_update || bpt_hit || bpt_d_hit){
		for(int i = 0; i < MAX_BPT; i++){
			if(bpt_ibank[i] != -1)
				printf("%d:%02lx->%02lx,%d[%d,%d,%d,%d]  ", i, 
					bpt_ibank[i], bpt_rbank[i]->value(),
					history[i][0], history[i][1], history[i][2], history[i][3],
					history[i][4]);
		}
		printf("\n");
	}
	#endif
	
	// Print BPT table
	if( halt_inst == true ){
		printf("\nBPT Statistics\n==============\n");
		int total_queries = predictions + failures;
		printf("Predictions:\t%02d\nFailures:\t%02d\n", predictions, failures);
		printf("Accuracy:\t%02.0f%%\n",
			total_queries == 0 ? 0
			: (predictions - failures + 0.0f) / predictions * 100.0);
		printf("Branch Swaps:\t%02d\nSaturation:\t%02.2f\n",
			total_branch_swapped,
			(total_branch_swapped + MAX_BPT) / (double) MAX_BPT );;
	}
	
	cout << endl;
}

void fetch(){
	
	sprintf(inst_str, "Read PC(%02lx)", pc.value() );
	
	// Reset all the flags
 	incr_override = false;
	bpt_update = false;
	bpt_hit = false;
	bpt_taken = false;
	bpt_jump = false;
	bpt_predicted = false;
	
	// Carry our PC to the next stage
	fd_pc.latchFrom( fetch_bus.OUT() );
	fetch_bus.IN().pullFrom( pc );
	
	int index = check_entry( pc.value() );
	bpt_index = index;
	if( index != -1 ){
		bpt_hit = true;
		predictions++;
		print_prediction( index );
		bpt_predicted = predict_taken( index );
		if( bpt_predicted ){
			sprintf(inst_output, "Pred Taken");
			incr_override = true;
			pc_bus.IN().pullFrom( *bpt_rbank[ index ] );
			pc.latchFrom( pc_bus.OUT() );
			
			// Store the current PC incremented in case we are wrong.
			fd_npc.latchFrom( incr_alu.OUT() );
			incr_alu.OP1().pullFrom( pc );
			incr_alu.OP2().pullFrom( const_one );
			incr_alu.perform( BusALU::op_add );
		}
		else{
			sprintf(inst_str, "Read PC(%02lx)", pc.value());
			sprintf(inst_output, "Pred Not Taken");
			pc_bus.IN().pullFrom( pc );
		}
	}
	else{
		sprintf(inst_str, "Read PC(%02lx)", pc.value());
		pc_bus.IN().pullFrom( pc );
		bpt_hit = false;
		sprintf(inst_output, "No BPT Hit");
	}
	
	inst_bus.IN().pullFrom( pc );
	inst_mem.MAR().latchFrom( inst_bus.OUT() );
}

long decode(){

	long opc = fd_ir(DATA_BITS - 1, DATA_BITS - 4);
	d_curr_opc = opc; // for second clock tick
	dx_pc.latchFrom( dx_bus[3]->OUT() );
	dx_bus[3]->IN().pullFrom( fd_pc );

	make_inst_str(fd_ir);
	
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
			sprintf(inst_output, "%02lx == 0?", reg_file[reg_rs]->value());
			if( reg_file[reg_rs]->value() == 0 ){
				bpt_taken = true;
				dx_imm.latchFrom( small_bus.OUT() );
				small_bus.IN().pullFrom( fd_ir );
			}
			bpt_d_update = true;
			break;
		case 13: // BNZ
			sprintf(inst_output, "%02lx != 0?", reg_file[reg_rs]->value());
			if( reg_file[reg_rs]->value() != 0 ){
				bpt_d_taken = true;
				dx_imm.latchFrom( small_bus.OUT() );
				small_bus.IN().pullFrom( fd_ir );
			}
			bpt_d_update = true;
			break;
		case 14: // Jump
			dx_imm.latchFrom( large_bus.OUT() );
			large_bus.IN().pullFrom( fd_ir );
			sprintf(inst_output, "imm<-%02lx", large_imm);
			incr_override = true;
			bpt_d_jump = true;
			bpt_d_update = true;
			bpt_d_taken = true;
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
	sprintf(inst_output, "a<-%02lx imm<-%02lx",
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
	xm_bus[0]->IN().pullFrom(dx_a);
	xm_a.latchFrom(xm_bus[0]->OUT());
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
		case 1: // ADD
			memory_R_TYPE(); break;
		case 2: // ADDI
			memory_I_TYPE(); break;
		case 3: // SUB
			memory_R_TYPE(); break;
		case 4: // AND
			memory_R_TYPE(); break;
		case 5: // OR
			memory_R_TYPE(); break;
		case 6: // XOR
			memory_R_TYPE(); break;
		case 7: // SLL
			memory_R_TYPE(); break;
		case 8: // SRL
			memory_R_TYPE(); break;
		case 9: // SRA
			memory_R_TYPE(); break;
		case 10: // SB
			sprintf(inst_output, "MEM[ALU_OUT]");
			mw_bus[0]->IN().pullFrom(xm_alu_out);
			data_mem.MAR().latchFrom(mw_bus[0]->OUT());
			break;
		case 11: // LB
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

void memory_R_TYPE(){
	mw_bus[0]->IN().pullFrom(xm_alu_out);
	mw_alu_out.latchFrom(mw_bus[0]->OUT());
	sprintf(inst_output, "passthru");
}

void memory_I_TYPE(){
	memory_R_TYPE();
}

void writeback( long opc ){

	w_curr_opc = opc; // for second clock tick
	make_inst_str(mw_ir);
	
	switch( opc ) {
		default:
			sprintf(inst_output, "-");
	}
}

void fetch_second(){

	bool bad_pc = false;
	
	#ifdef DEBUG
	sprintf(debug_msg, "%s P:%d T:%d", 
		debug_msg, bpt_d_predicted, bpt_d_taken);
	#endif

	// Actually fetch the next instruction.
	// Bad PC, need to flush and replace with the incremented PC
	if( bpt_d_predicted && !bpt_d_taken ){
		sprintf(inst_output, "PC? ");
		fetch_bus.IN().pullFrom( fd_nop );
		fd_ir.latchFrom( fetch_bus.OUT() );
		pc.latchFrom( pc_bus.OUT() );
		pc_bus.IN().pullFrom( fd_npc );
		bad_pc = true;
	}
	
	// Bad PC, need to flush and replace with the predicted PC
	else if( bpt_d_taken && !bpt_d_predicted ){
		sprintf(inst_output, "PC? ");
		fetch_bus.IN().pullFrom( fd_nop );
		fd_ir.latchFrom( fetch_bus.OUT() );
		pc.latchFrom( offset_alu.OUT() );
		bad_pc = true;
	}
	
	// Normal case, where we just want to increment the PC
	else{
		sprintf(inst_output, "PC+ ");
		inst_mem.read();
		fd_ir.latchFrom( inst_mem.READ() );
	}
	
	// If we didn't get a hit, but we do have a branch, this is a new entry
	if( bpt_d_update && !bpt_d_hit && check_entry( dx_pc.value() ) == -1 ){
		sprintf(inst_output, "%sB%02d<-PC @%02lx", inst_output,
			insert_index,
			dx_pc.value() );
		add_entry( dx_pc.value(), bpt_d_taken );
	}
	// If we did get a hit, then we are updating an existing entry.
	else if( bpt_d_update && bpt_d_hit ){
		sprintf(inst_output, "%sA:%s, P:%s", inst_output, 
			bpt_d_taken ? "T" : "F", bpt_d_predicted ? "T" : "F");
		update_entry( dx_pc.value(), bpt_d_taken );
		if( bpt_d_predicted != bpt_d_taken ){
			failures++;
		}
	}
	else{
		sprintf(inst_output, "-");
	}
	
	if( !bad_pc && !incr_override){
		pc.incr();
	}
}

void decode_second(){

	int reg_rt = parse_rt( fd_ir );
	
	switch( d_curr_opc ) {
		case 12: // fall through; print case 13
		case 13:
		
			// If we predicted not-taken, and we are taking, then we need to
			// override the current PC.
			if( bpt_d_taken && !bpt_d_predicted ){
				sprintf(inst_output, "pc<-%02lx", parse_ea( fd_ir, false ) );
				offset_alu.OP1().pullFrom( dx_imm );
				offset_alu.OP2().pullFrom( *reg_file[ parse_rt( fd_ir ) ] );
				offset_alu.perform( BusALU::op_add );
			}
			else{
				sprintf(inst_output, "-");
			}
			break;
		case 14:
		
			// if we didn't predict this jump would happen, we need to make it
			// happen
			if( !bpt_d_predicted ){
				offset_alu.OP1().pullFrom( dx_imm );
				offset_alu.perform( BusALU::op_rop1 );
				pc.latchFrom( offset_alu.OUT() );
				sprintf(inst_output, "pc<-%02lx", dx_imm.value());
			}
			break;
		default:
			sprintf(inst_output, "-");
	}
	
	// If our prediction was bad, the current IR is bad; bubble with NOP 
	// if( bpt_predicted != bpt_taken ){
		// dx_bus[2]->IN().pullFrom( fd_nop );
	// }
	// else{
		dx_bus[2]->IN().pullFrom( fd_ir );
	// }
	dx_ir.latchFrom( dx_bus[2]->OUT() );
	
	// Grab the previous fetch's prediction state for the next decode phase
	bpt_d_update = bpt_update;
	bpt_d_hit = bpt_hit;
	bpt_d_taken = bpt_taken;
	bpt_d_jump = bpt_jump;
	bpt_d_predicted = bpt_predicted;
	bpt_d_index = bpt_index;
}

void execute_second(){

	switch(x_curr_opc){
		case 0:  // fall through; print case 15
		case 12:
		case 13:
		case 14:
		case 15:
			sprintf(inst_output, "-");
			break;
		default:
			sprintf(inst_output, "ALU_OUT<-%02lx", xm_alu_out.value());
	}
	
	xm_bus[0]->IN().pullFrom(dx_ir);
	xm_ir.latchFrom(xm_bus[0]->OUT());

}

void memory_second(){

	switch( m_curr_opc ) {
		case 10:
			data_mem.WRITE().pullFrom(xm_a);
			data_mem.write();
			sprintf(inst_output, "MEM[%02lx]<-A", xm_alu_out.value());
			break;
		case 11:
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
		case 10:
			sprintf(inst_output, "R%lx<-%02lx",
				mw_ir(DATA_BITS - 5, DATA_BITS - 6), mw_mdr.value());
			wd_bus.IN().pullFrom(mw_mdr);
			reg_file[
				mw_ir(DATA_BITS - 5, DATA_BITS - 6)]->latchFrom(wd_bus.OUT());
			reg_changed = true;
			break;
		case 15:
			sprintf(inst_output, "Smartest Off");
			halt_inst = true;
			done = true;
			break;
		default:
			sprintf(inst_output, "R%lx<-%02lx",
				mw_ir(DATA_BITS - 9, DATA_BITS - 10), mw_alu_out.value());
			wd_bus.IN().pullFrom(mw_alu_out);
			reg_file[
				mw_ir(DATA_BITS - 9, DATA_BITS - 10)]->latchFrom(wd_bus.OUT());
			reg_changed = true;
	}
}
