//
// Created by nhy20 on 2020-10-07.
//


#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"

void check_call( GameboyCPU & cpu, CallCheckCondition check_condition, bool jp_ok  )
{
	WORD before_sp = cpu.GetRegisterSP().reg_16;
	WORD before_pc = cpu.GetRegisterPC().reg_16;

	CallResult result = callIfCondition( cpu, check_condition, 0x5260 );

	REQUIRE( ( result.reg_16_sp == ( before_sp - 2 ) ) == jp_ok );
	REQUIRE( ( result.reg_16_pc == 0x5260 ) == jp_ok );

	WORD saved_pc_value = static_cast<WORD>( static_cast<WORD>( cpu.GetMemoryValue(result.reg_16_sp + 1 ) ) << 8u ) | // hi
						  static_cast<WORD>( cpu.GetMemoryValue( result.reg_16_sp ) ); // lo.

	REQUIRE( ( saved_pc_value == ( before_pc + 1 ) ) == jp_ok ); // 한 스텝 진행했으니까..
}

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

	SECTION("CALL cc, WORD")
	{
		SECTION("CALL C, WORD")
		{
			SECTION("OK")
			{
				subN(cpu, 0, 1);
				check_flags(cpu, false, true, true, true);
				check_call(cpu, CallCheckCondition::C, true); // OK.
			}

			SECTION("NOT OK")
			{
				subN(cpu, 2, 1);
				check_flags(cpu, false, false, true, false);
				check_call(cpu, CallCheckCondition::C, false); // NOT OK.
			}
		}

		SECTION("CALL NC, WORD")
		{
			SECTION("OK")
			{
				subN(cpu, 2, 1);
				check_flags(cpu, false, false, true, false);
				check_call(cpu, CallCheckCondition::NC, true); // OK.
			}

			SECTION("NOT OK")
			{
				subN(cpu, 0, 1);
				check_flags(cpu, false, true, true, true);
				check_call(cpu, CallCheckCondition::NC, false); // NOT OK.
			}
		}

		SECTION("CALL Z, WORD")
		{
			SECTION("OK")
			{
				subN(cpu, 1, 1);
				check_flags(cpu, true, false, true, false);
				check_call(cpu, CallCheckCondition::Z, true); // OK.
			}

			SECTION("NOT OK")
			{
				subN(cpu, 2, 1);
				check_flags(cpu, false, false, true, false);
				check_call(cpu, CallCheckCondition::Z, false); // NOT OK.
			}
		}

		SECTION("CALL NZ, WORD")
		{
			SECTION("OK")
			{
				subN(cpu, 2, 1);
				check_flags(cpu, false, false, true, false);
				check_call(cpu, CallCheckCondition::NZ, true); //  OK.
			}

			SECTION("NOT OK")
			{
				subN(cpu, 1, 1);
				check_flags(cpu, true, false, true, false);
				check_call(cpu, CallCheckCondition::NZ, false); // NOT OK.
			}
		}
	}
}