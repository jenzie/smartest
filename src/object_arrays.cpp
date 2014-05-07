
#include "includes.h"

/**
 * Convenience function for creating a large number of Arch Objects.
 * Each object is given a name and appended with its index (to help with
 * debugging.
 */
void build_object_arrays(){
	
	// Used by everyone to give the created object a name
	char* name;
	
	// The BPT needs a large number of registers to predict where to jump.
	bpt_rbank = ( StorageObject** ) malloc( 16 * sizeof( StorageObject* ) );
	for( int i = 0; i < 16; i++ ){
		name = ( char* ) malloc( 8 );
		sprintf( name, "rbank_%i", i );
		bpt_rbank[ i ] = new StorageObject( name, ADDR_BITS );
	}
	
	// The register file is a simple block of four registers
	reg_file = ( StorageObject** ) malloc( 4 * sizeof( StorageObject* ) );
	for( int i = 0; i < 4; i++ ){
		name = ( char* ) malloc( 5 );
		sprintf( name, "Reg_", i );
		reg_file[ i ] = new StorageObject( name, DATA_BITS );
	}
	
	// The BPT needs a large number of registers to predict where to jump.
	dx_bus = ( Bus** ) malloc( 4 * sizeof( Bus* ) );
	for( int i = 0; i < 4; i++ ){
		name = ( char* ) malloc( 4 );
		sprintf( name, "dx_%i", i );
		dx_bus[ i ] = new Bus( name, DATA_BITS );
	}
	
	// The BPT needs a large number of registers to predict where to jump.
	xm_bus = ( Bus** ) malloc( 3 * sizeof( Bus* ) );
	for( int i = 0; i < 3; i++ ){
		name = ( char* ) malloc( 4 );
		sprintf( name, "xm_%i", i );
		xm_bus[ i ] = new Bus( name, DATA_BITS );
	}
	
	// The BPT needs a large number of registers to predict where to jump.
	mw_bus = ( Bus** ) malloc( 3 * sizeof( Bus* ) );
	for( int i = 0; i < 3; i++ ){
		name = ( char* ) malloc( 4 );
		sprintf( name, "mw_%i", i );
		mw_bus[ i ] = new Bus( name, DATA_BITS );
	}
}
