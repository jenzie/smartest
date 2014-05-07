// execute.C
//
// Instruction execution routine for the dummest little computer.

#include <cstdio>
#include "includes.h"

//
// execute() - Moves instructions down through the pipeline
//

void execute() {
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
	
	cout << "Fetch executing " << endl;
	pc_bus.IN().pullFrom( pc );
	inst_mem.MAR().latchFrom( pc_bus.OUT() );
}

long decode(){

	long opc = fd_ir.value();
	d_curr_opc = opc; // for second stage

	cout << "Decode executing " << opc;
	
	switch( opc ) {
		case 0: // NOP
			break;
		case 1: // Add
			cout << " Add";
	}
	cout << endl;

	// swap out the previous opc and return it for the next stage.
	long old_opc = d_prev_opc;
	d_prev_opc = opc;
	return old_opc;
}

long execute( long opc ){

	cout << "Execute executing " << opc << endl;

	// swap out the previous opc and return it for the next stage.
	long old_opc = x_prev_opc;
	x_prev_opc = opc;
	return old_opc;
}

long memory( long opc ){

	cout << "Memory executing " << opc << endl;

	// swap out the previous opc and return it for the next stage.
	long old_opc = m_prev_opc;
	m_prev_opc = opc;
	return old_opc;
}

void writeback( long opc ){

	cout << "Writeback executing " << opc << endl;
}

void fetch_second(){

	inst_mem.read();
	fd_ir.latchFrom( inst_mem.READ() );
	pc.incr();
}

void decode_second(){

	switch( d_curr_opc ) {
		case 0: // NOP
			break;
		case 1: // Add
			cout << " Add" << endl;
	}
}

void execute_second(){

}

void memory_second(){

}

void writeback_second(){

}
