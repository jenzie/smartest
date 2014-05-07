// execute.C
//
// Instruction execution routine for the dummest little computer.

#include <cstdio>
#include "includes.h"

//
// execute() - decode and execute the instruction
//

void execute() {
	long opc;

	

	Clock::tick();
}

void print_contents() {
	printf(" %03lx   AC=%04lx", ir( DATA_BITS-4, 0 ), ac.value());
}
