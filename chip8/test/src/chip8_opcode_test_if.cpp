#include <catch.hpp>
#include "Chip8.h"

TEST_CASE( "Test if OpCodes", "[if]" )
{
    Chip8 chip8;

    SECTION( "3XKK" )
    {

        chip8.injectionCode(0x3128); //x = 0x1 kk = 0x28
        chip8.injectionCode(0x3333); //x = 0x3 kk = 0x33

        WORD temp = chip8.getProgramCounter();
        chip8.setRegisterValue(1,0x0028);

        chip8.nextStep();
        REQUIRE(chip8.getProgramCounter() == temp + 4);

        chip8.nextStep();
		REQUIRE(chip8.getProgramCounter() == temp + 6); //해당하지 않으니 + 2
    }
    chip8.reset();

    SECTION( "4XKK" )
    {
        chip8.injectionCode( 0x4128); //x=0x1 kk = 0x28
		chip8.injectionCode( 0x4028); //x=0x0 kk = 0x28

		WORD temp = chip8.getProgramCounter();

        chip8.setRegisterValue(1,0x0000); //다르게 설정
        chip8.nextStep();
        REQUIRE(chip8.getProgramCounter() == temp+4); //다르면 +4가 되어야함

        chip8.setRegisterValue(0,0x0028);
        chip8.nextStep();
        REQUIRE(chip8.getProgramCounter() == temp+6); //같으면 +2가 되어야 함.
    }
    chip8.reset();

    SECTION( "5XY0" )
    {
		chip8.injectionCode(0x5120); //Vx = 0x1 Vy = 0x2
		chip8.injectionCode(0x5230); //Vx = 0x2 Vy = 0x3

		WORD temp = chip8.getProgramCounter();

		chip8.setRegisterValue(1,0x0000); //다를 때
		chip8.setRegisterValue(2,0x0012);
		chip8.nextStep();
		REQUIRE(temp+2 == chip8.getProgramCounter());

		chip8.setRegisterValue(3,0x0012); //같을 때
		chip8.nextStep();
		REQUIRE(temp+6 == chip8.getProgramCounter());
    }
    chip8.reset();
}