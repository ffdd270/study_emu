//
// Created by ly073 on 2020-05-22.
//

#include <catch.hpp>
#include "Chip8.h"

TEST_CASE("TEST Code Opcodes", "math")
{
	Chip8 chip8;

	//ADD Vx Byte. ( 더하기 Vx Byte ) Vx = X, Byte = NN
	SECTION("7XKK")
	{
		chip8.injectionCode(0x7011); //VxIndex = 0 NN = 11
		chip8.setRegisterValue(0,0x11);
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(0) == 0x11 + 0x11); //Vx = Vx + NN

	}
	chip8.reset();

	//ADD Vx Vy.   Vx = Vx + Vy. Vf = Carry.
	//Vx와 Vy를 더하고. 결과가 8비트(255)가 넘는다면 Vf를 1로.
	//넘지 않으면 Vf 0으로 정합니다. Vx에는 나머지 8비트만 저장됩니다.
	SECTION("8XY4")
	{
		chip8.injectionCode(0x8014); //VxIndex = 0, VyIndex = 1 Carry
		chip8.injectionCode(0x8234); //VxIndex = 2, VyIndex = 3 No Carry

		chip8.setRegisterValue(0,0x11);
		chip8.setRegisterValue(1,0x11);
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(0xF) == 0); //No Carry
		REQUIRE(chip8.getRegisterValue(0) == chip8.getRegisterValue(1) + 0x11); //No carry

		chip8.setRegisterValue(2,0xFF);
		chip8.setRegisterValue(3,0xFF);
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(0xF) == 1); //Carry
		REQUIRE(chip8.getRegisterValue(2) == 0xFE);

	}
	chip8.reset();

	//SUB (빼기)  Vx Vy. Vx = Vx - Vy . Vf = NOT borrow.
	//Vx에 Vy를 빼고, Vf를 Vx가 Vy보다 크면 1.
	//크지 않으면 0을 저장. Vx엔 결과가 저장된다.
	SECTION("8XY5")
	{
		chip8.injectionCode(0x8015);
		chip8.injectionCode(0x8235);
		chip8.setRegisterValue(0,0xFF);
		chip8.setRegisterValue(1,0x11); //Vx-Vy 가 Vy 보다 큰 경우
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(0xF) == 1);

		chip8.setRegisterValue(2,0x21);
		chip8.setRegisterValue(3,0x11); //Vx-Vy 가 Vy보다 작은 경우
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(0xF) == 0);

	}
	chip8.reset();

	//SUBN (SUB Not ( 반대로 빼기 ) ) Vx, Vy. Vx = Vy - Vx. VF = NOT borrow.
	//Vy에 Vx를 빼고, Vf를 Vy가 Vx보다 크면 1.
	//크지 않으면 0을 저장. Vx엔 결과가 저장된다.
	SECTION("8XY7")
	{
		chip8.injectionCode(0x8017);
		chip8.injectionCode(0x8237);
		chip8.setRegisterValue(0,0x11);
		chip8.setRegisterValue(1,0xFF); //Vy-Vx 가 Vx 보다 큰 경우
		//Vx = 0xEE
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(0xF) == 1);

		chip8.setRegisterValue(2,0x22); //
		chip8.setRegisterValue(3,0x11); //Vy-Vx 가 Vx보다 작은 경우
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(0xF) == 0);

	}
	chip8.reset();

	// ADD I, Vx. I = I + Vx.
	// register I의 값에 Vx를 더한다.
	SECTION("FX1E")
	{
		chip8.injectionCode(0xF01E);
		WORD temp = chip8.getRegisterValue(chip8.getAddressIndex());
		chip8.setRegisterValue(0,0x11);
		chip8.nextStep();

		REQUIRE(temp+chip8.getRegisterValue(0) == chip8.getRegisterValue(chip8.getAddressIndex()));
	}
	chip8.reset();
}