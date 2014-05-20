
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
		case 0:  sprintf(inst_str, "NOP"); break; // NOP
		
		case 1:  sprintf(inst_str, "R%d = R%d + R%d", 
					reg_rd, reg_rs, reg_rt); break; // ADD
					
		case 2:  sprintf(inst_str, "R%d = R%d + %02lx",
					reg_rd, reg_rs, small_imm); break; // ADDI
					
		case 3:  sprintf(inst_str, "R%d = R%d - R%d",
					reg_rd, reg_rs, reg_rt); break; // SUB
					
		case 4:  sprintf(inst_str, "R%d = R%d & R%d",
					reg_rd, reg_rs, reg_rt); break; // AND
					
		case 5:  sprintf(inst_str, "R%d = R%d & R%d",
					reg_rd, reg_rs, reg_rt); break; // OR
					
		case 6:  sprintf(inst_str, "R%d = R%d XOR R%d",
					reg_rd, reg_rs, reg_rt); break; // XOR
					
		case 7:  sprintf(inst_str, "R%d = R%d << R%d",
					reg_rd, reg_rs, reg_rt); break; // SLL
					
		case 8:  sprintf(inst_str, "R%d = R%d >> R%d",
					reg_rd, reg_rs, reg_rt); break; // SRL
					
		case 9:  sprintf(inst_str, "R%d = R%d >>_a R%d",
					reg_rd, reg_rs, reg_rt); break; // SRA
					
		case 10:  sprintf(inst_str, "MEM[R%d+%02lx]<-R%d",
					reg_rt, small_imm, reg_rs); break; // SB
					
		case 11: sprintf(inst_str, "R%d<-MEM[R%d+%02lx]", 
					reg_rs, reg_rt, small_imm); break; // LB
					
		case 12:  sprintf(inst_str, "if R%d=0, PC<-EA",
					reg_rs); break; // BEZ
					
		case 13:  sprintf(inst_str, "if R%d!=0, PC<-EA",
					reg_rs); break; // BNE
					
		case 14: sprintf(inst_str, "JUMP %02lx", large_imm); break; // JUMP
		
		case 15: sprintf(inst_str, "HALT"); break; // HALT
		
		default: sprintf(inst_str, "UNKNOWN OPCODE"); break; // UNKNOWN
	}
}