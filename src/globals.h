// globals.h
//
// Extern declarations of global variables for the dummest little computer.

#ifndef _GLOBALS_H
#define _GLOBALS_H

#include "includes.h"

extern const unsigned int ADDR_BITS; // 8-bit address => 256 units of memory
extern const unsigned int DATA_BITS; // 12-bit unit size

extern Bus abus; // bus to carry memory addresses
extern Bus dbus; // bus to carry data and instructions

extern StorageObject ir;  // instruction register
extern Counter mdr; 	  // memory data register
extern Clearable ac;      // accumulator
extern Memory m;          // primary memory
extern BusALU alu;        // arithmetic logic unit

extern bool done;	  			// is the simulation over?
extern bool halt_inst;    		// did we halt because of the halt instruction?

// Smartest Start

// Fetch
extern long bpt_ibank[16];
extern StorageObject** bpt_rbank;
extern Counter pc;
extern Bus pc_bus;
extern Memory inst_mem;

// Fetch -> Decode
extern StorageObject fd_ir;
extern StorageObject fd_pc;
extern StorageObject fd_npc;
extern Bus fetch_bus;

// Decode
extern BusALU offset_alu;
extern BusALU comp_alu;
extern StorageObject** reg_file;
extern Bus decode_branch_bus;

// Decode -> Execute
extern StorageObject dx_ir;
extern StorageObject dx_imm;
extern StorageObject dx_a;
extern StorageObject dx_b;
extern Bus** dx_bus;

// Execute
extern BusALU exec_alu;

// Execute -> Memory
extern StorageObject xm_ir;
extern StorageObject xm_b;
extern StorageObject xm_alu_out;
extern Bus** xm_bus;

// Memory
extern Memory data_mem;

// Memory -> Writeback
extern StorageObject mw_ir;
extern StorageObject mw_mdr;
extern StorageObject mw_alu_out;
extern Bus** mw_bus;

// Writeback
// N/A

// Writeback -> Regfile
extern Bus wd_bus;

#endif
