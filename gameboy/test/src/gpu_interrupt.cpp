#include <catch.hpp>
#include "Motherboard.h"
#include "util.h"
#include "GameboyCPU.h"
#include "memory/MemoryManageUnit.h"
#include "memory/GPU.h"
#include <iostream>

inline void STEP( std::shared_ptr<GPU> & ref_ptr_gpu, Motherboard & ref_motherboard, size_t prv_clock )
{
	ref_ptr_gpu->NextStep( prv_clock );
	ref_motherboard.Step(); // 여기서 한 클럭 더 씀
}


inline void HBLANK( std::shared_ptr<GPU> & ref_ptr_gpu, Motherboard & ref_motherboard )
{
	STEP( ref_ptr_gpu, ref_motherboard, 80 + 172 );
}

inline void VBLANK( std::shared_ptr<GPU> & ref_ptr_gpu, Motherboard & ref_motherboard )
{
	STEP( ref_ptr_gpu, ref_motherboard, GPUHelper::RealScanlineEnd * GPUHelper::LinePerDots - 1  );
}

inline void OAM( std::shared_ptr<GPU> & ref_ptr_gpu, Motherboard & ref_motherboard )
{
	STEP( ref_ptr_gpu, ref_motherboard, 79 );
}

inline void JUMP_TO_100LINE( std::shared_ptr<GPU> & ref_ptr_gpu, Motherboard & ref_motherboard )
{
	STEP( ref_ptr_gpu, ref_motherboard, ( 100 * GPUHelper::LinePerDots ) - 1 );
}

