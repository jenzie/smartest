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
	printf("Start Next Instruction\n");
	fetch();
	opc = decode();
	opc = execute( opc );
	opc = memory( opc );
	writeback( opc );
	Clock::tick();
	
	// Execute the second tick for each stage.
	fetch_second();
	decode_second();
	execute_second();
	memory_second();
	writeback_second();
	Clock::tick();
	
	printf("Registers %02lx %02lx %02lx %02lx\n",reg_file[0]->value(),
		reg_file[1]->value(), reg_file[2]->value(), reg_file[3]->value());
	
	cout << endl;
}

void fetch(){
	
	//cout << "F\t" << endl;
	pc_bus.IN().pullFrom( pc );
	inst_mem.MAR().latchFrom( pc_bus.OUT() );
}

long decode(){

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
			printf("D\tNOP\n");
			break;
		case 1: // Add
			printf("D\tR%d = R%d + R%d\n", reg_rd, reg_rs, reg_rt);
			write_reg = reg_rd;
			dx_bus[0]->IN().pullFrom(*reg_file[reg_rs]);
			dx_bus[1]->IN().pullFrom(*reg_file[reg_rt]);
			dx_a.latchFrom(dx_bus[0]->OUT());
			dx_b.latchFrom(dx_bus[1]->OUT());
			break;
		case 10:
			printf("D\tR%d = MEM[R%d + %lx]\n", reg_rs, reg_rt, small_imm);
			dx_bus[0]->IN().pullFrom(*reg_file[reg_rt]);
			small_bus.IN().pullFrom(fd_ir);
			dx_b.latchFrom(dx_bus[0]->OUT());
			dx_imm.latchFrom(small_bus.OUT());
			write_reg = reg_rs;
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
	
	switch( opc ) {
		case 0: // NOP
			break;
		case 1: // Add
			exec_alu.OP1().pullFrom(dx_a);
			exec_alu.OP2().pullFrom(dx_b);
			exec_alu.perform(BusALU::op_add);
			xm_alu_out.latchFrom(exec_alu.OUT());
			break;
		case 10:
			exec_alu.OP1().pullFrom(dx_b);
			exec_alu.OP2().pullFrom(dx_imm);
			exec_alu.perform(BusALU::op_add);
			xm_alu_out.latchFrom(exec_alu.OUT());
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
	
	switch( opc ) {
		case 0: // NOP
			break;
		case 1: // Add
			mw_bus[0]->IN().pullFrom(xm_alu_out);
			mw_alu_out.latchFrom(mw_bus[0]->OUT());
			break;
		case 10:
			printf("M\tRead MEM[%02lx]\n", xm_alu_out.value());
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
	
	switch( opc ) {
		case 0: // NOP
			break;
		case 1: // Add
			break;
		case 10:
		
			break;
	}
}

void fetch_second(){

	//cout << "F2\t";
	inst_mem.read();
	fd_ir.latchFrom( inst_mem.READ() );
	pc.incr();
}

void decode_second(){
	
	//cout << "D2\t";

	switch( d_curr_opc ) {
		case 0: // NOP
			break;
		case 1: // Add
			//cout << "\tAdd";
			break;
		case 10:
		
			break;
	}
}

void execute_second(){

	//cout << "X2\t";
	switch(x_curr_opc){
		case 1:
			printf("X\tALU_OUT = %02lx\n", xm_alu_out.value() );
			break;
		case 10:
			printf("X\tEA = %02lx\n", xm_alu_out.value() );
			break;
	}
}

void memory_second(){

	//cout << "M2\t";
	
	switch( m_curr_opc ) {
		case 0: // NOP
			break;
		case 10:
			mw_mdr.latchFrom(data_mem.READ());
			data_mem.read();
			break;
	}
}

void writeback_second(){

	//cout << "W2\t";
	
	switch( w_curr_opc ) {
		case 0: // NOP
			break;
		case 1: // Add
			printf("W\tR%lx = %02lx\n", write_reg, mw_alu_out.value());
			wd_bus.IN().pullFrom(mw_alu_out);
			reg_file[write_reg]->latchFrom(wd_bus.OUT());
			break;
		case 10:
			printf("W\tR%lx = %02lx\n", write_reg, mw_mdr.value());
			wd_bus.IN().pullFrom(mw_mdr);
			reg_file[write_reg]->latchFrom(wd_bus.OUT());
			break;
		case 15:
			halt_inst = true;
			done = true;
	}
}
