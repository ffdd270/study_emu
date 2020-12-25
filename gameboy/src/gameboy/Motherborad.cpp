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

void Motherborad::procInterrupt(WORD interrupt_address)
{

}
