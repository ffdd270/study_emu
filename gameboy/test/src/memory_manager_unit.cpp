#include <catch.hpp>
#include "GameboyCPU.h"
#include "util.h"
#include "memory/MemoryManageUnit.h"

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
		mMock[ mem_addr ] = value;
	}
private:
	std::array<BYTE, 0xffff> mMock;
};

SCENARIO("Memory Manage Unit", "[MMUNIT]")
{
	GIVEN("A Single Memory Manage Unit")
	{
		std::shared_ptr<MockMemory> ptr_mock_memory = std::make_shared<MockMemory>();
		std::shared_ptr<MemoryManageUnit> ptr_memory_manage_unit = std::make_shared<MemoryManageUnit>( std::static_pointer_cast<MemoryInterface>( ptr_mock_memory ) );

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
	}
}