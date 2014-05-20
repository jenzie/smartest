
#include "includes.h"

/**
 * Utility functions.
 */
 
void make_inst_str(StorageObject &ir){
	
	// Extract the packed bits
	long opc = parse_opc( ir );
	int reg_rs = parse_rs( ir );
	int reg_rt = parse_rt( ir );
	int reg_rd = parse_rd( ir );
	long small_imm = parse_imm( ir, true );
	long large_imm = parse_imm( ir, false );

	switch(opc){
		case 0:  sprintf(inst_str, "NOP"); break;
		case 1:  sprintf(inst_str, "R%d = R%d + R%d", reg_rd, reg_rs, reg_rt); break;
		case 10: sprintf(inst_str, "R%d = MEM[R%d+%lx]", reg_rs, reg_rt, small_imm); break;
		case 14: sprintf(inst_str, "JUMP %02lx", large_imm); break;
		case 15: sprintf(inst_str, "HALT"); break;
	}
}

long parse_imm( StorageObject &ir, bool i_type ){
	return i_type ? ir(DATA_BITS - 9, DATA_BITS - 12) :
		ir(DATA_BITS - 5, DATA_BITS - 12);
}

long parse_ea( StorageObject &ir, bool jump ){
	return jump ? parse_imm( ir, false ) :
		get_reg_value( parse_rt( ir ) ) + parse_imm( ir, true );
}

long parse_rs( StorageObject &ir ){
	return ir(DATA_BITS - 5, DATA_BITS - 6);
}

long parse_rt( StorageObject &ir ){
	return ir(DATA_BITS - 7, DATA_BITS - 8);
}

long parse_rd( StorageObject &ir ){
	return ir(DATA_BITS - 9, DATA_BITS - 10);
}

long get_reg_value( int reg ){
	return reg_file[reg]->value();
}

long parse_opc( StorageObject &ir ){
	return ir(DATA_BITS - 1, DATA_BITS - 4);
}
