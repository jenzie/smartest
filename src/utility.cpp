
#include "includes.h"

/**
 * Utility functions.
 */
 
void make_inst_str(StorageObject &ir){
	
	// Extract the packed bits
	long opc = ir(DATA_BITS - 1, DATA_BITS - 4);
	int reg_rs = ir(DATA_BITS - 5, DATA_BITS - 6);
	int reg_rt = ir(DATA_BITS - 7, DATA_BITS - 8);
	int reg_rd = ir(DATA_BITS - 9, DATA_BITS - 10);
	long small_imm = ir(DATA_BITS - 9, DATA_BITS - 12);
	long large_imm = ir(DATA_BITS - 5, DATA_BITS - 12);

	switch(opc){
		case 0:  sprintf(inst_str, "NOP"); break;
		case 1:  sprintf(inst_str, "R%d = R%d + R%d", reg_rd, reg_rs, reg_rt); break;
		case 10: sprintf(inst_str, "R%d = MEM[R%d+%lx]", reg_rs, reg_rt, small_imm); break;
		case 14: sprintf(inst_str, "JUMP %02lx", large_imm); break;
		case 15: sprintf(inst_str, "HALT"); break;
	}
}