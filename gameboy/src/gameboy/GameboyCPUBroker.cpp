//
// Created by HaruGakkaP on 2020-10-13.
//

#include "GameboyCPUBroker.h"

using namespace HaruCar::CPU;

constexpr size_t NO_INITED = 0xDEADBEEF;

static const std::array<const char *, 8> REGISTER_NAMES =
		{ "B", "C", "D", "E",
		  "H", "L", "NIL", "A" };

GameboyCPUBroker::GameboyCPUBroker() : mIndexC( NO_INITED ), mIndexH( NO_INITED ), mIndexN( NO_INITED ), mIndexZ( NO_INITED ),
	mRegisterIndices( { NO_INITED })
{

}

static ProviderRegister providerMaker( WORD reg_16_value, const std::string & ref_hi_reg_name, const std::string & ref_lo_reg_name  )
{
	ProviderRegister reg;
	reg.register_value = reg_16_value;
	reg.UseHiLo( ref_hi_reg_name, ref_lo_reg_name, 2 );

	return reg;
}

static ProviderRegister providerMaker( WORD reg_16_value )
{
	ProviderRegister reg;
	reg.register_value = reg_16_value;
	return reg;
}

std::shared_ptr<CPUProvider> GameboyCPUBroker::MakeProvider(GameboyCPU &cpu)
{
	std::shared_ptr<CPUProvider> provider_ptr = std::make_shared<CPUProvider>();

	mIndexC = provider_ptr->AddFlag( "C (Carry Flag)", cpu.GetFlagC() );
	mIndexH = provider_ptr->AddFlag( "H (Half Flag)", cpu.GetFlagH() );
	mIndexZ = provider_ptr->AddFlag( "Z (Zero Flag)", cpu.GetFlagZ() );
	mIndexN = provider_ptr->AddFlag( "N (Negative Flag)", cpu.GetFlagN() );

	mIndexAF = provider_ptr->AddRegister( "AF", providerMaker( cpu.GetRegisterAF().reg_16 , "A", "F" ) );
	mIndexBC = provider_ptr->AddRegister( "BC", providerMaker( cpu.GetRegisterBC().reg_16, "B", "C" ));
	mIndexDE = provider_ptr->AddRegister( "DE", providerMaker( cpu.GetRegisterDE().reg_16, "D", "E" ));
	mIndexHL = provider_ptr->AddRegister( "HL", providerMaker( cpu.GetRegisterHL().reg_16, "H", "L" ) );
	mIndexSP = provider_ptr->AddRegister( "SP", providerMaker( cpu.GetRegisterSP().reg_16 ));
	mIndexPC = provider_ptr->AddRegister( "PC", providerMaker( cpu.GetRegisterPC().reg_16 ));

	cpu.SetOnInstructionCallback([&](const char * instruction, BYTE opcode)
	{
		this->mAddedInstructions.emplace_back( instruction );
		this->mAddedOpCodes.emplace_back( opcode );
	});

	return provider_ptr;
}

void GameboyCPUBroker::UpdateProvider(GameboyCPU &cpu, std::shared_ptr<CPUProvider> &provider_ref_ptr)
{
	provider_ref_ptr->UpdateFlag( mIndexC, cpu.GetFlagC() );
	provider_ref_ptr->UpdateFlag( mIndexH, cpu.GetFlagH() );
	provider_ref_ptr->UpdateFlag( mIndexZ, cpu.GetFlagZ() );
	provider_ref_ptr->UpdateFlag( mIndexN, cpu.GetFlagN() );

	provider_ref_ptr->UpdateRegister( mIndexAF, cpu.GetRegisterAF().reg_16 );
	provider_ref_ptr->UpdateRegister( mIndexBC, cpu.GetRegisterBC().reg_16 );
	provider_ref_ptr->UpdateRegister( mIndexDE, cpu.GetRegisterDE().reg_16 );
	provider_ref_ptr->UpdateRegister( mIndexHL, cpu.GetRegisterHL().reg_16 );
	provider_ref_ptr->UpdateRegister( mIndexSP, cpu.GetRegisterSP().reg_16 );
	provider_ref_ptr->UpdateRegister( mIndexPC, cpu.GetRegisterPC().reg_16 );

	for( size_t i = 0; i < mAddedInstructions.size(); i++ )
	{
		provider_ref_ptr->AddInstruction( mAddedInstructions[i], mAddedOpCodes[i] );
	}

	mAddedInstructions.clear();
	mAddedOpCodes.clear();
}

void GameboyCPUBroker::Close( GameboyCPU & cpu )
{
	cpu.RemoveInstructionCallback();
}


