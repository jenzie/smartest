/**
 * prototypes.h
 *
 * Function prototypes for the dummest little computer.
 */

#ifndef _PROTOTYPES_H
#define _PROTOTYPES_H

void assemble();
void make_connections();
void run_simulation( char *, char * );
void build_object_arrays();

// Stages
void fetch();
long decode();
long execute( long );
long memory( long );
void writeback( long );

void fetch_second();
void decode_second();
void execute_second();
void memory_second();
void writeback_second();

#endif
