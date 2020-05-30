#ifdef _CHIP8_DISASM_BUILD


//
// Created by ffdd270 on 2020-05-20.
//

#include "../Chip8.h"
#include "disasm_util.h"
// [math]
//ADD Vx Byte. ( 더하기 Vx Byte ) Vx = X, Byte = NN
void Chip8::opCode7XKK(WORD opCode)
{
	std::string comment = createComment_RegByte( "ADD", GetOpCodeSecondValue( opCode ), opCode & 0x00FF );
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

// [math]
//ADD Vx Vy.   Vx = Vx + Vy. Vf = Carry.
//Vx와 Vy를 더하고. 결과가 8비트(255)가 넘는다면 Vf를 1로. 넘지 않으면 Vf 0으로 정합니다. Vx에는 나머지 8비트만 저장됩니다.
void Chip8::opCode8XY4(WORD opCode)
{
	std::string comment = createComment_Registers( "ADD", GetOpCodeSecondValue( opCode ), GetOpCodeThirdValue( opCode ) );
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

// [math]
//SUB (빼기)  Vx Vy. Vx = Vx - Vy . Vf = NOT borrow.
//Vf를 Vx가 Vy보다 크면 1. 크지 않으면 0을 저장. Vx에 Vy를 뺀다.  Vx엔 결과가 저장된다.
//If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
void Chip8::opCode8XY5(WORD opCode)
{
	std::string comment = createComment_Registers( "SUB", GetOpCodeSecondValue( opCode ), GetOpCodeThirdValue( opCode ) );
	pushDisASMString(opCodeToDisASMString(opCode, comment));

}

// [math]
//SUBN (SUB Not ( 반대로 빼기 ) ) Vx, Vy. Vx = Vy - Vx. VF = NOT borrow.
//Vf를 Vy가 Vx보다 크면 1. 크지 않으면 0을 저장. Vy에 Vx를 뺀다. Vy엔 결과가 저장된다.
//If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
void Chip8::opCode8XY7(WORD opCode)
{
	std::string comment = createComment_Registers( "SUBN", GetOpCodeSecondValue( opCode ), GetOpCodeThirdValue( opCode ) );
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

// [math]
// ADD I, Vx. I = I + Vx.
// register I의 값에 Vx를 더한다.
void Chip8::opCodeFX1E(WORD opCode)
{
	std::string comment = "ADD I, V" + hex_to_string( GetOpCodeSecondValue( opCode ) );
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

#endif