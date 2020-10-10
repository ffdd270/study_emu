//
// Created by nhy20 on 2020-10-07.
//


#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"

void check_call(GameboyCPU & cpu, CheckCondition check_condition, bool jp_ok  )
{
	WORD before_sp = cpu.GetRegisterSP().reg_16;
	WORD before_pc = cpu.GetRegisterPC().reg_16;

	CallResult result = callIfCondition( cpu, check_condition, 0x5260 );

	REQUIRE( ( result.reg_16_sp == ( before_sp - 2 ) ) == jp_ok );
	REQUIRE( ( result.reg_16_pc == 0x5260 ) == jp_ok );

	WORD saved_pc_value = static_cast<WORD>( static_cast<WORD>( cpu.GetMemoryValue(result.reg_16_sp + 1 ) ) << 8u ) | // hi
						  static_cast<WORD>( cpu.GetMemoryValue( result.reg_16_sp ) ); // lo.

	REQUIRE( ( saved_pc_value == ( before_pc + 3 ) ) == jp_ok ); // 한 스텝 진행했으니까.. -> 그리고 3개가 뛰어넘어짐.
}

void check_condition_ret(GameboyCPU & cpu, CheckCondition check_condition, bool rtn_ok, BYTE a_value )
{
	WORD before_sp = cpu.GetRegisterSP().reg_16;
	WORD before_pc = cpu.GetRegisterPC().reg_16;

	callWord( cpu, 0x5260 );
	cpu.SetInjectionCount( 0x5260 );
	call_subN(cpu, a_value, 1);

	REQUIRE( ( returnIfCondition( cpu, check_condition ) == before_pc + 3 ) == rtn_ok );
	REQUIRE( ( before_sp == cpu.GetRegisterSP().reg_16 ) == rtn_ok );
}

