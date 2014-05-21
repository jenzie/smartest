/**
 * branchprediction.cpp
 * 
 * Functions for performing branch prediction.
 */
 
#include "includes.h"

void init_table(){
	for( int i = 0; i < MAX_BPT; i++ ){
		bpt_ibank[i] = -1;
		
		for( int k = 1; k < 6; k++){
			history[i][k] = 1;
		}
	}
}
 
int check_entry( long pc ){
	
	if( pc < 0 || pc >= 255 ){
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
	history[insert_index][0] = 0;
	history[insert_index][0] = 0;
	bpt_rbank[insert_index]->latchFrom(offset_alu.OUT());
	insert_index = (insert_index + 1) % MAX_BPT;
}

void update_entry( long pc, bool taken ){
	long index = check_entry( pc );
	long previous = history[index][0];
	int add = taken ? 1 : -1;
	long counter = history[index][previous + 1] + add;
	sprintf(debug_msg, "%s Prev: %lx", debug_msg, (counter - add));
	
	// Update the saturation counter for the corresponding history entry
	// with a new value (0-3, bounding and inclusive).
	history[index][previous + 1] = counter > 3 ? 3 : counter < 0 ? 0 : counter;
	sprintf(debug_msg, "%s New: %d", debug_msg, history[index][previous + 1]);
	
	// We only need two bits of history; shift out the oldest entry and add
	// our new one.
	previous = ((previous << 1) & 3) + (taken ? 1 : 0);
	history[index][0] = previous;
}

bool predict_taken( int index ){
	long previous = history[index][0];
	sprintf(debug_msg, "H:%lx P:%d", previous, history[index][previous + 1]);
	return history[index][previous + 1] > 1;
}

void print_prediction( int index ){
	bool taken = predict_taken( index );
	sprintf(inst_output, "%s %02lx", taken ? "T" : "N",
		bpt_rbank[index]->value());
}
