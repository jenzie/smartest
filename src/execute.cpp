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
	cout << endl;
}

void fetch(){
	
	cout << "F\t" << endl;
	pc_bus.IN().pullFrom( pc );
	inst_mem.MAR().latchFrom( pc_bus.OUT() );
}

long decode(){

	long opc = fd_ir(DATA_BITS - 1, DATA_BITS - 4);
	d_curr_opc = opc; // for second stage

	cout << "D\t" << opc;
	
	switch( opc ) {
		case 0: // NOP
			break;
		case 1: // Add
			cout << "\tAdd";
	}
	cout << endl;

	// swap out the previous opc and return it for the next stage.
	long old_opc = d_prev_opc;
	d_prev_opc = opc;
	return old_opc;
}

long execute( long opc ){

	cout << "X\t" << opc << endl;

	// swap out the previous opc and return it for the next stage.
	long old_opc = x_prev_opc;
	x_prev_opc = opc;
	return old_opc;
}

long memory( long opc ){

	cout << "M\t" << opc << endl;

	// swap out the previous opc and return it for the next stage.
	long old_opc = m_prev_opc;
	m_prev_opc = opc;
	return old_opc;
}

void writeback( long opc ){

	cout << "W\t" << opc << endl;
}

void fetch_second(){

	cout << "F2\t";
	inst_mem.read();
	fd_ir.latchFrom( inst_mem.READ() );
	pc.incr();
	cout << endl;
}

void decode_second(){
	
	cout << "D2\t";

	switch( d_curr_opc ) {
		case 0: // NOP
			break;
		case 1: // Add
			cout << "\tAdd";
	}
	cout << endl;
}

void execute_second(){

	cout << "X2\t";
	
	cout << endl;
}

void memory_second(){

	cout << "M2\t";
	
	cout << endl;
}

void writeback_second(){

	cout << "W2\t";
	
	cout << endl;
}
