/**
 * globals.C
 * 
 * Global variables for the smartest little computer.
 */ 

#include "includes.h"


/**
 * Constants
 */
const unsigned int ADDR_BITS( 8 ); // 18-bit address => 256 units of memory
const unsigned int DATA_BITS( 12 ); // 12-bit unit size
const unsigned int SIMM_BITS( 4 ); // Small immediate width

/**
 * Runtime control variables
 */
bool done ( false );		// is the simulation over?
bool halt_inst ( false );    // did we halt because of the halt instruction?

/**
 * These will get passed onto the next pipeline stage at the following cycle.
 */
long d_prev_opc( 0 );
long x_prev_opc( 0 );
long m_prev_opc( 0 );

/**
 * These tell the second clock tick in a pipeline stage what that stage was
 * working on in the first clock tick.
 */
long d_curr_opc( 0 );
long x_curr_opc( 0 );
long m_curr_opc( 0 );
long w_curr_opc( 0 );

/**
 * Memory used by the Smartest Little Computer
 */
Memory inst_mem( "Inst_Memory", ADDR_BITS, DATA_BITS );
Memory data_mem( "Data_Memory", ADDR_BITS, DATA_BITS );

// Fetch
long bpt_ibank[16];
StorageObject** bpt_rbank;
Counter pc( "PC", ADDR_BITS );
Bus pc_bus( "PC_Bus", ADDR_BITS );

// Fetch -> Decode
StorageObject fd_ir( "FD_IR_Bus", DATA_BITS );
StorageObject fd_pc( "FD_PC", ADDR_BITS );
StorageObject fd_npc( "FD_NPC", ADDR_BITS );
Bus fetch_bus( "Fetch_Bus", ADDR_BITS );

// Decode
BusALU offset_alu( "Offset_ALU", ADDR_BITS );
BusALU comp_alu( "Comp_ALU", ADDR_BITS );
StorageObject** reg_file;
Bus decode_branch_bus( "Decode_Branch_Bus", ADDR_BITS );

// Decode -> Execute
StorageObject dx_ir( "DX_IR", DATA_BITS );
StorageObject dx_imm( "DX_IMM", ADDR_BITS );
StorageObject dx_a( "DX_A", DATA_BITS );
StorageObject dx_b( "DX_B", DATA_BITS );
StorageObject dx_c( "DX_C", DATA_BITS );
Bus** dx_bus;
Bus small_bus( "Small_Bus", SIMM_BITS );
Bus large_bus( "Large_Bus", ADDR_BITS );

// Execute
BusALU exec_alu( "EXEC_ALU", DATA_BITS );

// Execute -> Memory
StorageObject xm_ir( "XM_IR", DATA_BITS );
StorageObject xm_b( "XM_B", DATA_BITS );
StorageObject xm_c( "XM_C", DATA_BITS );
StorageObject xm_alu_out( "XM_ALU_OUT", DATA_BITS );
Bus** xm_bus;

// Memory
// N/A

// Memory -> Writeback
StorageObject mw_ir( "MW_IR", DATA_BITS );
StorageObject mw_mdr( "MW_MDR", DATA_BITS );
StorageObject mw_c( "MW_C", DATA_BITS );
StorageObject mw_alu_out( "MW_ALU_OUT", DATA_BITS );
Bus** mw_bus;

// Writeback
long write_reg(-1);

// Writeback -> Regfile
Bus wd_bus( "WD_Bus", DATA_BITS );
