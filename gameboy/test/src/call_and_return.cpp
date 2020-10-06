//
// Created by nhy20 on 2020-10-07.
//


#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"

TEST_CASE( "CALL AND RETURN", "[CALL_AND_RETURN]")
{
	GameboyCPU cpu;

	SECTION("CALL WORD")
	{
		WORD before_sp = cpu.GetRegisterSP().reg_16;
		WORD before_pc = cpu.GetRegisterPC().reg_16;

		CallResult result = callWord( cpu, 0x2460 );

		REQUIRE( result.reg_16_sp == ( before_sp - 2 ) );
		REQUIRE( result.reg_16_pc == 0x2460 );

		WORD saved_pc_value = static_cast<WORD>( static_cast<WORD>( cpu.GetMemoryValue(result.reg_16_sp + 1 ) ) << 8u ) | // hi
				static_cast<WORD>( cpu.GetMemoryValue( result.reg_16_sp ) ); // lo.

		REQUIRE( saved_pc_value == ( before_pc + 1 ) ); // 한 스텝 진행했으니까..
	}
}