//
// Created by nhy20 on 2020-12-25.
//

#include "Motherborad.h"
#include <memory/MemoryManageUnit.h>
#include <memory/GPU.h>

Motherborad::Motherborad()
{
	Boot();
}

void Motherborad::Boot()
{
	// 인터페이스 폭파
	mInterfaces = { nullptr };
	// CPU 클리어.
	mCPU = nullptr;


	// 인터페이스 초기화.

	// 카트리지는 나중에 생각하고.

	// GPU.
	mInterfaces[ Interface_GPU ] = std::make_shared<GPU>();

	// Memory Management Unit
	mInterfaces[ Interface_MMUNIT ] = std::make_shared<MemoryManageUnit>( nullptr, mInterfaces[ Interface_GPU ] );

	// CPU
	mCPU = GameboyCPU::CreateWithMemoryInterface( mInterfaces[ Interface_MMUNIT ] );
}

void Motherborad::Step()
{
	mCPU->NextStep();
	std::static_pointer_cast<GPU>(mInterfaces[ Interface_GPU ] )->NextStep( 0 );

	// 인터럽트 처리
	size_t interrupt_len = 0;
	std::array<WORD, 10> interrupt_array = { 0 };

	for( auto & ref_interface : mInterfaces )
	{
		if ( ref_interface->IsReportedInterrupt() )
		{
			interrupt_array[interrupt_len] = ref_interface->GetReportedInterrupt();
		}
	}
}

void Motherborad::SetCartridge(std::shared_ptr<MemoryInterface> ptr_cartridge)
{
	mInterfaces[ Interface_ROM ] = ptr_cartridge;
	std::static_pointer_cast<MemoryManageUnit>( mInterfaces[ Interface_MMUNIT ] )->SetCartridge( std::move( ptr_cartridge ) );
}

void Motherborad::procInterrupts(std::array<WORD, 10> &array_interrupt, size_t interrupt_len)
{
	if ( interrupt_len == 0 ) { return; }

	for ( size_t i = 0; i < interrupt_len; i++ )
	{
		procInterrupt( array_interrupt[i] );
	}
}

void proc_dma_step( std::shared_ptr<GPU> & ref_ptr_gpu, std::shared_ptr<MemoryManageUnit> & ref_ptr_mmunit )
{
	WORD source_address = ref_ptr_gpu->GetDMASource();
	WORD dest_address = ref_ptr_gpu->GetDMADest();

	for( size_t i = 0; i < 0x10; i++ )
	{
		ref_ptr_gpu->Set( dest_address + i, ref_ptr_mmunit->Get( source_address + i ) );
	}

	BYTE remain = ref_ptr_gpu->GetRemainDMA();
	ref_ptr_gpu->SetRemainDMA( remain - 1 );
	if ( ref_ptr_gpu->GetRemainDMA() == 0 )
	{
		ref_ptr_gpu->SetRemainDMA( 0x7fu );
	}

	ref_ptr_gpu->SetDMAAddresses( source_address + 0x10, dest_address + 0x10 );
}

bool proc_dma_interrupt(std::shared_ptr<GPU> & ref_ptr_gpu, std::shared_ptr<MemoryManageUnit> & ref_ptr_mmunit )
{
	// 모드 판단하기.
	BYTE dma_mode = ref_ptr_gpu->GetDMAMode();

	if( dma_mode == 0x0 ) // GDMA
	{
		// 모든 데이터 전송.
		BYTE len = ref_ptr_gpu->GetRemainDMA();

		for ( int i = 0; i < len; i++ )
		{
			proc_dma_step(ref_ptr_gpu, ref_ptr_mmunit );
		}

		return true;
	}
	else // HDMA
	{
		//TODO : H-Vlank 일때는 돌아가면 안 됨.

		proc_dma_step( ref_ptr_gpu, ref_ptr_mmunit );

		BYTE len = ref_ptr_gpu->GetRemainDMA();

		if( len == 0x7f ) // 모든 비트가 111111111.
		{
			return true;
		}
	}

	return false;
}

void Motherborad::procInterrupt(WORD interrupt_address)
{
	if ( interrupt_address == 0xff55u ) // DMA Interrupt.
	{
		std::shared_ptr<GPU> gpu_ptr = std::static_pointer_cast<GPU>( mInterfaces[ Interface_GPU ] );
		std::shared_ptr<MemoryManageUnit> mmunit_ptr = std::static_pointer_cast<MemoryManageUnit>( mInterfaces[ Interface_MMUNIT ] );

		bool interrupt_end = proc_dma_interrupt( gpu_ptr, mmunit_ptr );

		if ( interrupt_end )  { gpu_ptr->ResolveInterrupt( 0xff55u ); }
	}
}
