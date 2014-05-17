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
void make_inst_str( StorageObject & );
void print_prediction( int index );

// BPT
void init_table();
int check_entry( long );
void add_entry( long, bool );
void update_entry( long, bool );

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
