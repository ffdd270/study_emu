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
		chip8.injectionCode( 0x61FF ); // V1 = FF

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
	// LD Vx, K. Delay Timer = Vx.
	// DT(Delay Timer)를 Vx로.
	SECTION( "FX15" )
	{
		chip8.injectionCode( 0x61FE ); // V1 = FE
		chip8.injectionCode( 0xF115 ); // DT = V1

		chip8.nextStep();
		chip8.nextStep();

		REQUIRE( chip8.getDelayTimer() == 0xFE );
	}

	chip8.reset();

	// [load]
	// LD Vx, DT. Delay Vx = DT.
	// Vx를 DT로 Set.
	SECTION( "FX07" )
	{
		chip8.injectionCode(0x60FE); //V0 = FE
		chip8.injectionCode(0xF015); //DT = V0
		chip8.injectionCode(0xF107); //V0 = DT

		chip8.nextStep();
		chip8.nextStep();
		chip8.nextStep();

		REQUIRE( chip8.getRegisterValue(1) == 0xFE );
	}


	chip8.reset();

	// [load]
	// LD Vx, Key. Set Vx = Key.
	// 키 입력까지 대기 후, Vx에 키 값을 저장. 키를 누를 떄 까지 모든 행동을 중지.
	SECTION( "FX0A" )
	{
		chip8.injectionCode(0xF20A );

		chip8.addInput( 0x1 );
		chip8.nextStep();

		REQUIRE( chip8.getRegisterValue( 2 ) == 0x1 );
	}



	chip8.reset();

	// [load]
	// LD ST, Vx. Sound Timer = Vx.
	// ST(Sound Timer)를 Vx로.
	SECTION( "FX18" )
	{
		chip8.injectionCode( 0x60EF ); // V0 = EF
		chip8.injectionCode( 0xF018 ); // ST = V0

		chip8.nextStep();
		chip8.nextStep();

		REQUIRE( chip8.getSoundTimer() == 0xEF );
	}


	chip8.reset();

	// [load]
	// LD B, Vx. I = BCD( Vx의 100자리 수. ) I + 1 = BCD( Vx의 10자리 수.) I + 2 = BCD( Vx의 1자리 수 ),
	// 메모리 위치 I, I + 1 및 I + 2에 Vx의 백~일의 자리수의 BCD 표현을 저장. ( BCD : 4비트로 10자리 수를 표현하는 기법. )
	SECTION( "FX33" )
	{
		chip8.injectionCode( 0x60FF ); // V0 = FF
		chip8.injectionCode( 0xF033 ); // mI = 2, mI + 1 = 5, mI + 2 = 5.

		chip8.nextStep();
		chip8.nextStep();

		BYTE address_index = chip8.getAddressIndex();

		REQUIRE( chip8.getMemoryValue( address_index ) == 0x2 );
		REQUIRE( chip8.getMemoryValue( address_index + 1 ) == 0x5 );
		REQUIRE( chip8.getMemoryValue( address_index + 2 ) == 0x5 );
	}

	chip8.reset();

	// [load]
	// LD [I], Vx. Memory Address [I] to length of ( V0 to Vx ) = V0 to Vx.
	// V0부터 Vx에 해당하는 값들을 메모리 주소 I에 저장 ( 그럼 메모리 끝점은 Vx 만큼 이동하겠죠? )
	SECTION( "FX55" )
	{
		BYTE address_index = chip8.getAddressIndex();

		chip8.setRegisterValue( 0, 0x31 );
		chip8.setRegisterValue( 1, 0x41 );
		chip8.setRegisterValue( 2, 0x59 );
		chip8.setRegisterValue( 3, 0x2 );


		chip8.injectionCode( 0xF355 ); // mem[MA] ~ mem[MA + 3] = V0 ~ V3;

		chip8.nextStep();

		REQUIRE( chip8.getMemoryValue( address_index ) == 0x31 );
		REQUIRE( chip8.getMemoryValue( address_index + 1 ) == 0x41 );
		REQUIRE( chip8.getMemoryValue( address_index + 2 ) == 0x59 );
		REQUIRE( chip8.getMemoryValue( address_index + 3 ) == 0x2 );
	}

	chip8.reset();

	// [load]
	// LD Vx, DT. Delay Vx = DT.
	// Vx를 DT로 Set.
	SECTION( "FX65" )
	{
		BYTE address_index = chip8.getAddressIndex();

		chip8.setMemoryValue( address_index, 0xEF );
		chip8.setMemoryValue( address_index + 1, 0xED );
		chip8.setMemoryValue( address_index + 2, 0xBB );
		chip8.setMemoryValue( address_index + 3, 0xAA );

		chip8.injectionCode( 0xF365 ); // V0~V3 = mem [MA]~ mem [MA+3]
		chip8.nextStep();

		REQUIRE( chip8.getRegisterValue( 0 ) == 0xEF );
		REQUIRE( chip8.getRegisterValue( 1 ) == 0xED );
		REQUIRE( chip8.getRegisterValue( 2 ) == 0xBB );
		REQUIRE( chip8.getRegisterValue( 3 ) == 0xAA );
	}
}