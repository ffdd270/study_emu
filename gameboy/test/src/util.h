//
// Created by nhy20 on 2020-07-18.
//

#ifndef GAMEBOY_UTIL_H
#define GAMEBOY_UTIL_H


#include "GameboyCPU.h"
#include "catch.hpp"


enum Param8BitIndex
{
	B = 0b000,
	C = 0b001,
	D = 0b010,
	E = 0b011,
	H = 0b100,
	L = 0b101,
	MEM_HL = 0b110,
	A = 0b111
};

enum Register16BitIndex
{
	BC = 0b00,
	DE = 0b01,
	HL = 0b10,
	AF = 0b11,
};

inline void check_flags( GameboyCPU & cpu, bool z, bool h, bool n, bool c )
{
	BYTE result[4] = { cpu.GetFlagZ(), cpu.GetFlagH(), cpu.GetFlagN(), cpu.GetFlagC() };

	REQUIRE( ( result[0] == 1 ) == z );
	REQUIRE( ( result[1] == 1 ) == h );
	REQUIRE( ( result[2] == 1 ) == n );
	REQUIRE( ( result[3] == 1 ) == c );
}

inline void setRegister16( GameboyCPU & cpu_ref ,BYTE register_index, WORD value )
{
	BYTE hi = ( 0xFF00 & value ) >> 8; // hi byte,
	BYTE lo = 0x00FF & value; // lo byte

	cpu_ref.InjectionMemory( 0b00000001 | ( register_index << 4 ) );
	cpu_ref.InjectionMemory( lo );
	cpu_ref.InjectionMemory( hi );
}

inline void setRegister8( GameboyCPU & cpu_ref, BYTE register_index, BYTE value )
{
	cpu_ref.InjectionMemory( 0b00000110 | ( register_index << 3 ) ); // LD B, imm8
	cpu_ref.InjectionMemory( value ); // imm
}

inline void callSetRegister8( GameboyCPU & cpu_ref, BYTE register_index, BYTE value )
{
	setRegister8( cpu_ref, register_index, value );
	cpu_ref.NextStep();
}

// 3 clock.
inline void setMemory3Step(GameboyCPU & cpu_ref, BYTE reg8_index, WORD mem_addr, BYTE value )
{

	setRegister16(  cpu_ref, Register16BitIndex::HL, mem_addr ); // LD HL(10), imm16( mem_addr )
	//HL = mem_addr. 1 Step.

	setRegister8( cpu_ref, reg8_index, value ); // LD reg8_index, 0xBA
	//reg8_index = value. 2 Step.

	cpu_ref.InjectionMemory( 0b01110000 | reg8_index ); // LD (HL), reg8_index
	//(HL) = value  3 Step.
}

inline void callSetMemory3Step(GameboyCPU & cpu_ref, BYTE reg8_index, WORD mem_addr, BYTE value )
{
	setMemory3Step( cpu_ref, reg8_index, mem_addr, value );
	for( int i = 0; i < 3; i++ ) { cpu_ref.NextStep(); }
}




inline void base_op_code_n(GameboyCPU & cpu, BYTE op_code, BYTE a_value, BYTE n, bool only_injection = false )
{
	setRegister8(cpu, Param8BitIndex::A, a_value );
	cpu.InjectionMemory( op_code );
	cpu.InjectionMemory( n );

	if(!only_injection)
	{
		for ( int i = 0; i < 2; i++ ) { cpu.NextStep(); }
	}
}

inline void base_op_code_reg8(GameboyCPU & cpu, BYTE base_op_code, BYTE a_value, BYTE register_index, BYTE n, bool only_injection = false )
{
	setRegister8(cpu, Param8BitIndex::A, a_value );
	setRegister8( cpu, register_index, n );
	cpu.InjectionMemory( ( base_op_code | register_index )  );

	if(!only_injection)
	{
		for (int i = 0; i < 3; i++) { cpu.NextStep(); }
	}
}

inline void base_op_code_hl(GameboyCPU & cpu, BYTE op_code, BYTE a_value, WORD mem_hl_address, BYTE n, bool only_injection = false )
{
	setMemory3Step(cpu, Param8BitIndex::B, mem_hl_address, n );
	setRegister8(cpu, Param8BitIndex::A, a_value );
	cpu.InjectionMemory( op_code );

	if(!only_injection)
	{
		for (int i = 0; i < 5; i++)
		{
			cpu.NextStep();
		}
	}
}

