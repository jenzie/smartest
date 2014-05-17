/**
 * make_connections.C
 *
 * Component connection function.  At execution time, this will
 * connect all the CPU components according to the design of
 * the architecture.
 */

#include "includes.h"

void make_connections() {

	// Fetch
	inst_mem.MAR().connectsTo( pc_bus.OUT() );
	pc.connectsTo( pc_bus.IN() );
	pc.connectsTo( pc_bus.OUT() );
	pc.connectsTo( inst_mem.READ() );
	for( int i = 0; i < 16; i++ ){
		bpt_rbank[ i ]->connectsTo( pc_bus.IN() );
		bpt_rbank[ i ]->connectsTo( offset_alu.OUT() );
	}
	
	fd_ir.connectsTo( inst_mem.READ() );
	pc.connectsTo( offset_alu.OUT() );
	
	// Decode
	fd_ir.connectsTo( offset_alu.OP1() );
	for( int i = 0; i < 4; i++ ){
		reg_file[ i ]->connectsTo( dx_bus[0]->IN() );
		reg_file[ i ]->connectsTo( dx_bus[1]->IN() );
		reg_file[ i ]->connectsTo( dx_bus[2]->IN() );
		reg_file[ i ]->connectsTo( dx_bus[3]->IN() );
		reg_file[ i ]->connectsTo( comp_alu.OP1() );
		reg_file[ i ]->connectsTo( comp_alu.OP2() );
		reg_file[ i ]->connectsTo( offset_alu.OP1() );
		reg_file[ i ]->connectsTo( offset_alu.OP2() );
		dx_a.connectsTo( dx_bus[i]->OUT() );
		dx_b.connectsTo( dx_bus[i]->OUT() );
		dx_c.connectsTo( dx_bus[i]->OUT() );
		fd_ir.connectsTo( dx_bus[i]->IN() );
		dx_ir.connectsTo( dx_bus[i]->OUT() );
	}
	fd_ir.connectsTo( small_bus.IN() );
	fd_ir.connectsTo( large_bus.IN() );
	dx_imm.connectsTo( small_bus.OUT() );
	dx_imm.connectsTo( large_bus.OUT() );
	dx_imm.connectsTo( offset_alu.OP1() );
	dx_imm.connectsTo( offset_alu.OP2() );
	pc.connectsTo( decode_branch_bus.OUT() );
	dx_imm.connectsTo( decode_branch_bus.IN() );
	
	// Execute
	xm_alu_out.connectsTo( exec_alu.OUT() );
	dx_a.connectsTo( exec_alu.OP1() );
	dx_a.connectsTo( exec_alu.OP2() );
	dx_b.connectsTo( exec_alu.OP1() );
	dx_b.connectsTo( exec_alu.OP2() );
	dx_imm.connectsTo( exec_alu.OP1() );
	dx_imm.connectsTo( exec_alu.OP2() );
	for( int i = 0; i < 3; i++ ){
		xm_b.connectsTo( xm_bus[ i ]->OUT() );
		dx_a.connectsTo( xm_bus[ i ]->IN() );
		dx_b.connectsTo( xm_bus[ i ]->IN() );
		dx_imm.connectsTo( xm_bus[ i ]->IN() );
		dx_ir.connectsTo( xm_bus[ i ]->IN() );
		xm_ir.connectsTo( xm_bus[ i ]->OUT() );
	}
	
	// Memory
	mw_mdr.connectsTo( data_mem.READ() );
	xm_b.connectsTo( data_mem.WRITE() );
	for( int i = 0; i < 3; i++ ){
		xm_alu_out.connectsTo( mw_bus[ i ]->IN() );
		data_mem.MAR().connectsTo( mw_bus[ i ]->OUT() );
		xm_alu_out.connectsTo( mw_bus[ i ]->IN() );
		mw_alu_out.connectsTo( mw_bus[ i ]->OUT() );
		xm_ir.connectsTo( mw_bus[ i ]->IN() );
		mw_ir.connectsTo( mw_bus[ i ]->OUT() );
	}
	
	// Writeback
	mw_alu_out.connectsTo( wd_bus.IN() );
	mw_mdr.connectsTo( wd_bus.IN() );
	for( int i = 0; i < 4; i++ ){
		reg_file[ i ]->connectsTo( wd_bus.OUT() );
	}
}
