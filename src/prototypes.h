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
long parse_imm( StorageObject &, bool );
long parse_ea( StorageObject &, bool );
long parse_rs( StorageObject & );
long parse_rt( StorageObject & );
long parse_rd( StorageObject & );
long get_reg_value( int );
long parse_opc( StorageObject & );
bool can_branch( StorageObject & );

// BPT
void init_table();
int check_entry( long );
void add_entry( long, bool );
void update_entry( long, bool );
bool predict_taken( int );

// Stages
void fetch();
long decode();
void decode_R_TYPE();
void decode_I_TYPE();
long execute( long );
void execute_R_TYPE( BusALU::Operation );
void execute_I_TYPE();
long memory( long );
void memory_R_TYPE();
void memory_I_TYPE();
void writeback( long );

void fetch_second();
void decode_second();
void execute_second();
void memory_second();
void writeback_second();

#endif
