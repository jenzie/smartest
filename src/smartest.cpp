
#include "includes.h"

int main ( int argc, char *argv[] ) {

	// turn on data flow tracing and memory dump after load

	// CPUObject::debug |= CPUObject::trace | CPUObject::memload;

	// make sure we've been given an object file name

	if( argc != 2 ) {
		cerr << "Usage:  " << argv[0] << " object-file-name\n\n";
		exit( 1 );
	}

	cout << hex; // change base for future printing

	try {

		make_connections();

		run_simulation( argv[1] );

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
