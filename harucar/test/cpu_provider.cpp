#include <catch.hpp>
#include <cpu/cpu_provider.h>
#include <array>
#include <functional>

using namespace HaruCar::CPU;

constexpr std::array<const char *, 9> TEST_NAMES = { "A", "B", "C",
										   "D", "E", "F", "G",
										   "H", "L"};

void add_value_and_check_rtn(
		 					 const std::function<size_t(const std::string &, int)> & add_func,
							 const std::function<int()> & answer_func,
							 const std::function<std::string ( size_t )> & get_func,
							 const std::function<int ( size_t )> & get_value_func,
							 const std::function<int (const std::string &)> & find_func )
{
	std::array<const char *, 5> select_names = {};
	std::array<size_t, 5> indices = {};
	std::array<int, 5> answers = {};

	size_t start_pos = rand() % 4;
	for( int i = 0; i < select_names.size(); i++ )
	{
		int answer = answer_func();
		select_names[ i ] = TEST_NAMES[ start_pos + i ];
		indices[ i ] = add_func( select_names[ i ], answer );
		answers[ i ] = answer;
	}

	for ( int i = 0; i < indices.size(); i++ )
	{
		const std::string & require_string = select_names[ i ];
		REQUIRE_NOTHROW(  get_func( indices[i]) );
		const std::string & result_string = get_func( indices[i] );

		REQUIRE( require_string == result_string );
		REQUIRE( find_func( require_string ) == i );
		REQUIRE( get_value_func( indices[i] ) == answers[ indices[i] ] );
	}
}

void add_value_and_wrong_attemp(
		const std::function<size_t(const std::string &, int )> & add_func,
		const std::function<std::string ( size_t )> & get_func,
		const std::function<int ( size_t )> & get_value_func,
		const std::function<int (const std::string &)> & find_func
)
{
	std::array<const char *, 5> select_names = {};
	std::array<size_t, 5> indices = {};

	for( int i = 0; i < select_names.size(); i++ )
	{
		select_names[ i ] = TEST_NAMES[ rand() % TEST_NAMES.size() ];
		indices[ i ] = add_func( select_names[ i ], i );
	}

	REQUIRE_THROWS( get_func( 6 ) );
	REQUIRE_THROWS( get_value_func( 6 ) );
	REQUIRE( find_func( "Z" ) == -1 );
}

void add_value_and_check_values(
		const std::function<size_t(const std::string &, int )> & add_func,
		const std::function<int ()> & answer_func,
		const std::function<const std::vector<std::string>()> & get_names_func,
		const std::function<const std::vector<int>()> & get_values_func
)
{
	std::array<const char *, 5> select_names = {};
	std::array<int, 5> answers = {};

	size_t start_pos = rand() % 4;

	for( int i = 0; i < select_names.size(); i++ )
	{
		int answer = answer_func();
		select_names[ i ] = TEST_NAMES[ start_pos + i ];
		add_func( select_names[ i ], answer );
		answers[ i ] = answer;
	}

	auto & values = get_values_func();
	auto & names = get_names_func();

	REQUIRE( values.size() == answers.size() );
	REQUIRE( names.size() == select_names.size() );

	for( int i = 0; i < answers.size(); i++ )
	{
		REQUIRE( values[i] == answers[i] );
		REQUIRE( names[i] == select_names[i] );
	}
}


