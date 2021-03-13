//
// Created by nhy20 on 2020-12-25.
//

#include "Motherboard.h"
#include <memory/MemoryManageUnit.h>
#include <memory/GPU.h>
#include "memory/Timer.h"
#include <string>

Motherboard::Motherboard()
{
	Boot();
}

void Motherboard::Boot()
{
	// 인터페이스 폭파
	mInterfaces = { nullptr };
	// CPU 클리어.
	mCPU = nullptr;


	// 인터페이스 초기화.

	// 카트리지는 나중에 생각하고.

	// GPU.
	mInterfaces[ Interface_GPU ] = std::static_pointer_cast<MemoryInterface>(std::make_shared<GPU>() );

	// Timer
	mInterfaces[ Interface_TIMER ] = std::static_pointer_cast<MemoryInterface>( std::make_shared<Timer>() );

	// Memory Management Unit
	mInterfaces[ Interface_MMUNIT ] = std::static_pointer_cast<MemoryInterface>(
			std::make_shared<MemoryManageUnit>(
					nullptr, mInterfaces[ Interface_GPU ], mInterfaces[Interface_TIMER] ) );

	// CPU
	mCPU = GameboyCPU::CreateWithMemoryInterface( mInterfaces[ Interface_MMUNIT ] );
}

void Motherboard::Step()
{
	size_t clock = mCPU->NextStep();
	std::static_pointer_cast<GPU>(mInterfaces[ Interface_GPU ] )->NextStep( clock );
	std::static_pointer_cast<Timer>(mInterfaces[ Interface_TIMER ] )->NextStep( clock );

	// 인터럽트 처리
	std::vector<InterruptsType> interrupt_array = {};

	for( std::shared_ptr<MemoryInterface> & ref_interface : mInterfaces )
	{
		if (ref_interface == nullptr) { continue; }

		bool reported = ref_interface->IsReportedInterrupt();

		if ( reported )
		{
			std::vector<InterruptsType> interrupt_types = ref_interface->GetReportedInterrupts();

			for ( InterruptsType interrupt_type : interrupt_types )
			{
				interrupt_array.emplace_back( interrupt_type );
			}
		}
	}

	procInterrupts( interrupt_array );
}

void Motherboard::SetCartridge(std::shared_ptr<MemoryInterface> ptr_cartridge)
{
	mInterfaces[ Interface_ROM ] = ptr_cartridge;
	std::static_pointer_cast<MemoryManageUnit>( mInterfaces[ Interface_MMUNIT ] )->SetCartridge( std::move( ptr_cartridge ) );
}

std::shared_ptr<MemoryInterface> Motherboard::GetInterface(Motherboard::Interfaces selected_interface)
{
	return mInterfaces[ selected_interface ];
}

void Motherboard::procInterrupts(const std::vector<InterruptsType> &array_interrupt)
{
	for( InterruptsType type : array_interrupt )
	{
		procInterrupt( type);
	}
}

void proc_dma_step( std::shared_ptr<GPU> & ref_ptr_gpu, std::shared_ptr<MemoryManageUnit> & ref_ptr_mmunit )
{
	WORD source_address = ref_ptr_gpu->GetHDMASource();
	WORD dest_address = ref_ptr_gpu->GetHDMADest();

	for( size_t i = 0; i < 0x10; i++ )
	{
		ref_ptr_mmunit->Set( dest_address + i, ref_ptr_mmunit->Get( source_address + i ) );
	}

	ref_ptr_gpu->SetHDMAAddresses(source_address + 0x10, dest_address + 0x10);
	
	if (ref_ptr_gpu->GetRemainHDMA() == 0 ) // 0 == 0x10이라서, 뺼샘을 뒤로 미뤄야 한다.
	{
		ref_ptr_gpu->SetRemainHDMA(0x7fu);
		return;
	}
	
	BYTE remain = ref_ptr_gpu->GetRemainHDMA();
	ref_ptr_gpu->SetRemainHDMA(remain - 1);
}

bool proc_hdma_interrupt(std::shared_ptr<GPU> & ref_ptr_gpu, std::shared_ptr<MemoryManageUnit> & ref_ptr_mmunit )
{
	// 모드 판단하기.
	BYTE dma_mode = ref_ptr_gpu->GetHDMAMode();

	if( dma_mode == 0x0 ) // GDMA
	{
		// 모든 데이터 전송.
		BYTE len = ref_ptr_gpu->GetRemainHDMA();

		for ( int i = 0; i < len + 1; i++ ) // 0 == 0x10이라서.  1을 더해준다.
		{
			proc_dma_step(ref_ptr_gpu, ref_ptr_mmunit );
		}

		return true;
	}
	else // HDMA
	{
		//TODO : H-Vlank 일때는 돌아가면 안 됨.

		proc_dma_step( ref_ptr_gpu, ref_ptr_mmunit );

		BYTE len = ref_ptr_gpu->GetRemainHDMA();

		if( len == 0x7f ) // 모든 비트가 111111111.
		{
			return true;
		}
	}

	return false;
}

bool proc_dma_interrupt(std::shared_ptr<GPU> & ref_ptr_gpu, std::shared_ptr<MemoryManageUnit> &  ref_ptr_mmunit)
{
	WORD source_address = ref_ptr_gpu->GetDMASource();
	WORD dest_address = ref_ptr_gpu->GetDMADest();

	for( size_t i = 0; i < ref_ptr_gpu->GetDMALength(); i++ )
	{
		ref_ptr_mmunit->Set( dest_address + i, ref_ptr_mmunit->Get( source_address + i ) );
	}

	return true;
}

void Motherboard::procInterrupt(InterruptsType interrupt_address)
{
	Interfaces resolve_interrupt_interface = Interface_GPU;
	bool interrupt_resolved = false;

	std::shared_ptr<GPU> gpu_ptr = std::static_pointer_cast<GPU>( mInterfaces[ Interface_GPU ] );
	std::shared_ptr<MemoryManageUnit> mmunit_ptr = std::static_pointer_cast<MemoryManageUnit>( mInterfaces[ Interface_MMUNIT ] );

	switch ( interrupt_address )
	{
		case InterruptsType::DMA:
		{
			interrupt_resolved = proc_dma_interrupt( gpu_ptr, mmunit_ptr );
			break;
		}
		case InterruptsType::HDMA:
		{
			interrupt_resolved = proc_hdma_interrupt(gpu_ptr, mmunit_ptr);
			break;
		}
		case InterruptsType::V_BLANK:
		{
			interrupt_resolved = true;
			BYTE origin_value =  mmunit_ptr->Get( 0xff0f );
			mmunit_ptr->Set( 0xff0f, origin_value | 1u );
			break;
		}
		case InterruptsType::LCD_STAT:
		{
			interrupt_resolved = true;
			BYTE origin_value =  mmunit_ptr->Get( 0xff0f );
			mmunit_ptr->Set( 0xff0f, origin_value | 0b10u );
			break;
		}
		case InterruptsType::TIMER:
		{
			interrupt_resolved = true;
			resolve_interrupt_interface = Interface_TIMER;

			BYTE origin_value =  mmunit_ptr->Get( 0xff0f );
			mmunit_ptr->Set( 0xff0f, origin_value | 0b100u );
			break;
		}
		default:
			throw std::logic_error("Not Impl Interrupt : " + std::to_string( static_cast<WORD>(interrupt_address) ) );
	}

	if ( interrupt_resolved )
	{
		mInterfaces[ resolve_interrupt_interface ]->ResolveInterrupt( interrupt_address );
	}
}
