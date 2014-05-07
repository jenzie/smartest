// run_simulation.C
//
// Primary execution loop for the dummest little computer simulator.

#include <cstdio>
#include "includes.h"

void run_simulation( char *instobj, char *dataobj ) {

	// step 1:  load the object file

	inst_mem.load( instobj );
	data_mem.load( dataobj );

	// step 2:  set the entry point

	pc.latchFrom( inst_mem.READ() );
	Clock::tick();

	// step 3:  continue fetching, decoding, and executing instructions
	// until we stop the simulation

	while( !done && !halt_inst) {

		// TODO: Check for PC overflow
		if(false){
			cout << endl << "MACHINE HALTED due to PC overflow" << endl << endl;
			done = true;
			break;
		}
		

		// Move instructions through the pipeline
		execute();
	}
	
	if( halt_inst ) {
		cout << endl << "MACHINE HALTED due to halt instruction" << endl << endl;
	}
}
