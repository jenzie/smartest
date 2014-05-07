// prototypes.h
//
// Function prototypes for the dummest little computer.

#ifndef _PROTOTYPES_H
#define _PROTOTYPES_H

void clear_ac();
void add_to_ac();
void execute();
void print_contents(); 
void fetch_into( StorageObject &, Bus &, StorageObject & );
void make_connections();
void run_simulation( char * );
void build_object_arrays();

#endif
