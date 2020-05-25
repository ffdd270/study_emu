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

	SECTION( "ANNN" )
	{
		chip8.injectionCode( 0xAABC ); // AddressIndex = ABC;
		chip8.nextStep();

		REQUIRE( chip8.getAddressIndex() == 0xABC );
	}

	chip8.reset();

	SECTION( "BNNN" )
	{
		chip8.injectionCode( 0xB700 ); // 0x700 +  V0로 점프.
		chip8.setRegisterValue( 0, 0x65 ); // V0 = 65

		chip8.nextStep();

		REQUIRE( chip8.getProgramCounter() == 0x765 );
	}

	chip8.reset();

	SECTION( "CXKK" )
	{
		chip8.injectionCode( 0xC10F ); // V1 = ( rand % 255 )  &  0x0F

		chip8.nextStep();

		int value = chip8.getRegisterValue( 1 );
		REQUIRE( ( chip8.getRandomValue() & 0x0F )  == value );
	}

	chip8.reset();

	SECTION( "DXYN" )
	{
		chip8.injectionCode( 0xA000 ); // Register Index를 0으로 Set.
		chip8.injectionCode( 0xD005 ); // V0, V0 좌표에서 Register Index로 부터 5바이트 READ후 Screen에 DRW.

		BYTE sprite_data[] = { // 0.
				0b11111111,
				0b10000001,
				0b10000001,
				0b10000001,
				0b11111111,
		};

		// 메모리 0번지부터 5번지까지.
		for( int i = 0; i < 5; i++ )
		{
			chip8.setMemoryValue( i, sprite_data[i] );
		}

		// 여기까지 O. 메모리 0x0-0x5 스프라이트 데이터.
		chip8.setRegisterValue( 0, 0 );

		chip8.nextStep();
		chip8.nextStep();

		for( int y = 0; y < 5; y++ )
		{
			BYTE result = 0;

			for (int x = 0; x < 8; x++)
			{
				BYTE screen_data = chip8.getScreenData(x, y);
				result |= screen_data << x;
			}

			REQUIRE(sprite_data[y] == result);
		}
	}

	chip8.reset();

	SECTION( "FX29" )
	{
		chip8.injectionCode(0xF129);
		chip8.injectionCode(0xFF29);

		BYTE * font_set = Chip8::getFontSet();

		chip8.nextStep();


		chip8.nextStep();


	}
}