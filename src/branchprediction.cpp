/**
 * branchprediction.cpp
 * 
 * Functions for performing branch prediction.
 */
 
#include "includes.h"

void init_table(){
	for( int i = 0; i < MAX_BPT; i++ ){
		bpt_ibank[i] = -1;
	}
}
 
int check_entry( long pc ){
	
	if( pc < 0 || pc >= MAX_BPT ){
		return -1;
	}
	
	for( int i = 0; i < MAX_BPT; i++ ){
		if(bpt_ibank[i] == pc){
			return i;
		}
	}
	return -1;
}

void add_entry( long pc, bool taken ){
	if(bpt_ibank[insert_index] != -1){
		total_branch_swapped++;
		
		// reset the history info with a clean predict-not-taken scheme
		history[insert_index][0] += taken ? 1 : 0;
		for( int i = 1; i < 6; i++){
			history[insert_index][i] = 1;
		}
	}
	
	bpt_ibank[insert_index] = pc;
	bpt_rbank[insert_index]->latchFrom(offset_alu.OUT());
	insert_index++;
}

void update_entry( long pc, bool taken ){
	long index = check_entry( pc );
	long previous = history[index][0];
	int add = taken ? 1 : -1;
	long counter = history[index][previous + 1] + add;
	
	// Update the saturation counter for the corresponding history entry
	// with a new value (0-3, bounding and inclusive).
	history[index][previous + 1] = counter > 3 ? 3 : counter < 0 ? 0 : counter;
	
	// We only need two bits of history; shift out the oldest entry and add
	// our new one.
	previous = ((previous << 1) & 3) + (taken ? 1 : 0);
	history[index][0] = previous;
}

void print_prediction( int index ){
	long previous = history[index][0];
	bool taken = history[index][previous + 1] > 1;
	sprintf(inst_output, "%s %02lx", taken ? "T" : "N",
		bpt_rbank[index]->value());
}
