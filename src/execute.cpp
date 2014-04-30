// execute.C
//
// Instruction execution routine for the dummest little computer.

#include <cstdio>
#include "includes.h"

//
// load_to_ac() - load value from memory at address given to ac.
//

void load_to_ac() {

	// Grab the value to load from in IR(11-0)
	fetch_into( ir, abus, mdr );
	
	// move the value into the AC
	dbus.IN().pullFrom( mdr );
	ac.latchFrom( dbus.OUT() );
}

//
// store_to_mem() - store value in ac to memory at address given.
//

void store_to_mem() {
	
	// Transfer IR(11-0) to MAR for storage loc
	abus.IN().pullFrom( ir );
	m.MAR().latchFrom( abus.OUT() );
	
	// give AC to MDR for transfer to Mem
	dbus.IN().pullFrom( ac );
	mdr.latchFrom( dbus.OUT() );
	Clock::tick();
	
	m.WRITE().pullFrom( mdr );
	m.write();
}

//
// increment_and_skip_if_zero() - increments the value in memory at the address
//		given.  If the result is zero, then the next instruction is skipped.
//

void increment_and_skip_if_zero() {
	
	// Grab the value to load from in IR(11-0)
	fetch_into( ir, abus, mdr );
	mdr.incr();
	Clock::tick();
	m.WRITE().pullFrom( mdr );
	m.write();
	
	// Only skip if the value incremented is zero.
	if( mdr.value() == 0 ) {
		pc.incr();
	}
}

//
// jump() - replaces the PC with the address given in the instruction.
//

void jump() {
	abus.IN().pullFrom( ir );
	pc.latchFrom( abus.OUT() );
}

//
// halt() - prevents execution of further instructions.
//

void halt() {
	halt_inst = true;
	done = true;
}

//
// branch_if_ac_zero() - replaces PC with the given address if AC equals zero.
//

void branch_if_ac_zero() {
	if( ac.value() == 0 ) {
		jump();
	}
}

//
// add_to_ac() - add from memory to the accumulator
//

void add_to_ac() {

	// get memory operand using address in IR[11,0]
	fetch_into( ir, abus, mdr );

	// set up the addition
	alu.OP1().pullFrom( ac );
	alu.OP2().pullFrom( mdr );
	ac.latchFrom( alu.OUT() );
	alu.perform( BusALU::op_add );

}

//
// swap_mem_and_ac() - swaps the value of memory at the given address and the
//		the value in AC.
//

void swap_mem_and_ac() {

	// Perform the parallel swap with the ALU and DBUS
	fetch_into( ir, abus, mdr );
	
	// parallel swap of AC and MDR
	alu.OP2().pullFrom( mdr );
	ac.latchFrom( alu.OUT() );
	dbus.IN().pullFrom( ac );
	mdr.latchFrom( dbus.OUT() );
	alu.perform( BusALU::op_rop2 );
	Clock::tick();
	
	m.WRITE().pullFrom( mdr );
	m.write();
}

//
// execute() - decode and execute the instruction
//

void execute() {
	long opc;

	// in each case, note that the last set of operations
	// aren't actually performed until we leave the switch
	// statement
	//
	// instr format:  opcode = bits 14:12, address = bits 11:0

	opc = ir( DATA_BITS-1, DATA_BITS-3 );

	switch( opc ) {

		case 0:	cout << " LOAD"; print_contents(); load_to_ac();  break;
		
		case 1:	cout << "STORE"; print_contents(); store_to_mem();  break;
		
		case 2:	cout << "  ISZ"; print_contents(); increment_and_skip_if_zero();  break;
		
		case 3:	cout << " JUMP"; print_contents(); jump();  break;
		
		case 4:	cout << " HALT"; print_contents(); halt();  break;
		
		case 5:	cout << " BZAC"; print_contents(); branch_if_ac_zero();  break;

		case 6:	cout << "  ADD"; print_contents(); add_to_ac();  break;
		
		case 7:	cout << "  SWP"; print_contents(); swap_mem_and_ac();  break;

		default:
			cout << endl << "MACHINE HALTED due to unknown op code " << endl << endl;
			done = true;
	}
	
	Clock::tick();
}

void print_contents() {
	printf(" %03x   AC=%04x", ir( DATA_BITS-4, 0 ), ac.value());
}
