//
// Created by HaruGakkaP on 2020-05-26.
//


#include <catch.hpp>
#include "Chip8.h"

TEST_CASE("TEST Load Opcodes", "load")
{
	Chip8 chip8;

	// [load]   
	//LD (LoaD) Vx, Byte . ( 로드 Vx Byte ) Vx = X, Byte = KK
	//LD는 Load의 약자다 =ㅁ=.
	SECTION( "6XKK" )
	{
		chip8.injectionCode(0x61FF ); // V1 = FF

		chip8.nextStep();

		REQUIRE( chip8.getRegisterValue( 1 ) == 0xFF );
	}

	chip8.reset();


	// [load]
	//LD(LoaD) Vx, Vy. ( 로드 Vx Vy )
	SECTION( "8XY0" )
	{
		chip8.injectionCode( 0x60EE );  // V0 = EE
		chip8.injectionCode( 0x8100 ); // V1 = V0

		chip8.nextStep();
		chip8.nextStep();

		REQUIRE( chip8.getRegisterValue( 0 ) == chip8.getRegisterValue( 1 ) );
	}

	chip8.reset();

	// [load]
	// LD B, Vx. I = BCD( Vx의 100자리 수. ) I + 1 = BCD( Vx의 10자리 수.) I + 2 = BCD( Vx의 1자리 수 ),
	// 메모리 위치 I, I + 1 및 I + 2에 Vx의 백~일의 자리수의 BCD 표현을 저장. ( BCD : 4비트로 10자리 수를 표현하는 기법. )
	SECTION( "FX33" )
	{
		chip8.injectionCode(0xF133 );
		chip8.setRegisterValue( 1, 0xFF );

		chip8.nextStep();

		REQUIRE( chip8.getMemoryValue( chip8.getAddressIndex() ) == 2 );
		REQUIRE( chip8.getMemoryValue( chip8.getAddressIndex() + 1 ) == 5 );
		REQUIRE( chip8.getMemoryValue( chip8.getAddressIndex() + 2 ) == 5 );
	}

}