TEST_CASE( "CALL AND RETURN", "[CALL_AND_RETURN]")
{
	GameboyCPU cpu;

	SECTION("CALL WORD")
	{
		SECTION("BASIC TEST")
		{
			WORD before_sp = cpu.GetRegisterSP().reg_16;
			WORD before_pc = cpu.GetRegisterPC().reg_16;

			CallResult result = callWord( cpu, 0x2460 );

			REQUIRE( result.reg_16_sp == ( before_sp - 2 ) );
			REQUIRE( result.reg_16_pc == 0x2460 );

			WORD saved_pc_value = static_cast<WORD>( static_cast<WORD>( cpu.GetMemoryValue(result.reg_16_sp + 1 ) ) << 8u ) | // hi
								  static_cast<WORD>( cpu.GetMemoryValue( result.reg_16_sp ) ); // lo.

			REQUIRE( saved_pc_value == ( before_pc + 3 ) ); // 한 스텝 진행했으니까.. -> 가 아니라 3개가 날아간거다!
		}
	}



	SECTION("CALL cc, WORD")
	{
		SECTION("CALL C, WORD")
		{
			SECTION("OK")
			{
				call_subN(cpu, 0, 1);
				check_flags(cpu, false, true, true, true);
				check_call(cpu, CheckCondition::C, true); // OK.
			}

			SECTION("NOT OK")
			{
				call_subN(cpu, 2, 1);
				check_flags(cpu, false, false, true, false);
				check_call(cpu, CheckCondition::C, false); // NOT OK.
			}
		}

		SECTION("CALL NC, WORD")
		{
			SECTION("OK")
			{
				call_subN(cpu, 2, 1);
				check_flags(cpu, false, false, true, false);
				check_call(cpu, CheckCondition::NC, true); // OK.
			}

			SECTION("NOT OK")
			{
				call_subN(cpu, 0, 1);
				check_flags(cpu, false, true, true, true);
				check_call(cpu, CheckCondition::NC, false); // NOT OK.
			}
		}

		SECTION("CALL Z, WORD")
		{
			SECTION("OK")
			{
				call_subN(cpu, 1, 1);
				check_flags(cpu, true, false, true, false);
				check_call(cpu, CheckCondition::Z, true); // OK.
			}

			SECTION("NOT OK")
			{
				call_subN(cpu, 2, 1);
				check_flags(cpu, false, false, true, false);
				check_call(cpu, CheckCondition::Z, false); // NOT OK.
			}
		}

		SECTION("CALL NZ, WORD")
		{
			SECTION("OK")
			{
				call_subN(cpu, 2, 1);
				check_flags(cpu, false, false, true, false);
				check_call(cpu, CheckCondition::NZ, true); //  OK.
			}

			SECTION("NOT OK")
			{
				call_subN(cpu, 1, 1);
				check_flags(cpu, true, false, true, false);
				check_call(cpu, CheckCondition::NZ, false); // NOT OK.
			}
		}

		SECTION("RET")
		{
			WORD before_sp = cpu.GetRegisterSP().reg_16;
			WORD before_pc = cpu.GetRegisterPC().reg_16;

			callWord( cpu, 0x3242 );
			cpu.SetInjectionCount( 0x3242 );
			WORD ret_pc = returnInstruction( cpu );

			REQUIRE( ret_pc == before_pc + 3 ); // PC는 1늘었을테니.. 가 아니라 뒤에 WORD 있어서 3.
			REQUIRE( before_sp == cpu.GetRegisterSP().reg_16 );
		}


		SECTION("RET cc")
		{
			SECTION("cc = C")
			{
				SECTION("OK")
				{
					check_condition_ret( cpu, CheckCondition::C, true, 0 );
					check_flags(cpu, false, true, true, true);
				}

				SECTION("NOT OK")
				{
					check_condition_ret( cpu, CheckCondition::C, false, 2 );
					check_flags(cpu, false, false, true, false);
				}
			}

			SECTION("cc = NC")
			{
				SECTION("OK")
				{
					check_condition_ret( cpu, CheckCondition::NC, true, 2 );
					check_flags(cpu, false, false, true, false);
				}

				SECTION("NOT OK")
				{
					check_condition_ret( cpu, CheckCondition::NC, false, 0 );
					check_flags(cpu, false, true, true, true);
				}
			}

			SECTION("cc = Z")
			{
				SECTION("OK")
				{
					check_condition_ret( cpu, CheckCondition::Z, true, 1 );
					check_flags(cpu, true, false, true, false);
				}

				SECTION("NOT OK")
				{
					check_condition_ret( cpu, CheckCondition::Z, false, 0 );
					check_flags(cpu, false, true, true, true);
				}
			}

			SECTION("cc = NZ")
			{
				SECTION("OK")
				{
					check_condition_ret( cpu, CheckCondition::NZ, true, 0 );
					check_flags(cpu, false, true, true, true);
				}

				SECTION("NOT OK")
				{
					check_condition_ret( cpu, CheckCondition::NZ, false, 1 );
					check_flags(cpu, true, false, true, false);
				}
			}
		}
	}

	SECTION("RETI")
	{
		WORD before_pc = cpu.GetRegisterPC().reg_16;

		callWord( cpu, 0x2460 );
		cpu.SetInjectionCount( 0x2460 );
		cpu.InjectionMemory( 0xD9 ); //RETI
		cpu.NextStep();

		REQUIRE(before_pc + 3 == cpu.GetRegisterPC().reg_16); // CALL Instruction은 3 개의 명령어 스텝임.
		REQUIRE(cpu.IsInterruptEnable());
	}

	SECTION("RST param")
	{
		BYTE base_op_code = 0b11000111u;
	 	const BYTE param_to_jp_pos[] = { 0x0, 0x8, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38 };

	 	/* SET
	 	 * 0x1000 => RST 0x0
	 	 * 0x0 => RTN
	 	 * 0x1001 => RST 0x8
	 	 * 0x0 => RTN
	 	 * ...
	 	 */
	 	for( BYTE i = 0; i <= 0b111u; i++)
		{
	 		cpu.SetInjectionCount( 0x1000 + i );
			cpu.InjectionMemory( base_op_code | static_cast<BYTE>( i << 3u ) );
			cpu.SetInjectionCount(  param_to_jp_pos[i] );
			cpu.InjectionMemory(0xC9); // RTN
		}


	 	for( BYTE i = 0; i <= 0b111u; i++ )
		{
			REQUIRE( cpu.GetRegisterPC().reg_16 == 0x1000 + i );
	 		cpu.NextStep(); //RET i
			REQUIRE( cpu.GetRegisterPC().reg_16 == param_to_jp_pos[i] );
			cpu.NextStep(); //RTN
		}
	}

}

TEST_CASE("FIX IN CALL AND RETURN", "[FIX_CALL_AND_RETURN]")
{
	GameboyCPU cpu;

	// CALL과 RET 이후로 명령어가 제대로 진행되지 않아서 생겼던 문제의 테스트 케이스.
	SECTION("CALL and RET, After Instruction OK?")
	{
		callSetRegister8( cpu, Param8BitIndex::A, 0xA ); // 레지스터 .
		callWord( cpu, 0x2460 ); // 함수 호출
		subN( cpu, 1 );

		// 함수. RTN만 있다.
		cpu.SetInjectionCount( 0x2460 );
		returnInstruction( cpu );

		cpu.NextStep(); // subN.

		REQUIRE( cpu.GetRegisterAF().hi == 0x9 );
	}
}