inline void JUMP_TO_NEXT_LINE( std::shared_ptr<GPU> & ref_ptr_gpu, Motherboard & ref_motherboard )
{
	STEP( ref_ptr_gpu, ref_motherboard, (  GPUHelper::LinePerDots ) - 1 );
}


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
	constexpr BYTE LCD_STAT_INTERRUPT_REQ_VALUE = 0b10;
	constexpr BYTE V_BLANK_INTERRUPT_REQ_VALUE = 0b1;


	Motherboard motherboard;
	std::shared_ptr<MockMemory> memory = std::make_shared<MockMemory>();
	motherboard.SetCartridge( std::static_pointer_cast<MockMemory>( memory ) );

	std::shared_ptr<GPU> ptr_gpu = std::static_pointer_cast<GPU>( motherboard.GetInterface( Motherboard::Interface_GPU ) );
	std::shared_ptr<MemoryManageUnit> ptr_mmunit = std::static_pointer_cast<MemoryManageUnit>( motherboard .GetInterface( Motherboard::Interface_MMUNIT ));

	ptr_mmunit->Set( 0xff0f, 0 );

	GIVEN("0xff0f -> 0, Not GPU Interrupt Set")
	{
		ptr_mmunit->Set( 0xff41, 0 ); // 인터럽트 호영  다 끔.

		WHEN("V-BLANK, NOT ENABLE LCD STAT/V-BLANK")
		{
			VBLANK( ptr_gpu, motherboard );

			THEN("0xff0f, BIT 0 is Set.")
			{
				REQUIRE( ptr_mmunit->Get( 0xff0f ) == V_BLANK_INTERRUPT_REQ_VALUE );
				REQUIRE_FALSE( ptr_gpu->IsEnableMode1VBlankInterrupt() );
			}
		}

		WHEN("H-BLANK / LCD STAT")
		{
			HBLANK( ptr_gpu, motherboard );

			THEN("0xff0f, ALL BIT NOT SET, HBLANK INTERRUPT FALSE")
			{
				REQUIRE( ptr_mmunit->Get( 0xff0f ) == 0 );
				REQUIRE_FALSE( ptr_gpu->IsEnableMode0HBlankInterrupt() );
			}
		}

		WHEN("OAM / LCD STAT")
		{
			OAM(ptr_gpu, motherboard);

			THEN("0xff0f, ALL BIT NO SET, OAM INTERRUPT Flag FALSE")
			{
				REQUIRE( ptr_mmunit->Get( 0xff0f ) == 0 );
				REQUIRE_FALSE( ptr_gpu->IsEnableMode2OAMInterrupt() );
			}
		}
	}

	GIVEN("0xff0f->0, GPU Interrupt HBLANK ON.")
	{
		ptr_mmunit->Set( 0xff41, 0b1000 );

		REQUIRE( ptr_gpu->IsEnableMode0HBlankInterrupt() );

		WHEN("H-BLANK")
		{
			HBLANK( ptr_gpu, motherboard );
			THEN( "0xff0f, BIT 1 is Set" )
			{
				REQUIRE( ptr_mmunit->Get( 0xff0f ) == LCD_STAT_INTERRUPT_REQ_VALUE );
				REQUIRE( ptr_gpu->IsEnableMode0HBlankInterrupt() );
			}
		}
	}
	GIVEN("0xff0f->0, GPU Interrupt LCD STAT/VBLANK ON")
	{
		ptr_mmunit->Set( 0xff41, 0b10000 );
		REQUIRE( ptr_gpu->IsEnableMode1VBlankInterrupt() );

		WHEN("V-BLANK")
		{
			VBLANK( ptr_gpu, motherboard );
			THEN("0xff0f, BIT 0, 1 is Set. VBLANK Interrupt Active.")
			{
				REQUIRE( ptr_mmunit->Get( 0xff0f ) == ( LCD_STAT_INTERRUPT_REQ_VALUE | V_BLANK_INTERRUPT_REQ_VALUE ));
				REQUIRE( ptr_gpu->IsEnableMode1VBlankInterrupt() );
			}
		}
	}

	GIVEN("GPU Interrupt LCD STAT/OAM ON")
	{
		ptr_mmunit->Set( 0xff41, 0b1u << 5u );
		REQUIRE( ptr_gpu->IsEnableMode2OAMInterrupt() );

		WHEN("MODE 2")
		{
			// MODE 2 => 80 ㅍ클ㄹ럭
			OAM( ptr_gpu, motherboard );

			THEN("0xff0f, BIT 2 is Set, OAM Interrupt Active")
			{
				REQUIRE( ptr_mmunit->Get( 0xff0f ) == LCD_STAT_INTERRUPT_REQ_VALUE );
				REQUIRE( ptr_gpu->IsEnableMode2OAMInterrupt() );
			}
		}
	}

	GIVEN("GPU Interrupt LCD STAT/LYC ON. LYC = 100")
	{
		ptr_mmunit->Set( 0xff41, 0b1u << 6u );
		REQUIRE( ptr_gpu->IsEnableLYCoincidenceInterrupt() );

		ptr_mmunit->Set( 0xff45, 100 ); // 100라인에서 LYC 작동

		WHEN("Jump to 100 Line")
		{
			JUMP_TO_100LINE( ptr_gpu, motherboard );

			THEN("0xff0f, BIT 2 is Set, OAM Interrupt Active")
	 		{
				REQUIRE( ptr_mmunit->Get( 0xff0f ) == LCD_STAT_INTERRUPT_REQ_VALUE );
				REQUIRE( ptr_gpu->IsCoincidence() );
				REQUIRE( ptr_gpu->IsEnableLYCoincidenceInterrupt() );
			}
		}

		WHEN("Jump to Next Line")
		{
			JUMP_TO_NEXT_LINE( ptr_gpu, motherboard );

			THEN("NO INTERRUPT, LYC Interrupt Active. Coincidence Not Active ")
			{
				REQUIRE( ptr_mmunit->Get( 0xff0f ) == 0);
				REQUIRE_FALSE( ptr_gpu->IsCoincidence() );
				REQUIRE( ptr_gpu->IsEnableLYCoincidenceInterrupt() );
			}
		}
	}

	GIVEN("GPU Interrupt LCD STAT ALL OFF. LYC = 100")
	{
		ptr_mmunit->Set( 0xff41, 0 );
		REQUIRE_FALSE( ptr_gpu->IsEnableLYCoincidenceInterrupt() );

		ptr_mmunit->Set( 0xff45, 100 ); // 100라인에서 LYC 작동

		WHEN("Jump to 100 Line")
		{
			JUMP_TO_100LINE( ptr_gpu, motherboard );

			THEN("NO INTERRUPT LYC Interrupt False. Coincidence Active")
			{
				REQUIRE( ptr_mmunit->Get( 0xff0f ) == 0);
				REQUIRE( ptr_gpu->IsCoincidence() );
				REQUIRE_FALSE( ptr_gpu->IsEnableLYCoincidenceInterrupt() );
			}
		}

		WHEN("Jump to Next Line")
		{
			JUMP_TO_NEXT_LINE( ptr_gpu, motherboard );

			THEN("NO INTERRUPT, LYC Interrupt Active. Coincidence Not Active ")
			{
				REQUIRE( ptr_mmunit->Get( 0xff0f ) == 0);
				REQUIRE_FALSE( ptr_gpu->IsCoincidence() );
				REQUIRE_FALSE( ptr_gpu->IsEnableLYCoincidenceInterrupt() );
			}
		}
	}
}

TEST_CASE("LYC Interrupt Flag Set on every next line", "[GPU]")
{
	Motherboard motherboard;
	std::shared_ptr<MockMemory> memory = std::make_shared<MockMemory>();
	motherboard.SetCartridge( std::static_pointer_cast<MockMemory>( memory ) );

	std::shared_ptr<GPU> ptr_gpu = std::static_pointer_cast<GPU>( motherboard.GetInterface( Motherboard::Interface_GPU ) );
	std::shared_ptr<MemoryManageUnit> ptr_mmunit = std::static_pointer_cast<MemoryManageUnit>( motherboard .GetInterface( Motherboard::Interface_MMUNIT ));

	ptr_mmunit->Set( 0xff41, 0b1u << 6u );
	REQUIRE( ptr_gpu->IsEnableLYCoincidenceInterrupt() );
	ptr_mmunit->Set( 0xff45, 100 ); // 100라인에서 LYC 작동

	JUMP_TO_NEXT_LINE( ptr_gpu, motherboard );

	REQUIRE( ptr_mmunit->Get( 0xff0f ) == 0 );
	REQUIRE_FALSE( ptr_gpu->IsCoincidence() );
	REQUIRE( ptr_gpu->IsEnableLYCoincidenceInterrupt() );
}