// 0b11010110 (0xD6)
inline void call_subN(GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_n(cpu, 0xD6, a_value, n);
}

inline void subN( GameboyCPU & cpu, BYTE n )
{
	cpu.InjectionMemory( 0xD6 );
	cpu.InjectionMemory( n );
}

// 0b10010110 (0x96)
inline void call_subHL(GameboyCPU & cpu, BYTE a_value, WORD mem_hl_address, BYTE n )
{
	base_op_code_hl(cpu, 0x96, a_value, mem_hl_address, n);
}

inline void call_subNC(GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_n(cpu, 0xDE, a_value, n);
}

inline void call_subRC(GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_reg8(cpu, 0b10011000, a_value, Param8BitIndex::D, n); // SBC A, D
}

inline void call_subHLC(GameboyCPU & cpu, BYTE a_value, WORD mem_hl_address, BYTE n )
{
	base_op_code_hl(cpu, 0x9E, a_value, mem_hl_address, n);
}

inline void call_andN(GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_n(cpu, 0xE6, a_value, n);
}

inline void call_andHL(GameboyCPU & cpu, BYTE a_value, WORD mem_hl_address, BYTE n )
{
	base_op_code_hl(cpu, 0xA6, a_value, mem_hl_address, n);
}

inline void call_andR(GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_reg8(cpu, 0b10100000, a_value, Param8BitIndex::D, n);
}

inline void call_orN(GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_n(cpu, 0xF6, a_value, n);
}

inline void call_orHL(GameboyCPU & cpu, BYTE a_value, WORD mem_hl_address, BYTE n )
{
	base_op_code_hl(cpu, 0xB6, a_value, mem_hl_address, n);
}

inline void call_orR(GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_reg8(cpu, 0b10110000, a_value, Param8BitIndex::D, n);
}

inline void call_xorN(GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_n(cpu, 0xEE, a_value, n);
}

inline void call_xorHL(GameboyCPU & cpu, BYTE a_value, WORD mem_hl_address, BYTE n )
{
	base_op_code_hl(cpu, 0xAE, a_value, mem_hl_address, n);
}

inline void call_xorR(GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_reg8(cpu, 0b10101000, a_value, Param8BitIndex::D, n);
}

inline void call_cpN(GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_n(cpu, 0xFE, a_value, n);
}

inline void call_cpHL(GameboyCPU & cpu, BYTE a_value, WORD mem_hl_address, BYTE n )
{
	base_op_code_hl(cpu, 0xBE, a_value, mem_hl_address, n);
}

inline void call_cpR(GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_reg8(cpu, 0b10111000, a_value, Param8BitIndex::D, n);
}

inline void incR( GameboyCPU & cpu, BYTE reg_index, BYTE reg_value )
{
	BYTE base_op_code = 0b00000100u;
	setRegister8( cpu, reg_index, reg_value );
	cpu.InjectionMemory( base_op_code | static_cast<BYTE>( reg_index << 3u )  );

	for( int i = 0; i < 2; i++ ) { cpu.NextStep(); }
}

inline void incHL( GameboyCPU & cpu, WORD mem_hl_address, BYTE set_value )
{
	setMemory3Step(cpu, Param8BitIndex::D, mem_hl_address, set_value );
	cpu.InjectionMemory( 0x34 );

	for ( int i = 0; i < 4; i++ ) { cpu.NextStep(); }
}

inline void decR( GameboyCPU & cpu, BYTE reg_index, BYTE reg_value )
{
	BYTE base_op_code = 0b00000101u;
	setRegister8( cpu, reg_index, reg_value );
	cpu.InjectionMemory( base_op_code | static_cast<BYTE>( reg_index << 3u ) );

	for( int i = 0; i < 2; i++ ) { cpu.NextStep(); }
}

inline void decHL( GameboyCPU & cpu, WORD mem_hl_address, BYTE set_value )
{
	setMemory3Step(cpu, Param8BitIndex::D, mem_hl_address, set_value );
	cpu.InjectionMemory( 0x35 );

	for ( int i = 0; i < 4; i++ ) { cpu.NextStep(); }
}

inline void addAtoHL( GameboyCPU & cpu, WORD mem_hl_address, BYTE mem_hl_set_value, BYTE a_set_value )
{
	setMemory3Step( cpu, 0, mem_hl_address, mem_hl_set_value );

	setRegister8(cpu, Param8BitIndex::A, a_set_value );

	cpu.InjectionMemory( 0x86 ); // ADD A, (HL)/

	for( int i = 0; i < 5; i++ ) { cpu.NextStep(); }
}


