#include <catch.hpp>
#include "ui/ui_event_protocol.h"

void Button_Test1_Mock( UIEventProtocol & protocol )
{
	protocol.AddEvent("DO IT!");
	protocol.AddEvent("TEST");
}


TEST_CASE("UI BUTTON EVENT TEST" , "[UI]")
{
	SECTION("ADD, GET, REMOVE, GET")
	{
		UIEventProtocol protocol;

		// Add
		Button_Test1_Mock( protocol );

		// Get
		REQUIRE( protocol.CheckEvent( "DO IT!") == true );
		REQUIRE( protocol.CheckEvent( "TEST") == true );
		REQUIRE( protocol.CheckEvent( "DO IT") == false );

		//Remove
		protocol.RemoveEvent("DO IT!");
		protocol.RemoveEvent("Not Exist Event."); // but no throw.

		// and remove get.
		REQUIRE( protocol.CheckEvent("DO IT!") == false );
		REQUIRE( protocol.CheckEvent("TEST") == true );
		REQUIRE( protocol.CheckEvent("Not Exist Event.") == false );
	}
}