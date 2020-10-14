#include <catch.hpp>
#include <CPUProvider.h>
#include <array>
#include <functional>

constexpr std::array<const char *, 9> TEST_NAMES = { "A", "B", "C",
										   "D", "E", "F", "G",
										   "H", "L"};

void add_value_and_check_rtn(
		 					 const std::function<size_t(const std::string &, int )> & add_func,
							 const std::function<std::string ( size_t )> & get_func,
							 const std::function<int (const std::string &)> & find_func)
{
	std::array<const char *, 5> select_names = {};
	std::array<size_t, 5> indices = {};

	size_t start_pos = rand() % 4;
	for( int i = 0; i < select_names.size(); i++ )
	{
		select_names[ i ] = TEST_NAMES[ start_pos + i ];
		indices[ i ] = add_func( select_names[ i ], i );
	}

	for ( int i = 0; i < indices.size(); i++ )
	{
		const std::string & require_string = select_names[ i ];
		REQUIRE_NOTHROW(  get_func( i ) );
		const std::string & result_string = get_func( i );

		REQUIRE( require_string == result_string );
		REQUIRE( find_func( require_string ) == i );
	}
}

void add_value_and_wrong_attemp(
		const std::function<size_t(const std::string &, int )> & add_func,
		const std::function<std::string ( size_t )> & get_func,
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
	REQUIRE( find_func( "DAMAE" ) == -1 );
}


TEST_CASE( "CPU PROVIDER", "[PROVIDER]" )
{
	CPUProvider provider;

	SECTION("ADD FLAG OK?")
	{
		add_value_and_check_rtn(
				[&provider](const std::string &string, int value) { return provider.AddFlag(string, value); },
				[&provider](size_t index) { return provider.GetFlagName(index); },
				[&provider](const std::string & ref_string) { return provider.FindFlagIndex( ref_string ); }
		);
	}

	SECTION("ADD FLAG, NO FLAG?")
	{
		add_value_and_wrong_attemp(
				[&provider](const std::string &string, int value) { return provider.AddFlag(string, value); },
				[&provider](size_t index) { return provider.GetFlagName(index); },
				[&provider](const std::string & ref_string) { return provider.FindFlagIndex( ref_string ); }
		);
	}
}