inline void daa( GameboyCPU & cpu )
{
	cpu.InjectionMemory( 0x27 ); // DAA
	cpu.NextStep();

}

inline void cpl( GameboyCPU & cpu )
{
	cpu.InjectionMemory(0x2F );
	cpu.NextStep();
}

inline void ccf( GameboyCPU & cpu )
{
	cpu.InjectionMemory( 0x3f );
	cpu.NextStep();
}

inline void scf( GameboyCPU &  cpu )
{
	cpu.InjectionMemory( 0x37 );
	cpu.NextStep();
}

// reg_index
// 0 => BC, 1 => DE, 2 => HL, 3 => SP
inline void addHLFromReg16( GameboyCPU & cpu, WORD hl_value, WORD reg_value, BYTE reg_index )
{
	// 0b00rr1001
	BYTE base_op_code = 0b00001001u | static_cast<BYTE>( reg_index << 4u );

	setRegister16( cpu, Register16BitIndex::HL, hl_value );  // Set HL
	setRegister16( cpu, reg_index, reg_value );
	cpu.InjectionMemory( base_op_code );

	for ( int i = 0; i < 3; i++ ) { cpu.NextStep(); }
}

// 0b00rr0011 { r = m16BitArugments ]
inline void incReg16( GameboyCPU & cpu, WORD set_reg_value, BYTE reg_index )
{
	BYTE base_op_code = 0b00000011u | static_cast<BYTE>(  reg_index << 4u );

	setRegister16( cpu, reg_index, set_reg_value );
	cpu.InjectionMemory( base_op_code );

	for( int i = 0; i < 2; i++ ) { cpu.NextStep(); }
}

inline void decReg16( GameboyCPU & cpu, WORD set_reg_value, BYTE reg_index )
{
	BYTE base_op_code = 0b00001011u | static_cast<BYTE>( reg_index << 4u );

	setRegister16( cpu, reg_index , set_reg_value );
	cpu.InjectionMemory( base_op_code );

	for ( int i = 0; i < 2; i++ ) { cpu.NextStep(); }
}

inline void rotateAndShiftRegisterExecute(GameboyCPU & cpu, BYTE op_code, BYTE set_reg_value, BYTE reg_index )
{
	setRegister8( cpu, reg_index, set_reg_value );

	cpu.InjectionMemory( 0xCB );
	cpu.InjectionMemory( op_code );

	for( int i = 0; i < 2; i++) { cpu.NextStep(); }
}

inline void rotateAndShiftMemoryExecute(GameboyCPU & cpu, BYTE op_code, BYTE set_value, WORD mem_address )
{
	setMemory3Step(cpu, Param8BitIndex::D, mem_address, set_value );

	cpu.InjectionMemory( 0xCB ); // Prefix
	cpu.InjectionMemory( op_code );

	for( int i = 0; i < 4; i++) { cpu.NextStep(); }
}


inline void rlcRegister( GameboyCPU & cpu, BYTE set_reg_value, BYTE reg_index )
{
	rotateAndShiftRegisterExecute(cpu, reg_index, set_reg_value, reg_index);
}

inline void rlcMemoryHL(  GameboyCPU & cpu, BYTE set_value, WORD mem_address )
{
	rotateAndShiftMemoryExecute(cpu, 0b110, set_value, mem_address);
}

inline void rrcRegister( GameboyCPU & cpu, BYTE set_reg_value, BYTE reg_index )
{
	rotateAndShiftRegisterExecute(cpu, 0b1000u | reg_index, set_reg_value, reg_index);
}

inline void rrcRegisterA( GameboyCPU & cpu, BYTE set_reg_value  )
{
	rotateAndShiftRegisterExecute(cpu, 0x0f, set_reg_value, Param8BitIndex::A);
}

inline void rrcMemoryHL( GameboyCPU & cpu, BYTE set_value, WORD mem_address )
{
	rotateAndShiftMemoryExecute(cpu, 0b1000u | 0b110u, set_value, mem_address);
}

inline void rlRegister( GameboyCPU & cpu, BYTE set_reg_value, BYTE reg_index )
{
	rotateAndShiftRegisterExecute(cpu, 0b10000u | reg_index, set_reg_value, reg_index);
}

