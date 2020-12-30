#include <catch.hpp>
#include "Motherboard.h"
#include "GameboyCPU.h"
#include "memory/MemoryManageUnit.h"
#include "memory/GPU.h"
#include <iostream>


constexpr BYTE NOT_SET_VALUE_ON_DMA = 0;

inline void pallet_test( GPU & gpu, WORD pallet_index_address, WORD pallet_address, BYTE red, BYTE green, BYTE blue )
{
	// PalletIndex = 0x2, Auto increment. = 0x80 ( BIT 7 )
	gpu.Set( pallet_index_address, 0x82 );

	// PalletIndex == 2?
	REQUIRE( ( gpu.Get( pallet_index_address ) & 0x1fu ) == 0x2 );

	// Green Lo 3 Bit | red 5 bit.
	gpu.Set( pallet_address, ( green & 0x07u ) << 5u | red );
	// Auto increment.
	REQUIRE( ( gpu.Get( pallet_index_address ) & 0x1fu ) == 0x3 );

	// blue 5bit. | Green hi 2 bit
	gpu.Set( pallet_address, static_cast<BYTE>( blue << 0x2u ) | static_cast<BYTE>( static_cast<BYTE>( green & 0x18u ) >> 3u ) );
	REQUIRE( ( gpu.Get( pallet_index_address ) & 0x1fu ) == 0x4 );

	THEN("Get Pallet, Then R = 0x1f, G = 0x0f, B = 0x10")
	{
		gpu.Set( pallet_index_address, 0x82 ); // Pallet Index = 0x2, Auto Increment
		BYTE lo = gpu.Get( pallet_address );
		REQUIRE(( gpu.Get( pallet_index_address ) & 0x1fu ) == 0x02 ); // Get methods no auto increment.

		gpu.Set( pallet_index_address, 0x03 ); // Pallet Index = 0x3.
		BYTE hi = gpu.Get( pallet_address );
		REQUIRE( gpu.Get( pallet_index_address ) == 0x03 );

		GPUHelper::ColorPallet pallet;
		pallet.SetLo( lo );
		pallet.SetHi( hi );

		REQUIRE( pallet.Red() == red );
		REQUIRE( pallet.Green() == green );
		REQUIRE( pallet.Blue() == blue );
	}
}

void hdma_prepare(Motherboard & motherboard, std::shared_ptr<GPU> & ref_ptr_gpu, std::shared_ptr<MemoryManageUnit> & ref_ptr_mmunit,
				  const WORD source_addr, const WORD dest_addr, const BYTE dma_length, const BYTE dma_mode )
{
	WORD dma_real_length = ( dma_length + 1 ) * 0x10;

	for( size_t i = 0; i < dma_real_length; i++ ) // Source에 데이터 Set.
	{
		ref_ptr_mmunit->Set(source_addr + i, static_cast<BYTE>( ( i % ( 0xfe ) ) + 1 ) ); // 0x00은 체크용.
	}

	// GDMA 1,2. Source 설정.
	ref_ptr_mmunit->Set(0xff51, (source_addr & 0xff00u ) >> 8u ); // Source Hi, 0x30.
	ref_ptr_mmunit->Set(0xff52, (source_addr & 0xffu )); // Source Lo, 0x0f. but low 4bit ignored. 0x3000.
	REQUIRE(ref_ptr_gpu->GetHDMASource() == source_addr );

	// GDMA 3,4. Dest 설정.
	ref_ptr_mmunit->Set(0xff53, (dest_addr & 0xff00u ) >> 8u ); // Source Hi, 0x80.
	ref_ptr_mmunit->Set(0xff54, (dest_addr & 0xffu ) ); // Source Lo. 0x00.
	REQUIRE(ref_ptr_gpu->GetHDMADest() == dest_addr );

	// GDMA 5. DMA Start, Length 0x7f == 0x800.
	BYTE dma_status_value = dma_length | ( ( dma_mode & 0x1u ) << 7u );

	ref_ptr_mmunit->Set(0xff55, dma_status_value ); // GDMA, Len = 0x800.
	REQUIRE(ref_ptr_gpu->GetRemainHDMA() == dma_length );
	REQUIRE(ref_ptr_gpu->GetHDMAMode() == dma_mode );
	REQUIRE(ref_ptr_gpu->IsReportedInterrupt()); // 인터럽트 시작되어야 함.
	REQUIRE(ref_ptr_mmunit->Get(0xff55) == 0x00); // 활성화 상태.
}

