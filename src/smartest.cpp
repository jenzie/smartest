
#include "includes.h"

int main ( int argc, char *argv[] ) {

	// turn on data flow tracing and memory dump after load

	// CPUObject::debug |= CPUObject::trace | CPUObject::memload;

	// make sure we've been given an object file name

	if( argc != 3 ) {
		cerr << "Usage:  " << argv[0] << " inst-obj data-obj\n\n";
		exit( 1 );
	}

	cout << hex; // change base for future printing

	try {

		// Define our object arrays
		build_object_arrays();
	
		// Connect all the Arch component together
		make_connections();

		// Begin the simulation of the Arch CPU
		run_simulation( argv[1], argv[2] );

	}

	catch( ArchLibError &err ) {

		cout << endl
		     << "Simulation aborted - ArchLib runtime error"
		     << endl
		     << "Cause:  " << err.what() << endl;
		return( 1 );
	
	}

	return( 0 );
}