inline void rlMemoryHL( GameboyCPU & cpu, BYTE set_value, WORD mem_address)
{
	rotateAndShiftMemoryExecute(cpu, 0b10000u | 0b110u, set_value, mem_address);
}

inline void rrRegister( GameboyCPU & cpu, BYTE set_reg_value, BYTE reg_index )
{
	rotateAndShiftRegisterExecute(cpu, 0b11000u | reg_index, set_reg_value, reg_index);
}

inline void rrRegisterA( GameboyCPU & cpu, BYTE set_reg_value  )
{
	rotateAndShiftRegisterExecute(cpu, 0x1f, set_reg_value, Param8BitIndex::A);
}

inline void rrMemoryHL( GameboyCPU & cpu, BYTE set_value, WORD mem_address)
{
	rotateAndShiftMemoryExecute(cpu, 0b11000u | 0b110u, set_value, mem_address);
}

inline void slaRegister( GameboyCPU & cpu, BYTE set_reg_value, BYTE reg_index )
{
	rotateAndShiftRegisterExecute(cpu, 0b100000u | reg_index, set_reg_value, reg_index);
}

inline void slaMemoryHL( GameboyCPU & cpu, BYTE set_value, WORD mem_address )
{
	rotateAndShiftMemoryExecute(cpu, 0b100000u | 0b110u, set_value, mem_address);
}

inline void sraRegister( GameboyCPU & cpu, BYTE set_reg_value, BYTE reg_index )
{
	rotateAndShiftRegisterExecute(cpu, 0b101000u | reg_index, set_reg_value, reg_index);
}

inline void sraMemoryHL( GameboyCPU & cpu, BYTE set_value, WORD mem_address )
{
	rotateAndShiftMemoryExecute(cpu, 0b101000u | 0b110u, set_value, mem_address);
}

inline void swapRegister( GameboyCPU & cpu, BYTE set_value, Param8BitIndex reg_index )
{
	rotateAndShiftRegisterExecute( cpu, 0b110000u | reg_index, set_value, reg_index );
}

inline void swapMemoryHL( GameboyCPU & cpu, BYTE set_value, WORD mem_address )
{
	rotateAndShiftMemoryExecute( cpu, 0b110000u | 0b110u, set_value, mem_address );
}

inline void srlRegister( GameboyCPU & cpu, BYTE set_reg_value, BYTE reg_index )
{
	rotateAndShiftRegisterExecute(cpu, 0b111000u | reg_index, set_reg_value, reg_index);
}

inline void srlMemoryHL( GameboyCPU & cpu, BYTE set_value, WORD mem_address )
{
	rotateAndShiftMemoryExecute(cpu, 0b111000u | 0b110u, set_value, mem_address);
}

inline void bitInstructionExecute( GameboyCPU & cpu, BYTE pre_op_code, BYTE index, BYTE bit_pos )
{
	BYTE value_index = static_cast<BYTE>( index );
	BYTE op_code = static_cast<BYTE>( pre_op_code << 6u ) |
				   static_cast<BYTE>( bit_pos << 3u );

	op_code |= static_cast<BYTE>( value_index );

	cpu.InjectionMemory( 0xCB );
	cpu.InjectionMemory( op_code );
	cpu.NextStep();
}

inline BYTE bitTest( GameboyCPU & cpu, Param8BitIndex index, BYTE test_index )
{
	bitInstructionExecute( cpu, 0b01u, index, test_index );

	return cpu.GetFlagZ();
}


inline BYTE setBitByRegister( GameboyCPU & cpu, Param8BitIndex index, BYTE set_index_pos )
{
	REQUIRE( index != Param8BitIndex::MEM_HL );

	bitInstructionExecute( cpu, 0b11u, index, set_index_pos );

	return cpu.GetRegisterValueBy8BitIndex( index );
}


// set and return result
inline BYTE setBitByMemory( GameboyCPU & cpu, WORD mem_address , BYTE set_index_pos )
{
	bitInstructionExecute( cpu, 0b11u, 0b110, set_index_pos );

	return cpu.GetMemoryValue( mem_address );
}

inline BYTE resetBitByRegister( GameboyCPU & cpu, Param8BitIndex index, BYTE set_index_pos )
{
	REQUIRE( index != Param8BitIndex::MEM_HL );

	bitInstructionExecute( cpu, 0b10u, index, set_index_pos );

	return cpu.GetRegisterValueBy8BitIndex( index );
}