void hdma_check(std::shared_ptr<GPU> & ref_ptr_gpu, std::shared_ptr<MemoryManageUnit> & ref_ptr_mmunit,
				const WORD source_addr, const WORD dest_addr, const BYTE dma_length )
{
	WORD dma_real_length = ( dma_length + 1 ) * 0x10;

	for( size_t i = 0; i < dma_real_length; i++ )
	{
		BYTE source_value = 0, dest_value = 1;

		REQUIRE_NOTHROW( source_value = ref_ptr_mmunit->Get(source_addr + i )  );
		REQUIRE_NOTHROW( dest_value = ref_ptr_mmunit->Get(dest_addr + i ) );

		REQUIRE( source_value == dest_value );
	}
}

void all_color_set ( std::array<BYTE, 8> & pallet_line, BYTE set_pallet )
{
	for(BYTE & i : pallet_line)
	{
		i = set_pallet;
	}
}

void half_color_set( std::array<BYTE, 8> & pallet_line, BYTE pallet_1, BYTE pallet_2 )
{
	for( size_t i = 0; i < 4; i++ )
	{
		pallet_line[i] = pallet_1;
	}

	for( size_t i = 4; i < 8; i++ )
	{
		pallet_line[i] = pallet_2;
	}
}



class DummyMemory : public MemoryInterface
{
public:
	DummyMemory() : mDummyMemory()
	{
		mDummyMemory.fill( 0xff ); // VRAM은 0x0으로 초기화하니까, 얘는 완전 다른 값으로..

		// 0x100부터 0x3100까지는 CPU를 위해 텅텅 비워놓는다. NOP만 타게..
		for ( size_t i = 0; i < 0x3000; i++ )
		{
			mDummyMemory[ 0x100 + i ] = 0;
		}
	}

	[[nodiscard]] BYTE Get( size_t mem_addr ) const override
	{
		return mDummyMemory[ mem_addr ];
	}

	void Set( size_t mem_addr, BYTE value ) override
	{
		mDummyMemory[ mem_addr ] = value;
	}
private:
	std::array<BYTE, 0xffff> mDummyMemory;
};


