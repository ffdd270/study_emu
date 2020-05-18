#include <catch.hpp>
#include "Chip8.h"

TEST_CASE( "Test if OpCodes", "[if]" )
{
    Chip8 chip8;

    SECTION( "3XKK" )
    {

        chip8.injectionCode( 0x3128); //x=1 kk = 28
        WORD temp = chip8.getProgramCounter();
        chip8.setRegisterValue(1,0x0028);

        chip8.nextStep();

        REQUIRE(chip8.getProgramCounter() == temp+4);

    }

    chip8.reset();


    SECTION( "4XKK" )
    {
        chip8.injectionCode( 0x4128); //x=1 kk = 28
        WORD temp = chip8.getProgramCounter();

        chip8.setRegisterValue(1,0x0000); //다르게 설정
        chip8.nextStep();
        REQUIRE(chip8.getProgramCounter() == temp+4); //다르면 +4가 되어야함

        chip8.setRegisterValue(1,0x0028);
        chip8.nextStep();
        REQUIRE(chip8.getProgramCounter() == temp+6); //같으면 +2가 되어야 함.
    }

    chip8.reset();



    SECTION( "5XY0" )
    {
        chip8.injectionCode( 0x5120 ); //x = 1 Y = 2
        WORD  temp = chip8.getProgramCounter();
        chip8.setRegisterValue(1,0x0020); //같게 실행
        chip8.nextStep();
        REQUIRE( chip8.getProgramCounter() == temp+4 ); //같으면 +4

        chip8.setRegisterValue(1,0x000); //다르게
        chip8.nextStep();
        REQUIRE(chip8.getProgramCounter() == temp + 6); //다르면 +2
    }

    chip8.reset();

}