TEST_CASE( "CPU PROVIDER", "[PROVIDER]" )
{
	HaruCar::CPU::CPUProvider provider;

	SECTION("ADD FLAG OK?")
	{
		add_value_and_check_rtn(
				[&provider](const std::string &string, bool value) { return provider.AddFlag(string, value); },
				[]() { return rand() % 2; },
				[&provider](size_t index) { return provider.GetFlagName(index); },
				[&provider](size_t index) { return provider.GetFlag(index); },
				[&provider](const std::string & ref_string) { return provider.FindFlagIndex( ref_string ); }
		);
	}

	SECTION("ADD FLAGS and CHECK FLAGS")
	{
		add_value_and_check_values(
				[&provider](const std::string &string, bool value) { return provider.AddFlag(string, value); },
				[]() { return rand() % 2; },
				[&provider]() { return provider.GetFlagNames(); },
				[&provider]() {
					const std::vector<bool> & flags = provider.GetFlags();
					std::vector<int> return_vector;

					return_vector.reserve(flags.size());

					for( bool flag : flags )
					{
						return_vector.emplace_back( flag );
					}

					return return_vector;
				}
		);
	}

	SECTION("ADD FLAG, NO FLAG?")
	{
		add_value_and_wrong_attemp(
				[&provider](const std::string &string, int value) { return provider.AddFlag(string, value); },
				[&provider](size_t index) { return provider.GetFlagName(index); },
				[&provider](size_t index) { return provider.GetFlag(index); },
				[&provider](const std::string & ref_string) { return provider.FindFlagIndex( ref_string ); }
		);
	}

	SECTION("REGISTER HI, LO OK?")
	{
		HaruCar::CPU::ProviderRegister provider_register;

		REQUIRE_THROWS(provider_register.GetHigh() ); //안하고 GetHigh하면 폭발.
		REQUIRE_THROWS(provider_register.GetLow() ); //안하고 GetLow하면 폭발.

		provider_register.register_value = 0xf024;

		REQUIRE_THROWS(provider_register.UseHiLo("A", "B", 1));
		//16 BIT REGISTER TEST
		REQUIRE_NOTHROW(provider_register.UseHiLo("Hi", "Lo", 2));

		REQUIRE(provider_register.hi_register_name == "Hi");
		REQUIRE(provider_register.lo_register_name == "Lo");

		REQUIRE(provider_register.GetHigh() == 0xf0);
		REQUIRE(provider_register.GetLow() == 0x24);

		//32 BIT REGISTER TEST ( 실험적 )
		provider_register.register_value = 0x03241fff;
		REQUIRE_NOTHROW(provider_register.UseHiLo("16", "32", 4));
		REQUIRE(provider_register.hi_register_name == "16");
		REQUIRE(provider_register.lo_register_name == "32");

		REQUIRE(provider_register.GetHigh() == 0x0324);
		REQUIRE(provider_register.GetLow() == 0x1fff);
	}

	SECTION("ADD REGISTER OK?")
	{
		add_value_and_check_rtn(
				[&provider](const std::string &string, int value) {
					HaruCar::CPU::ProviderRegister provider_register;
					provider_register.register_value = value;
					return provider.AddRegister(string, provider_register);
				},
				[]() { return (rand() % 512) - 256; },
				[&provider](size_t index) { return provider.GetRegisterName(index); },
				[&provider](size_t index) { return provider.GetRegisterValue(index).register_value; },
				[&provider](const std::string & ref_string) { return provider.FindRegisterIndex( ref_string ); }
		);
	}

	SECTION("ADD REGISTER, and CHECK REGISTERS")
	{
		add_value_and_check_values(
				[&provider](const std::string &string, int value) {
					ProviderRegister provider_register;
					provider_register.register_value = value;
					return provider.AddRegister(string, provider_register);
					},
				[]() { return (rand() % 512) - 256; },
				[&provider]() { return provider.GetRegisterNames(); },
				[&provider]() {
					const std::vector<ProviderRegister> & regs = provider.GetRegisterValues();
					std::vector<int> return_vector;

					return_vector.reserve(regs.size());

					for( const ProviderRegister & reg : regs )
					{
						return_vector.emplace_back( reg.register_value );
					}

					return return_vector;
				}
		);
	}


	SECTION("ADD REGISTER, NO REGISTER?")
	{
		add_value_and_wrong_attemp(
				[&provider](const std::string &string, int value) {
					ProviderRegister register_value;
					register_value.register_value = value;
					return provider.AddRegister(string, register_value);
					},
				[&provider](size_t index) { return provider.GetRegisterName(index); },
				[&provider](size_t index) { return provider.GetRegisterValue(index).register_value; },
				[&provider](const std::string & ref_string) { return provider.FindRegisterIndex( ref_string ); }
		);
	}

	SECTION("ADD INSTRUCTION OK?")
	{
		add_value_and_check_rtn(
				[&provider](const std::string &string, int value) {
					return provider.AddInstruction(string, value);
				},
				[]() { return (rand() % 0xff); },
				[&provider](size_t index) { return provider.GetInstruction(index); },
				[&provider](size_t index) { return provider.GetOpCode(index); },
				[&provider](const std::string & ref_string) { return provider.FindInstructionIndex( ref_string ); }
		);
	}


	SECTION("ADD INSTRUCTION and CHECK INSTRUCTIONS")
	{
		add_value_and_check_values(
				[&provider](const std::string &string, int value) { return provider.AddInstruction(string, value); },
				[]() { return rand() % 0xff; },
				[&provider]() -> std::vector<std::string>
				{
					return provider.GetInstructions();
				},
				[&provider]() -> std::vector<int> {
					return provider.GetOpCodes();
				}
		);
	}

	SECTION("ADD INSTRUCTION, NO INSTRUCTION?")
	{
		add_value_and_wrong_attemp(
				[&provider](const std::string &string, int value) { return provider.AddInstruction(string, value); },
				[&provider](size_t index) { return provider.GetInstruction(index); },
				[&provider](size_t index) { return provider.GetOpCode(index); },
				[&provider](const std::string & ref_string) { return provider.FindInstructionIndex( ref_string ); }
		);
	}

	SECTION("FIND INSTRUCTIONS INDICES")
	{
		provider.AddInstruction("MOV", 0x34);
		provider.AddInstruction("MOV", 0x34);
		provider.AddInstruction("ADD", 0x24);
		provider.AddInstruction("MOV", 0x34);
		provider.AddInstruction("ADD", 0x24);
		provider.AddInstruction("MOV", 0x34);
		provider.AddInstruction("MOV", 0x34);
		//MOV 5.

		std::vector<int> indices = provider.FindInstructionIndices( "MOV" );
		REQUIRE( indices.size() == 5 );

		for( const int & index : indices )
		{
			REQUIRE( provider.GetInstruction( index ) == "MOV" );
			REQUIRE( provider.GetOpCode( index ) == 0x34 );
		}
	}

	SECTION("GET LAST INSTRUCTIONS")
	{
		provider.AddInstruction( "MOV", 0x34 );
		provider.AddInstruction( "MOV", 0x34 );
		provider.AddInstruction("ADD", 0x24 );
		provider.AddInstruction("ADD", 0x24 );

		REQUIRE( provider.GetLastInstruction() == "ADD" );
		REQUIRE( provider.GetLastOpCode() == 0x24 );
		REQUIRE( provider.GetInstructionsLength() == 4 );
	}

	SECTION("EMPTY, AND LAST INSTRUCTION. THEN THROW")
	{
		REQUIRE_THROWS( provider.GetLastOpCode() );
		REQUIRE_THROWS( provider.GetLastInstruction() );
	}

}