SCENARIO("GPU", "[GPU]")
{
	GIVEN("A Single GPU, Test Memory Access.")
	{
		GPU gpu;

		WHEN("Write 0x8200, 3")
		{
			gpu.Set(0x8200, 3 );

			THEN("Read 0x8200 == 3 ")
			{
				REQUIRE(gpu.Get( 0x8200 ) == 3 );
			}
		}

		WHEN("Write 0xA001, 3")
		{
			THEN("THROW.")
			{
				REQUIRE_THROWS(gpu.Set(0xA001, 3 ) );
			}
		}

		WHEN("Read 0xA001")
		{
			THEN("THROW")
			{
				REQUIRE_THROWS(gpu.Get(0xA001) );
			}
		}

		WHEN("Write 0xff40, 0b11011011")
		{
			gpu.Set( 0xff40, 0b11010011u );

			THEN("7, 6, 4, 1, 0 SET, Else -> NOT SET")
			{
				BYTE LCDC = gpu.Get( 0xff40 );

				REQUIRE(GPUHelper::IsLCDDisplayEnable(LCDC ) );
				REQUIRE(GPUHelper::GetSelectedWindowTileMap(LCDC ) == 0x9C00 );
				REQUIRE_FALSE(GPUHelper::IsWindowDisplayEnable(LCDC ) );
				REQUIRE(GPUHelper::GetSelectBGAndWindowTileData(LCDC ) == 0x8000 );
				REQUIRE(GPUHelper::GetSelectBGTileMapDisplay(LCDC ) == 0x9800 );
				REQUIRE_FALSE(GPUHelper::IsSpriteSize(LCDC ) );
				REQUIRE(GPUHelper::IsSpriteDisplayEnable(LCDC ) );
				REQUIRE(GPUHelper::CheckProperty(LCDC ) );
			}
		}

		WHEN("Write 0xff41, ( LCD Status Register ) , 0b01101111")
		{
			gpu.Set( 0xff41, 0b01101111u );

			THEN("6,5,3 Seteed, 2, 1, 0 -> READ ONLY, not chaged." )
			{
				BYTE LCDStatusRegister = gpu.Get( 0xff41 );

				REQUIRE(GPUHelper::IsEnableLYCoincidenceInterrupt(LCDStatusRegister ) );
				REQUIRE(GPUHelper::IsEnableMode2OAMInterrupt(LCDStatusRegister ) );
				REQUIRE(GPUHelper::IsEnableMode1VBlankInterrupt(LCDStatusRegister ) == false );
				REQUIRE(GPUHelper::IsEnableMode0HBlankInterrupt(LCDStatusRegister ) );
				REQUIRE(GPUHelper::IsCoincidence(LCDStatusRegister ) == false );
				REQUIRE(GPUHelper::GetModeFlag(LCDStatusRegister ) == 0 );
			}
		}

		WHEN("Write 0xff42, 0xff43. ( SCY, SCX ) 20, 30")
		{
			gpu.Set( 0xff42, 20 );
			gpu.Set( 0xff43, 30 );

			THEN("Get 0xff42, 0xff43 , 20, 30.")
			{
				REQUIRE( gpu.Get( 0xff42 ) == 20 );
				REQUIRE( gpu.Get( 0xff43 ) == 30 );
			}
		}

		WHEN("Write 0xff4a, 0xff4b, ( WY, WX ), 45, 51 ")
		{
			gpu.Set( 0xff4a, 45 );
			gpu.Set( 0xff4b, 51 );

			THEN("Get 0xff4a, 0xff4b, 45, 51")
			{
				REQUIRE( gpu.Get( 0xff4a ) == 45 );
				REQUIRE( gpu.Get( 0xff4b ) == 51 );
			}
		}

		WHEN ("Write 0xff47`0xff49 (Mono Pallet)")
		{
			gpu.Set( 0xff47, 0b11100100 ); // BLACK, DARK GRAY, LIGHT GRAY, WHITE.
			gpu.Set( 0xff48, 0xff ); // ALL BLACK.
			gpu.Set( 0xff49, 0x0 ); // ALL WHITE.

			THEN("Check Color Result : Ok")
			{
				BYTE bg = gpu.Get( 0xff47 );

				REQUIRE( GPUHelper::GetPalletData( bg, 0 ) == GPUHelper::MonoPallet::WHITE );
				REQUIRE( GPUHelper::GetPalletData( bg, 1 ) == GPUHelper::MonoPallet::LIGHT_GRAY );
				REQUIRE( GPUHelper::GetPalletData( bg, 2 ) == GPUHelper::MonoPallet::DARK_GRAY );
				REQUIRE( GPUHelper::GetPalletData( bg, 3 ) == GPUHelper::MonoPallet::BLACK );

				BYTE obj0 = gpu.Get( 0xff48 );
				for( int i = 0; i < 4; i++ )
				{
					REQUIRE( GPUHelper::GetPalletData( obj0, i ) == GPUHelper::MonoPallet::BLACK );
				}

				BYTE obj1 = gpu.Get( 0xff49 );
				for( int i = 0; i < 4; i++ )
				{
					REQUIRE( GPUHelper::GetPalletData( obj1, i ) == GPUHelper::MonoPallet::WHITE );
				}
			}
		}

		WHEN("Set BG Pallet R = 0x1f, G = 0x0f, B = 0x10")
		{
			pallet_test( gpu, 0xff68u, 0xff69u, 0x1f, 0x0f, 0x10 );
		}

		WHEN("Set Obj Pallet R = 0x02, G = 0x07, B = 0x04")
		{
			pallet_test( gpu, 0xff6au, 0xff6bu, 0x02, 0x07, 0x04 );
		}

		WHEN("FF 00 7E FF 85 81 89 83 93 85 A5 8B C9 97 7E FF -> Tile Test. 0x8000~0x800f ")
		{
			// https://www.huderlem.com/demos/gameboy2bpp.html  여기 최하단에 있는 걸로 만든
			std::array< BYTE, 16 > bytes = {  0xFF, 0x00,
											  0x00, 0xFF,
											  0x00, 0x00,
											  0xFF, 0xFF,
											  0xF0, 0x0F,
											  0x0F, 0xF0,
											  0xF0, 0x0F,
											  0x0F, 0x0F};
			// 1 => LIGHT_GRAY 8.
			// 2 => DARK_GRAY 8,
			// 3 => WHITE 8
			// 4 => BLACK 8
			// 5 => LIGHT_GRAY 4, BLACK_GRAY 4
			// 6 => BLACK_GRAY 4, LIGHT_GRAY 4
			// 7 => LIGHT_GRAY 4, BLACK GRAY 4
			// 8 => WHITE 4, BLACK 4

			for ( size_t i = 0; i < bytes.size(); i++ )
			{
				gpu.Set( 0x8000 + i, bytes[i] );
			}

			THEN("right colors.")
			{
				std::array< std::array< BYTE, 8 >, 8 > require_colors = {};

				all_color_set( require_colors[0], static_cast<BYTE>(GPUHelper::MonoPallet::LIGHT_GRAY) );
				all_color_set( require_colors[1], static_cast<BYTE>(GPUHelper::MonoPallet::DARK_GRAY) );
				all_color_set(require_colors[2], static_cast<BYTE>(GPUHelper::MonoPallet::WHITE));
				all_color_set( require_colors[3], static_cast<BYTE>(GPUHelper::MonoPallet::BLACK) );
				half_color_set( require_colors[4], static_cast<BYTE>(GPUHelper::MonoPallet::LIGHT_GRAY),  static_cast<BYTE>(GPUHelper::MonoPallet::DARK_GRAY) );
				half_color_set( require_colors[5], static_cast<BYTE>(GPUHelper::MonoPallet::DARK_GRAY),  static_cast<BYTE>(GPUHelper::MonoPallet::LIGHT_GRAY) );
				half_color_set( require_colors[6], static_cast<BYTE>(GPUHelper::MonoPallet::LIGHT_GRAY),  static_cast<BYTE>(GPUHelper::MonoPallet::DARK_GRAY) );
				half_color_set( require_colors[7], static_cast<BYTE>(GPUHelper::MonoPallet::WHITE),  static_cast<BYTE>(GPUHelper::MonoPallet::BLACK) );

				for ( size_t tile_line = 1; tile_line <= 8; tile_line++ )
				{
					size_t mem_add = ( ( tile_line - 1 ) * 2 );

					BYTE lo = gpu.Get( 0x8000 + mem_add  );
					BYTE hi = gpu.Get( 0x8000 + ( mem_add + 1 ) );

					std::array<BYTE, 8> pallets = GPUHelper::ToTileData( lo, hi );
					for( size_t pallet = 0; pallet < 8; pallet++ )
					{
						REQUIRE( pallets[pallet] == require_colors[ tile_line - 1 ][ pallet ] );
					}
				}
			}
		}
	}

	GIVEN("A Single GPU, Test NextStep")
	{
		GPU gpu;

		REQUIRE( gpu.GetModeFlag() == 0 );
		REQUIRE( gpu.IsEnableMode1VBlankInterrupt() == false );

		constexpr size_t ONE_LINE_DRAW_CLOCK = 456;

		WHEN("Clock = 114 * 144 After.")
		{
			constexpr size_t MAX_SCANLINE = 144;

			gpu.NextStep( ONE_LINE_DRAW_CLOCK * MAX_SCANLINE );

			THEN(", VBLANK is Active.")
			{
				REQUIRE( gpu.GetModeFlag() == 1 );
				REQUIRE( gpu.IsEnableMode1VBlankInterrupt() );
			}

			THEN("LY = 144")
			{
				REQUIRE( gpu.Get( 0xff44 ) == 144 );
			}
		}

		WHEN("LYC = 100, Step 114 * 100, -> LYC == LY Interrupt.")
		{
			gpu.Set( 0xff41, 0b1u << 6u ); // Set Enable LY Coincidence Interrupt BIT 6.
			REQUIRE( gpu.IsEnableLYCoincidenceInterrupt() );

			constexpr size_t LYC = 100;
			gpu.Set( 0xff45, LYC );

			REQUIRE( gpu.Get( 0xff45 ) == LYC );
			REQUIRE( gpu.IsCoincidence() == false );

			gpu.NextStep( ONE_LINE_DRAW_CLOCK * LYC  );

			THEN(", LY Interrupt On.")
			{
				REQUIRE( gpu.IsCoincidence() );
			}
		}
	}


	GIVEN("A Single Motherboard")
	{
		Motherboard motherboard;

		std::shared_ptr<MemoryInterface> dummy_memory_ptr = std::make_shared<DummyMemory>();
		motherboard.SetCartridge( dummy_memory_ptr );

		std::shared_ptr<GPU> gpu_ptr = std::static_pointer_cast<GPU>( motherboard.GetInterface( Motherboard::Interface_GPU ) );
		std::shared_ptr<MemoryManageUnit> mmunit_ptr = std::static_pointer_cast<MemoryManageUnit>( motherboard.GetInterface( Motherboard::Interface_MMUNIT ) );

		WHEN("GDMA, Source 0x3000~0x3800, Dest 0x8000~0x8800, Len = 0x800")
		{
			WORD source_addr = 0x3000;
			WORD dest_addr = 0x8000;
			BYTE dma_length = 0x7f;

			hdma_prepare(motherboard, gpu_ptr, mmunit_ptr, source_addr, dest_addr, dma_length, 0x0);
			REQUIRE_NOTHROW( motherboard.Step() ); // 인터럽트 발생, 이제 값이 옮겨짐.

			THEN("0x3000~0x3800 == 0x8000~0x8800")
			{
				hdma_check(gpu_ptr, mmunit_ptr, source_addr, dest_addr, dma_length);
				REQUIRE( gpu_ptr->IsReportedInterrupt() == false ); // 인터럽트 종료되어야 함.
				REQUIRE( mmunit_ptr->Get(0xff55) == 0x80 ); // 비활성화 상태.
				REQUIRE(gpu_ptr->GetRemainHDMA() == 0x7f ); // 끝난 상태.
			}
		}

		WHEN("HDMA, Source 0x4050, 0x4550. Dest 0x9050, 0x9550.")
		{
			WORD source_addr = 0x3400;
			WORD dest_addr = 0x9050;
			BYTE dma_length = 0x4f; // 0x500만큼.

			hdma_prepare(motherboard, gpu_ptr, mmunit_ptr, source_addr, dest_addr, dma_length, 0x1); // HDMA Mode

			THEN("One Step = Copy 0x10.")
			{
				WORD copyed_source_addr = source_addr;
				WORD copyed_dest_addr = dest_addr;

				for(size_t i = 0x0; i < dma_length + 1; i++ )
				{
					REQUIRE_NOTHROW( motherboard.Step() );

					hdma_check(gpu_ptr, mmunit_ptr, copyed_source_addr, copyed_dest_addr, 0x0); // 한번에 0x1만큼만 옮겨진다.
					REQUIRE(gpu_ptr->Get( copyed_dest_addr + 0x11 ) == NOT_SET_VALUE_ON_DMA );

					copyed_dest_addr += 0x10;
					copyed_source_addr += 0x10;
				}

				REQUIRE( gpu_ptr->IsReportedInterrupt() == false );
				REQUIRE( mmunit_ptr->Get(0xff55) == 0x80 );
			}
		}

		WHEN("DMA, 0xff46. Source 0xee00, Dest 0xfe00, length = 0xa0")
		{
			for ( size_t i = 0; i < 0xa0; i++)
			{
				mmunit_ptr->Set( 0xee00 + i, i + 1 );
			}

			REQUIRE_NOTHROW( mmunit_ptr->Set( 0xff46, 0xee ) );
			REQUIRE_NOTHROW( motherboard.Step() );

			THEN("0xee00~0xeea0 == 0xfe00~0xfea0")
			{
				for ( size_t i = 0; i < 0xa0; i++ )
				{
					REQUIRE( mmunit_ptr->Get( 0xee00 + i ) == mmunit_ptr->Get( 0xfe00 + i ) );
				}
			}
		}
	}

}