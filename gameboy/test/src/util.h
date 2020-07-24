//
// Created by nhy20 on 2020-07-18.
//

#ifndef GAMEBOY_UTIL_H
#define GAMEBOY_UTIL_H


#include "GameboyCPU.h"


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

	setRegister16(  cpu_ref, 0b10, mem_addr ); // LD HL(10), imm16( mem_addr )
	//HL = mem_addr. 1 Step.

	setRegister8( cpu_ref, reg8_index, value ); // LD reg8_index, 0xBA
	//reg8_index = value. 2 Step.

	cpu_ref.InjectionMemory( 0b01110000 | reg8_index ); // LD (HL), reg8_index
	//(HL) = value  3 Step.
}

#endif //GAMEBOY_UTIL_H
