//
// Created by nhy20 on 2020-05-18.
//

/*
 * 코드 점프 관련 테스트들.
 */

#include <catch.hpp>
#include "Chip8.h"

TEST_CASE( "Test Code OpCodes", "[code]" )
{
	Chip8 chip8;

	SECTION( "1NNN" )
	{
		chip8.injectionCode( 0x1300 ); // JUMP 0x300
		chip8.nextStep();

		REQUIRE( chip8.getProgramCounter() == 0x300 );
	}

	chip8.reset();

	SECTION( "2NNN" )
	{
		chip8.injectionCode( 0x0000 ); // 의미 없는 코드. PC -> 0x202.
		chip8.injectionCode( 0x0000 ); // 의미 없는 코드. PC -> 0x204.
		chip8.injectionCode( 0x2404 ); // call 404; PC -> 0x404. Stack -> 0x204.

		chip8.nextStep(); //0x000 0x202
		chip8.nextStep(); //0x000 0x204
		REQUIRE( chip8.getProgramCounter() == 0x204 );
		chip8.nextStep(); //0x206  -> call 0x404

		std::vector<WORD> callStack = chip8.getCallStack();
		REQUIRE( chip8.getProgramCounter() == 0x404 ); // 404로 이동했어야 했고
		REQUIRE( callStack.size() == 1 ); // 콜스택이 쌓여있어야 했으며
		REQUIRE( callStack[0] == 0x206 ); // 콜스택은 0x204.
	}

	chip8.reset();
}