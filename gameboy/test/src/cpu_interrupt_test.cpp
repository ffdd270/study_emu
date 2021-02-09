//
// Created by nhy20 on 2021-02-09.
//


#include <catch.hpp>
#include "GameboyCPU.h"
#include "memory/MemoryManageUnit.h"
#include "util.h"

SCENARIO( "CPU INTERRUPT TEST", "[INTERRUPT]")
{
	GIVEN("A CPU")
	{
		std::shared_ptr<MockRWMemory> ptr_mock_memory = std::make_shared<MockRWMemory>();
		std::shared_ptr<MemoryManageUnit> ptr_mmunit = std::make_shared<MemoryManageUnit>( std::static_pointer_cast<MemoryInterface>( ptr_mock_memory ),
		        nullptr );
		std::shared_ptr<GameboyCPU> ptr_cpu = GameboyCPU::CreateWithMemoryInterface( ptr_mmunit );

		WHEN( "IE, INT V-BLANK" )
		{
			ptr_cpu->InjectionMemory( 0xF3 ); // 인터럽트 비활성화 ( DI )
			ptr_cpu->InjectionMemory( 0xFB ); // 인터럽트 활성화 ( EI )

			ptr_cpu->NextStep(); // DI
			ptr_mmunit->Set( 0xffff, 0x1f ); // 모든 인터럽트 활성화
			ptr_mmunit->Set( 0xff0f, 0x1  ); // V-BLANK 인터럽트 활성화

			ptr_cpu->NextStep(); // EI.
			ptr_cpu->NextStep(); // 인터럽트 처리.

			THEN("CPU MOVE TO INTERRUPT VECTOR, 0x40")
			{
				REQUIRE( ptr_cpu->GetRegisterPC().reg_16 == 0x40 );
				REQUIRE( ptr_cpu->IsInterruptEnable() == false ); // 실행후 인터럽트는 꺼져 있다.
			}

		}

		WHEN("DI, INT V-BLANK")
		{
			ptr_cpu->InjectionMemory( 0xF3 );
			ptr_cpu->NextStep(); // DI. 이걸 먼저 해야 우선순위가 있는 인터럽트에게 안 뺐김.

			WORD prv_pc = ptr_cpu->GetRegisterPC().reg_16;
			ptr_mmunit->Set( 0xffff, 0x1f ); // 모든 인터럽트 활성화
			ptr_mmunit->Set( 0xff0f, 0x1  ); // V-BLANK 인터럽트 활성화

			ptr_cpu->NextStep();

			THEN("CPU NOT INTERRUPTED")
			{
				REQUIRE( ptr_cpu->GetRegisterPC().reg_16 == prv_pc + 1 );
				REQUIRE( ptr_cpu->IsInterruptEnable() == false );
			}
		}

		WHEN("Interrupt Disable, Flag Set.")
		{
			ptr_cpu->InjectionMemory( 0xFB ); // 인터럽트 활성화 ( EI )
			ptr_cpu->NextStep(); // EI

			ptr_mmunit->Set( 0xffff, 0x1e ); // V-BLANK 빼고 모든 인터럽트 활성화.
			ptr_mmunit->Set( 0xff0f, 0x1  ); // V-BLANK 인터럽트 활성화

			WORD prv_pc = ptr_cpu->GetRegisterPC().reg_16;
			ptr_cpu->NextStep(); // PROC INTERRUPT, But failed.

			THEN("Not Interrupted")
			{
				REQUIRE( ptr_cpu->GetRegisterPC().reg_16 == prv_pc + 1 );
				REQUIRE( ptr_cpu->IsInterruptEnable()  );  // 작동 안 돼었으니 그냥 계속 true.
			}


		}
	}
}