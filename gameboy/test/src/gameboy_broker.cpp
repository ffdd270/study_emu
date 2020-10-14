//
// Created by HaruGakkaP on 2020-10-14.
//
#include "GameboyCPUBroker.h"
#include "catch.hpp"
#include "util.h"

TEST_CASE( "Gameboy CPU Broker", "[Broker]" )
{
	GameboyCPU cpu;
	GameboyCPUBroker broker;
	std::shared_ptr<CPUProvider> provider_ptr = nullptr;

	SECTION("Set Flag after Update OK?")
	{
		provider_ptr = broker.MakeProvider( cpu );

		REQUIRE( provider_ptr != nullptr );

		call_subN( cpu, 0x0, 1  );
		broker.UpdateProvider( cpu, provider_ptr );
		std::array<int, 4> index_flags = { -1 };
		std::array<std::string, 4> flag_names = { "Z (Zero Flag)", "H (Half Flag)",
											"C (Carry Flag)", "N (Negative Flag)" };

		for( int i = 0; i < flag_names.size(); i++ )
		{
			index_flags[i] = provider_ptr->FindFlagIndex( flag_names[i] );
			REQUIRE( index_flags[i] != -1 );
		}

		REQUIRE( provider_ptr->FindFlagIndex("O") == -1 ); // Negertive Require Test.

		std::array<BYTE, 4> require_values = { cpu.GetFlagZ(), cpu.GetFlagH(), cpu.GetFlagC(), cpu.GetFlagN() };

		for( int i = 0; i < require_values.size(); i++ )
		{
			REQUIRE(require_values[i] == provider_ptr->GetFlag(index_flags[i]));
		}
	}
}