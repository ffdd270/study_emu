//
// Created by HaruGakkaP on 2020-10-13.
//

#include "GameboyCPUBroker.h"


constexpr size_t NO_INITED = 0xDEADBEEF;

static const std::array<const char *, 8> REGISTER_NAMES =
		{ "B", "C", "D", "E",
		  "H", "L", "NIL", "A" };

GameboyCPUBroker::GameboyCPUBroker() : mIndexC( NO_INITED ), mIndexH( NO_INITED ), mIndexN( NO_INITED ), mIndexZ( NO_INITED ),
	mRegisterIndices( { NO_INITED })
{

}

std::shared_ptr<CPUProvider> GameboyCPUBroker::MakeProvider(GameboyCPU &cpu)
{
	std::shared_ptr<CPUProvider> provider_ptr = std::make_shared<CPUProvider>();

	mIndexC = provider_ptr->AddFlag( "C (Carry Flag)", cpu.GetFlagC() );
	mIndexH = provider_ptr->AddFlag( "H (Half Flag)", cpu.GetFlagH() );
	mIndexZ = provider_ptr->AddFlag( "Z (Zero Flag)", cpu.GetFlagZ() );
	mIndexN = provider_ptr->AddFlag( "N (Negative Flag)", cpu.GetFlagN() );

	mIndexAF = provider_ptr->AddRegister( "AF", cpu.GetRegisterAF().reg_16 );
	mIndexBC = provider_ptr->AddRegister( "BC", cpu.GetRegisterBC().reg_16 );
	mIndexDE = provider_ptr->AddRegister( "DE", cpu.GetRegisterDE().reg_16 );
	mIndexHL = provider_ptr->AddRegister( "HL", cpu.GetRegisterHL().reg_16 );


	for ( size_t i = 0; i < REGISTER_NAMES.size(); i++ )
	{
		if ( i == 6 ) { continue; }

		mRegisterIndices[i] = provider_ptr->AddRegister( REGISTER_NAMES[i], cpu.GetRegisterValueBy8BitIndex( i ) );
	}

	return provider_ptr;
}

void GameboyCPUBroker::UpdateProvider(GameboyCPU &cpu, std::shared_ptr<CPUProvider> &provider_ref_ptr) const
{
	provider_ref_ptr->UpdateFlag( mIndexC, cpu.GetFlagC() );
	provider_ref_ptr->UpdateFlag( mIndexH, cpu.GetFlagH() );
	provider_ref_ptr->UpdateFlag( mIndexZ, cpu.GetFlagZ() );
	provider_ref_ptr->UpdateFlag( mIndexN, cpu.GetFlagN() );
}

