#ifdef _CHIP8_DISASM_BUILD

//
// Created by ffdd270 on 2020-05-20.
//

#include "../Chip8.h"
#include <array>
#include "disasm_util.h"

// [load]
//LD (LoaD) Vx, Byte . ( 로드 Vx Byte ) Vx = X, Byte = KK
//LD는 Load의 약자다 =ㅁ=.
void Chip8::opCode6XKK(WORD opCode)
{
	std::string comment = createComment_RegByte( "LD", GetOpCodeSecondValue( opCode ), opCode & 0x00FF );
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}


// [load]
//LD(LoaD) Vx, Vy. ( 로드 Vx Vy )
void Chip8::opCode8XY0(WORD opCode)
{
	std::string comment = createComment_Registers( "LD", GetOpCodeSecondValue( opCode ), GetOpCodeThirdValue( opCode ) );
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

// [load]
// LD Vx, DT. Delay Vx = DT.
// Vx를 DT로 Set.
void Chip8::opCodeFX07(WORD opCode)
{
	std::string comment = "LD V" + hex_to_string( GetOpCodeSecondValue( opCode ) ) + " DT";
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

// [load]
// LD Vx, Key. Set Vx = Key.
// 키 입력까지 대기 후, Vx에 키 값을 저장. 키를 누를 떄 까지 모든 행동을 중지.
void Chip8::opCodeFX0A(WORD opCode)
{
	std::string comment = "LD V" + hex_to_string( GetOpCodeSecondValue( opCode ) ) + " KEY_INPUT (WAIT FOR KEY INPUT)";
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}


// [load]
// LD DT, Vx. Delay Timer = Vx.
// DT(Delay Timer)를 Vx로.
void Chip8::opCodeFX15(WORD opCode)
{
	std::string comment = "LD DT V" + hex_to_string( GetOpCodeSecondValue( opCode ) );
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

// [load]
// LD ST, Vx. Sound Timer = Vx.
// ST(Sound Timer)를 Vx로.
void Chip8::opCodeFX18(WORD opCode)
{
	std::string comment = "LD ST V" + hex_to_string( GetOpCodeSecondValue( opCode ) );
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}


// [load]
// LD B, Vx. I = BCD( Vx의 100자리 수. ) I + 1 = BCD( Vx의 10자리 수.) I + 2 = BCD( Vx의 1자리 수 ),
// 메모리 위치 I, I + 1 및 I + 2에 Vx의 백~일의 자리수의 BCD 표현을 저장. ( BCD : 4비트로 10자리 수를 표현하는 기법. )


void Chip8::opCodeFX33(WORD opCode)
{
	std::string hex_to_string_value = hex_to_string( GetOpCodeSecondValue( opCode ) );
	std::string comment = "LD BCD( I~I+2 )  V" +hex_to_string_value + " , V"  + hex_to_string_value + "to BCD.";
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

// [load]
// LD [I], Vx. Memory Address [I] to length of ( V0 to Vx ) = V0 to Vx.
// V0부터 Vx에 해당하는 값들을 메모리 주소 I에 저장 ( 그럼 메모리 끝점은 Vx 만큼 이동하겠죠? )
void Chip8::opCodeFX55(WORD opCode)
{
	std::string hex_to_string_value = hex_to_string( GetOpCodeSecondValue( opCode ) );
	std::string comment = "LD I~I+"  + hex_to_string_value + " V0~V" + hex_to_string_value+ " , I~I+" + hex_to_string_value + " to V0~V" + hex_to_string_value  ;
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}


// [load]
// LD Vx, [I]. V0 to Vx = Memory Address [I] to length of ( V0 to Vx ).
// 메모리 주소 I부터 I + x에 해당하는 메모리 주소까지 불러온 다음, V0부터 Vx에 저장합니다.
void Chip8::opCodeFX65(WORD opCode)
{
	std::string hex_to_string_value = hex_to_string( GetOpCodeSecondValue( opCode ) );
	std::string comment = "LD V0~V+"  + hex_to_string_value + " I~I" + hex_to_string_value+ " , V0~V" + hex_to_string_value + " to I~I+" + hex_to_string_value;
	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

#endif