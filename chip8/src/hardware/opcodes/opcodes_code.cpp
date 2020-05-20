//
// Created by ffdd270 on 2020-05-20.
//


#include "../Chip8.h"


// [code]
//JP(jump) addr ( 점프 addr. ) addr = NNN.
void Chip8::opCode1NNN(WORD opCode)
{
	mProgramCounter = opCode & 0x0FFF; // 앞 자리는 해독이 끝났으니. 실제 명령어가 점프할 데이터인 뒷 바이트를 가져온다.
}


// [code]
//call addr. ( 호출 addr ). addr = NNN. stack에 현재 주소 push.
void Chip8::opCode2NNN(WORD opCode)
{
	mStack.push_back( mProgramCounter ); //stack에 현재 주소 push.
	mProgramCounter = opCode & 0x0FFF;
}
