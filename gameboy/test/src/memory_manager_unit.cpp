#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"
#include "memory/MemoryManageUnit.h"
#include "memory/GPU.h"

class MockMemory : public MemoryInterface
{
public:
	MockMemory() : mMock( { 0 } ) { }

	[[nodiscard]] BYTE Get(size_t mem_addr) const override
	{
		return mMock[ mem_addr ];
	}

	void Set(size_t mem_addr, BYTE value) override
	{
		// NO SET.
	}
private:
	std::array<BYTE, 0xffff> mMock;
};

SCENARIO("Memory Manage Unit", "[MMUNIT]")
{
	GIVEN("A Single Memory Manage Unit, with single cartridge.")
	{
		std::shared_ptr<MockMemory> ptr_mock_memory = std::make_shared<MockMemory>();
		std::shared_ptr<MemoryManageUnit> ptr_memory_manage_unit = std::make_shared<MemoryManageUnit>( std::static_pointer_cast<MemoryInterface>( ptr_mock_memory ), nullptr );

		WHEN("Write to High - RAM Address.")
		{
			WORD address = 0xff80;
			BYTE value = 0x20;
			REQUIRE_NOTHROW( ptr_memory_manage_unit->Set( address, value ) );

			THEN("and Can Read.")
			{
				REQUIRE( ptr_memory_manage_unit->Get( address ) == value );
			}
		}

		WHEN("Write to ROM BANK Addresses")
		{
			for(int i = 0; i < 0x8000; i++)
			{
				ptr_memory_manage_unit->Set( i, i );
			}

			THEN("Nothing Write.")
			{
				for( int i = 0; i < 0x8000; i++ )
				{
					REQUIRE( ptr_memory_manage_unit->Get( i ) == 0 );
				}
			}
		}

		WHEN("ACCESS GPU?")
		{
			THEN("THROW.")
			{
				REQUIRE_THROWS( ptr_memory_manage_unit->Get( 0x8000 ) );
			}
		}
	}

	GIVEN("Memory Manage Unit With GPU.")
	{
		std::shared_ptr<GPU> ptr_vram = std::make_shared<GPU>();

		std::shared_ptr<MemoryManageUnit> ptr_memory_manage_unit = std::make_shared<MemoryManageUnit>
		    (
				nullptr,
				std::static_pointer_cast<GPU>(ptr_vram)
			);


		WHEN("WRITE 0x8250, 3 ")
		{
			ptr_memory_manage_unit->Set( 0x8250, 3 );
			THEN("READ 0x8250 == 3")
			{
				REQUIRE( ptr_memory_manage_unit->Get( 0x8250 ) == 3 );
			}
		}
	}
}