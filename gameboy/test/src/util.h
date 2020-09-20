//
// Created by nhy20 on 2020-07-18.
//

#ifndef GAMEBOY_UTIL_H
#define GAMEBOY_UTIL_H


#include "GameboyCPU.h"


enum Register8BitIndex
{
	B = 0b000,
	C = 0b001,
	D = 0b010,
	E = 0b011,
	H = 0b100,
	L = 0b101,
	//(HL) = 0b110
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



inline void base_op_code_n(GameboyCPU & cpu, BYTE op_code, BYTE a_value, BYTE n )
{
	setRegister8( cpu, Register8BitIndex::A, a_value );
	cpu.InjectionMemory( op_code );
	cpu.InjectionMemory( n );

	for ( int i = 0; i < 2; i++ ) { cpu.NextStep(); }
}

inline void base_op_code_reg8(GameboyCPU & cpu, BYTE base_op_code, BYTE a_value, BYTE register_index, BYTE n )
{
	setRegister8( cpu, Register8BitIndex::A, a_value );
	setRegister8( cpu, register_index, n );
	cpu.InjectionMemory( ( base_op_code | register_index )  );

	for ( int i = 0; i < 3; i++ ) { cpu.NextStep(); }
}

inline void base_op_code_hl(GameboyCPU & cpu, BYTE op_code, BYTE a_value, WORD mem_hl_address, BYTE n )
{
	setMemory3Step( cpu, Register8BitIndex::B, mem_hl_address, n );
	setRegister8( cpu, Register8BitIndex::A, a_value );
	cpu.InjectionMemory( op_code );

	for( int i = 0; i < 5; i++ )
	{
		cpu.NextStep();
	}
}

// 0b11010110 (0xD6)
inline void subN( GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_n(cpu, 0xD6, a_value, n);
}

// 0b10010110 (0x96)
inline void subHL(  GameboyCPU & cpu, BYTE a_value, WORD mem_hl_address, BYTE n )
{
	base_op_code_hl(cpu, 0x96, a_value, mem_hl_address, n);
}

inline void subNC( GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_n(cpu, 0xDE, a_value, n);
}

inline void subRC(  GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_reg8(cpu, 0b10011000, a_value, Register8BitIndex::D, n); // SBC A, D
}

inline void subHLC( GameboyCPU & cpu, BYTE a_value, WORD mem_hl_address, BYTE n )
{
	base_op_code_hl(cpu, 0x9E, a_value, mem_hl_address, n);
}

inline void andN( GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_n(cpu, 0xE6, a_value, n);
}

inline void andHL( GameboyCPU & cpu, BYTE a_value, WORD mem_hl_address, BYTE n )
{
	base_op_code_hl(cpu, 0xA6, a_value, mem_hl_address, n);
}

inline void andR( GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_reg8(cpu, 0b10100000, a_value, Register8BitIndex::D, n);
}

inline void orN( GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_n(cpu, 0xF6, a_value, n);
}

inline void orHL( GameboyCPU & cpu, BYTE a_value, WORD mem_hl_address, BYTE n )
{
	base_op_code_hl(cpu, 0xB6, a_value, mem_hl_address, n);
}

inline void orR( GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_reg8(cpu, 0b10110000, a_value, Register8BitIndex::D, n);
}

inline void xorN( GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_n(cpu, 0xEE, a_value, n);
}

inline void xorHL( GameboyCPU & cpu, BYTE a_value, WORD mem_hl_address, BYTE n )
{
	base_op_code_hl(cpu, 0xAE, a_value, mem_hl_address, n);
}

inline void xorR( GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_reg8(cpu, 0b10101000, a_value, Register8BitIndex::D, n);
}

inline void cpN( GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_n(cpu, 0xFE, a_value, n);
}

inline void cpHL( GameboyCPU & cpu, BYTE a_value, WORD mem_hl_address, BYTE n )
{
	base_op_code_hl(cpu, 0xBE, a_value, mem_hl_address, n);
}

inline void cpR( GameboyCPU & cpu, BYTE a_value, BYTE n )
{
	base_op_code_reg8(cpu, 0b10111000, a_value, Register8BitIndex::D, n);
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
	setMemory3Step( cpu, Register8BitIndex::D, mem_hl_address, set_value );
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
	setMemory3Step( cpu, Register8BitIndex::D, mem_hl_address, set_value );
	cpu.InjectionMemory( 0x35 );

	for ( int i = 0; i < 4; i++ ) { cpu.NextStep(); }
}

inline void addAtoHL( GameboyCPU & cpu, WORD mem_hl_address, BYTE mem_hl_set_value, BYTE a_set_value )
{
	setMemory3Step( cpu, 0, mem_hl_address, mem_hl_set_value );

	setRegister8( cpu, Register8BitIndex::A, a_set_value );

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

inline void rotateRegisterExecute( GameboyCPU & cpu, BYTE op_code, BYTE set_reg_value, BYTE reg_index )
{
	setRegister8( cpu, reg_index, set_reg_value );

	cpu.InjectionMemory( 0xCB );
	cpu.InjectionMemory( op_code );

	for( int i = 0; i < 2; i++) { cpu.NextStep(); }
}

inline void rotateMemoryExecute(  GameboyCPU & cpu, BYTE op_code, BYTE set_value, WORD mem_address )
{
	setMemory3Step( cpu, Register8BitIndex::D, mem_address, set_value );

	cpu.InjectionMemory( 0xCB ); // Prefix
	cpu.InjectionMemory( op_code );

	for( int i = 0; i < 4; i++) { cpu.NextStep(); }
}


inline void rlcRegister( GameboyCPU & cpu, BYTE set_reg_value, BYTE reg_index )
{
	rotateRegisterExecute( cpu, reg_index, set_reg_value, reg_index );
}

inline void rlcMemoryHL(  GameboyCPU & cpu, BYTE set_value, WORD mem_address )
{
	rotateMemoryExecute( cpu, 0b110, set_value, mem_address );
}

inline void rrcRegister( GameboyCPU & cpu, BYTE set_reg_value, BYTE reg_index )
{
	rotateRegisterExecute( cpu, 0b1000u | reg_index,  set_reg_value, reg_index );
}

inline void rrcMemoryHL( GameboyCPU & cpu, BYTE set_value, WORD mem_address )
{
	rotateMemoryExecute( cpu, 0b1000u | 0b110u,  set_value, mem_address );
}

#endif //GAMEBOY_UTIL_H