inline BYTE resetBitByMemory( GameboyCPU & cpu, WORD mem_address , BYTE set_index_pos )
{
	bitInstructionExecute( cpu, 0b10u, 0b110u, set_index_pos );

	return cpu.GetMemoryValue( mem_address );
}

inline void basicJump( GameboyCPU & cpu, BYTE op_code, WORD jp_mem_address )
{
	cpu.InjectionMemory( op_code ); // JP C, WORD
	cpu.InjectionMemory( static_cast<BYTE>( jp_mem_address & 0x00FFu ) );// Lo
	cpu.InjectionMemory( static_cast<BYTE>( static_cast<WORD>( jp_mem_address & 0xFF00u ) >> 8u ) ); //Hi.

	cpu.NextStep();
}

inline WORD jumpToWordIfCarry( GameboyCPU & cpu, WORD jp_mem_address )
{
	basicJump( cpu, 0xDA, jp_mem_address );
	return cpu.GetRegisterPC().reg_16;
}

inline WORD jumpToWordIfNotCarry( GameboyCPU & cpu, WORD jp_mem_address )
{
	basicJump( cpu, 0xD2, jp_mem_address );
	return cpu.GetRegisterPC().reg_16;
}

inline WORD jumpToWordIfZero( GameboyCPU & cpu, WORD jp_mem_address )
{
	basicJump( cpu, 0xCA, jp_mem_address );
	return cpu.GetRegisterPC().reg_16;
}

inline WORD jumpToWordNotIfZero( GameboyCPU & cpu, WORD jp_mem_address )
{
	basicJump( cpu, 0xC2, jp_mem_address );
	return cpu.GetRegisterPC().reg_16;
}

inline WORD jumpToWord( GameboyCPU & cpu, WORD jp_mem_address )
{
	basicJump(  cpu, 0xC3, jp_mem_address );
	return cpu.GetRegisterPC().reg_16;
}

inline WORD jumpToHL( GameboyCPU & cpu, WORD jp_mem_address )
{
	setRegister16( cpu, Param8BitIndex::D, jp_mem_address );  // Call 1.
	cpu.InjectionMemory( 0xE9 );

	for( int i = 0; i < 2; i++ ) { cpu.NextStep(); }

	return cpu.GetRegisterPC().reg_16;
}

struct CallResult
{
	WORD reg_16_pc;
	WORD reg_16_sp;

	CallResult( WORD reg_16_pc, WORD reg_16_sp ) : reg_16_pc( reg_16_pc ), reg_16_sp( reg_16_sp )
	{

	}
};

inline CallResult callWord( GameboyCPU & cpu, WORD jp_mem_address )
{
	cpu.InjectionMemory( 0xCD );
	cpu.InjectionMemory( ( jp_mem_address & 0x00ffu ) ); // LO;
	cpu.InjectionMemory( ( jp_mem_address & 0xff00u ) >> 8u ); // HI

	cpu.NextStep();

	return CallResult( cpu.GetRegisterPC().reg_16, cpu.GetRegisterSP().reg_16 );
}

enum class CheckCondition
{
	Z = 0,
	NZ = 1,
	C = 0b10,
	NC = 0b11,
};

inline CallResult callIfCondition(GameboyCPU & cpu, CheckCondition check_condition, WORD jp_mem_address )
{
	BYTE check_condition_opcode = static_cast<BYTE>(check_condition);

	cpu.InjectionMemory( 0b11000100u | static_cast<BYTE>( check_condition_opcode << 3u ) );
	cpu.InjectionMemory( ( jp_mem_address & 0x00ffu ) ); // LO;
	cpu.InjectionMemory( ( jp_mem_address & 0xff00u ) >> 8u ); // HI

	cpu.NextStep();

	return CallResult( cpu.GetRegisterPC().reg_16, cpu.GetRegisterSP().reg_16 );
}

inline WORD returnInstruction( GameboyCPU & cpu )
{
	cpu.InjectionMemory(0xC9);
	cpu.NextStep();

	return cpu.GetRegisterPC().reg_16;
}

inline WORD returnIfCondition( GameboyCPU & cpu, CheckCondition check_condition )
{
	BYTE check_condition_opcode = static_cast<BYTE>(check_condition);
	cpu.InjectionMemory( 0b11000000u | static_cast<BYTE>( check_condition_opcode << 3u ) );
	cpu.NextStep();

	return cpu.GetRegisterPC().reg_16;
}

#endif //GAMEBOY_UTIL_H
