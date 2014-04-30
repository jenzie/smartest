// run_simulation.C
//
// Original program:	Jim Heliotis, March 2000
// Updates:
//
//	2000/11/29:	wrc, restructured code
//	2001/12/10:	wrc, modified simulation control
//
// Primary execution loop for the dummest little computer simulator.

#include <cstdio>
#include "includes.h"

void run_simulation( char *objfile ) {

	// step 1:  load the object file

	m.load( objfile );

	// step 2:  set the entry point

	pc.latchFrom( m.READ() );
	Clock::tick();

	// step 3:  continue fetching, decoding, and executing instructions
	// until we stop the simulation

	while( !done && !halt_inst) {

		// instruction fetch
		fetch_into( pc, abus, mdr );

		// Check for PC overflow
		if(pc.value() == 0xfff){
			cout << endl << "MACHINE HALTED due to PC overflow" << endl << endl;
			done = true;
			break;
		}
		
		printf("%03x:  %04x = ", pc.value(), mdr.value());
		
		
		// bump PC for next instruction
		pc.incr();
		Clock::tick();

		// move instruction into IR
		dbus.IN().pullFrom( mdr );
		ir.latchFrom( dbus.OUT() );
		Clock::tick();

		// decode and execute
		execute();
		cout << endl;
	}
	
	if( halt_inst ) {
		cout << endl << "MACHINE HALTED due to halt instruction" << endl << endl;
	}
}
