//
// Created by ly073 on 2020-05-21.
//
#include <catch.hpp>
#include "Chip8.h"

TEST_CASE("Test bit OpCodes", "[bit]")
{
	Chip8 chip8;

	SECTION("8XY1")
	{
		chip8.injectionCode( 0x8121 ); //VxIndex = 1, VyIndex = 2
		chip8.injectionCode( 0x8341 ); //VxIndex = 3, VyIndex = 4

		chip8.setRegisterValue(1,0xF0);
		chip8.setRegisterValue(2,0x0F);
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(1) == 0xFF);

		chip8.setRegisterValue(3,0xA0);
		chip8.setRegisterValue(4,0x0B);
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(3) == 0xAB);

	}
	chip8.reset();

	SECTION("8XY2")
	{
		chip8.injectionCode( 0x8122 ); //VxIndex = 1, VyIndex = 2
		chip8.injectionCode( 0x8342 ); //VxIndex = 3, VyIndex = 4

		chip8.setRegisterValue(1,0xF0);
		chip8.setRegisterValue(2,0x0F);
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(1) == 0x00);

		chip8.setRegisterValue(3,0xAB);
		chip8.setRegisterValue(4,0x9B);
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(3) == 0x8B);
	}
	chip8.reset();

	SECTION("8XY3")
	{
		chip8.injectionCode( 0x8123 ); //VxIndex = 1, VyIndex = 2
		chip8.injectionCode( 0x8343 ); //VxIndex = 3, VyIndex = 4

		chip8.setRegisterValue(1,0xF0);
		chip8.setRegisterValue(2,0x0F);
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(1) == 0xFF);

		chip8.setRegisterValue(3,0xAB);
		chip8.setRegisterValue(4,0x9B);
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(3) == 0x30);
	}
	chip8.reset();

	SECTION("8XY6")
	{
		chip8.injectionCode(0x8016);
		chip8.injectionCode(0x8126);

		chip8.setRegisterValue(0,0x1); //최하위 비트 1
		auto temp = chip8.getRegisterValue(0);
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(0xF) == 1);
		REQUIRE(temp / 2 == chip8.getRegisterValue(0));

		chip8.setRegisterValue(1,0x2); //최하위 비트 0
		temp = chip8.getRegisterValue(1);
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(0xF) == 0);
		REQUIRE(temp / 2 == chip8.getRegisterValue(1));

	}
	chip8.reset();

	SECTION("8XYE")
	{
		chip8.injectionCode(0x801E);
		chip8.injectionCode(0x812E);

		chip8.setRegisterValue(0,0x1); //최하위 비트 1
		auto temp = chip8.getRegisterValue(0);
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(0xF) == 1);
		REQUIRE(temp * 2 == chip8.getRegisterValue(0));

		chip8.setRegisterValue(1,0x2); //최하위 비트 0
		temp = chip8.getRegisterValue(1);
		chip8.nextStep();
		REQUIRE(chip8.getRegisterValue(0xF) == 0);
		REQUIRE(temp * 2 == chip8.getRegisterValue(1));
	}
	chip8.reset();

}
