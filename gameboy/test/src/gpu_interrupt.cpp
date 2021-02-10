#include <catch.hpp>
#include "Motherboard.h"
#include "util.h"
#include "GameboyCPU.h"
#include "memory/MemoryManageUnit.h"
#include "memory/GPU.h"
#include <iostream>

/*
 * TODO : 남은 테스트 리스트
LCD STAT / HBLANK INTERRUPT ON, REQ INTERRUPT
LCD STAT / HBLANK INTERRUPT OFF, NOT_REQ INTERRUPT
LCD STAT / VBLANK INTERRUPT ON, REQ INTERRUPT
LCD STAT / VBLANK INTERRUPT OFF, NOT_REQ INTERRUPT
LCD STAT / OAM(Mode 1 or 2 ) INTERRUPT ON, REQ INTERRUPT
LCD STAT / OAM(Mode 1 or 2 ) INTERRUPT OFF, NOT_REQ INTERRUPT
LCD STAT / LYC Interrupt ON, REQ Interrupt.
LCD STAT / LYC Interrupt OFF, NOT_REQ Interrupt
 */

SCENARIO("GPU INTERRUPT", "[GPU]")
{
	Motherboard motherboard;
	std::shared_ptr<MockMemory> memory = std::make_shared<MockMemory>();
	motherboard.SetCartridge( std::static_pointer_cast<MockMemory>( memory ) );

	std::shared_ptr<GPU> ptr_gpu = std::static_pointer_cast<GPU>( motherboard.GetInterface( Motherboard::Interface_GPU ) );
	std::shared_ptr<MemoryManageUnit> ptr_mmunit = std::static_pointer_cast<MemoryManageUnit>( motherboard .GetInterface( Motherboard::Interface_MMUNIT ));


	GIVEN("0xff0f -> 0, Not GPU Interrupt Set")
	{
		ptr_mmunit->Set( 0xff0f, 0 );
		ptr_gpu->Set( 0xff41, 0 ); // 인터럽트 호영  다 끔.

		WHEN("V-BLANK")
		{
			ptr_gpu->NextStep( GPUHelper::RealScanlineEnd * GPUHelper::LinePerDots - 1 );
			motherboard.Step(); // 여기서 GPU +1.

			THEN("0xff0f, BIT 0 is Set.")
			{
				REQUIRE( ptr_mmunit->Get( 0xff0f ) == 1 );
			}
		}
	}

	GIVEN("0xff0f->0, GPU Interrupt HBLANK ON.")
	{
		ptr_mmunit->Set( 0xff0f, 0 );
		ptr_mmunit->Set( 0xff41, 0b1000 );

		REQUIRE( ptr_gpu->IsEnableMode0HBlankInterrupt() );

		WHEN("H-BLANK")
		{
			ptr_gpu->NextStep( 80 + 172  ); // H-BLANK 직전
			motherboard.Step(); // H-BLANK

			THEN( "0xff0f, BIT 1 is Set" )
			{
				REQUIRE( ptr_mmunit->Get( 0xff0f ) == 0b10 );
				REQUIRE( ptr_gpu->IsEnableMode0HBlankInterrupt() );
			}
		}
	}
}