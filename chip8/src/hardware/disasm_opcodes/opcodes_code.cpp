#ifdef _CHIP8_DISASM_BUILD


//
// Created by ffdd270 on 2020-05-20.
//



#include "../Chip8.h"
#include <cstdlib>
#include "disasm_util.h"

// [code]
// Clear the display.
void Chip8::opCode00E0(WORD opCode)
{
	pushDisASMString(opCodeToDisASMString(opCode, "CLS"));
}

// [code]
// Return from a subroutine.
//The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
void Chip8::opCode00EE(WORD opCode)
{
	pushDisASMString(opCodeToDisASMString(opCode, "RTN"));
}

// [code]
//JP(jump) addr ( 점프 addr. ) addr = NNN.
void Chip8::opCode1NNN(WORD opCode)
{
	std::string comment = "JP " + hex_to_string( opCode & 0xFFF );

	pushDisASMString(opCodeToDisASMString(opCode, comment));
}


// [code]
//call addr. ( 호출 addr ). addr = NNN. stack에 현재 주소 push.
void Chip8::opCode2NNN(WORD opCode)
{
	std::string comment = "CALL " + hex_to_string( opCode & 0xFFF );

	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

// [code]
//LD (LoaD) I, addr. register I(mAddressIndex) = NNN.
//레지스터 I의 값을 NNN으로 설정한다.
void Chip8::opCodeANNN(WORD opCode)
{
	std::string comment = "LD I " + hex_to_string( opCode & 0xFFF );

	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

// [code]
//JP(JumP) V0, addr. PC = NNN + V0.
//Program Count를 NNN + V0로 점프한다.
void Chip8::opCodeBNNN(WORD opCode)
{
	std::string comment = "JP V0 +" + hex_to_string( opCode & 0xFFF );

	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

// [code]
//RND(random) Vx, Byte(kk). Vx = random byte(255) AND kk.
//0에서 255까지의 값을 생성한 후, kk값으로 AND 연산을 한 결과를 Vx에 저장한다. AND에 관한 정보는 8xy2를 참조.
void Chip8::opCodeCXKK(WORD opCode)
{
	pushDisASMString(
			opCodeToDisASMString(opCode, createComment_RegByte("RND", GetOpCodeSecondValue(opCode), opCode & 0x00FF)));
}


// [code]
// DRW Vx, Vy, nibble. 메모리 레지스터 ( mAddressIndex ) I 에서 부터 N 바이트에 읽은 스프라이트를 (x = Vx, y = Vy) 에 표시하고. Vf = Collision 설정.
// 주소 I부터 N바이트에 있는 스프라이트를 부른 다음, ( Vx, Vy ) 좌표에 그린다. 그리는 방식은 기존 화면에 XOR.
// 만약 이로 인해 픽셀이 지워지면 Vf를 1로 설정. 아니면 0으로 설정.
// 스프라이트가 디스플레이의 외부로 위치하도록 좌표를 설정하면, 반대편 화면 ( 오른쪽에 배치하면 왼쪽에서 튀어나옴 ) 에서 나온다.
// XOR 명령은 8xy3 명령을 참조.
void Chip8::opCodeDXYN(WORD opCode)
{
	std::string comment = createComment_Registers( "DRW", GetOpCodeSecondValue( opCode ), GetOpCodeThirdValue( opCode ) );
	comment += " , Load " + hex_to_string( GetOpCodeForthValue( opCode ) ) +  " Byte.";

	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

// [code]
// Set F, V. I = Vx 값에 해당하는 숫자의 스프라이트 주소로.
// 레지스터 I를 Vx값에 해당하는 16진수 스프라이트 위치로 설정.
void Chip8::opCodeFX29(WORD opCode)
{
	std::string comment = "LD ADDRESS_REG SPRITE( V" + hex_to_string( opCode & 0x0F00 ) + " ) ";

	pushDisASMString(opCodeToDisASMString(opCode, comment));
}

#endif