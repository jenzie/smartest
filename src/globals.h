/**
 * globals.h
 *
 * Extern declarations of global variables for the dummest little computer.
 */

#ifndef _GLOBALS_H
#define _GLOBALS_H

#include "includes.h"

extern const unsigned int ADDR_BITS; // 8-bit address => 256 units of memory
extern const unsigned int DATA_BITS; // 12-bit unit size
extern const unsigned int SIMM_BITS;
extern const unsigned int MAX_BPT;

extern bool done;		// is the simulation over?
extern bool halt_inst;    // did we halt because of the halt instruction?

// Constant Registers
extern StorageObject const_one;

// Printing Data
extern char inst_str[16];
extern char inst_output[16];
extern char inst_total_output[80];
extern char debug_msg[80];
extern bool reg_changed;

// BPT Control
extern int insert_index;
extern int history[16][6];
extern bool bpt_update;
extern bool bpt_hit;
extern bool bpt_taken;
extern bool bpt_jump;
extern bool bpt_predicted;
extern bool bpt_index;
extern bool bubble;

extern bool bpt_d_update;
extern bool bpt_d_hit;
extern bool bpt_d_taken;
extern bool bpt_d_jump;
extern bool bpt_d_predicted;
extern bool bpt_d_index;

// BPT statistics
extern int predictions;
extern int failures;
extern int total_branch_swapped;

// Previous OPC's for passing between stages of the pipeline
extern long d_prev_opc;
extern long x_prev_opc;
extern long m_prev_opc;

extern long d_curr_opc;
extern long x_curr_opc;
extern long m_curr_opc;
extern long w_curr_opc;

// Fetch
extern long bpt_ibank[16];
extern StorageObject** bpt_rbank;
extern Counter pc;
extern Bus pc_bus;
extern Memory inst_mem;
extern bool incr_override;
extern BusALU incr_alu;
extern Bus predict_bus;
extern Bus inst_bus;

// Fetch -> Decode
extern StorageObject fd_ir;
extern StorageObject fd_pc;
extern StorageObject fd_npc;
extern StorageObject fd_nop;
extern Bus fetch_bus;

// Decode
extern BusALU offset_alu;
extern BusALU comp_alu;
extern StorageObject** reg_file;
extern Bus decode_branch_bus;

// Decode -> Execute
extern StorageObject dx_ir;
extern StorageObject dx_pc;
extern StorageObject dx_imm;
extern StorageObject dx_a;
extern StorageObject dx_b;
extern StorageObject dx_c;
extern Bus** dx_bus;
extern Bus small_bus;
extern Bus large_bus;

// Execute
extern BusALU exec_alu;

// Execute -> Memory
extern StorageObject xm_ir;
extern StorageObject xm_b;
extern StorageObject xm_c;
extern StorageObject xm_alu_out;
extern Bus** xm_bus;

// Memory
extern Memory data_mem;

// Memory -> Writeback
extern StorageObject mw_ir;
extern StorageObject mw_mdr;
extern StorageObject mw_c;
extern StorageObject mw_alu_out;
extern Bus** mw_bus;

// Writeback
// N/A

// Writeback -> Regfile
extern Bus wd_bus;

#endif
