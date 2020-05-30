#ifdef _CHIP8_DISASM_BUILD

//
// Created by ffdd270 on 2020-05-20.
//

#include "../Chip8.h"
#include "disasm_util.h"

// [if]
//SE (Skip if Equal) Vx, Byte( 같으면_건너띄기 Vx Byte ).Vx =  X. Byte = KK.
void Chip8::opCode3XKK(WORD opCode)
{
	std::string comment = createComment_RegByte( "SE", GetOpCodeSecondValue( opCode ), opCode & 0x00FF );
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

// [if]
//SNE (Skip if not Equal) Vx, Byte (같지_않으면_건너띄기 Vx Byte )  Vx = X. Byte = KK,
void Chip8::opCode4XKK(WORD opCode)
{
	std::string comment = createComment_RegByte( "SNE", GetOpCodeSecondValue( opCode ), opCode & 0x00FF );
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

// [if]
//SE (Skip if Equal ) Vx, Vy ( 같으면_건너띄기  Vx Vy ). Vx =  X, Vy = Y
void Chip8::opCode5XY0(WORD opCode)
{
	std::string comment = createComment_Registers( "SE", GetOpCodeSecondValue( opCode ), GetOpCodeThirdValue( opCode ) );
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

// [if]
//SNE ( Skip Not Equal ) Vx, Vy.
//Vx와 Vy가 같지 않으면 다음 구문을 생략한다.
void Chip8::opCode9XY0(WORD opCode)
{
	std::string comment = createComment_Registers( "SNE", GetOpCodeSecondValue( opCode ), GetOpCodeThirdValue( opCode ) );
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

#endif
