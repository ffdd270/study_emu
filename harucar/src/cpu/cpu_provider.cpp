//
// Created by HaruGakkaP on 2020-10-12.
//

#include <cpu/cpu_provider.h>
#include <assert.h>

using namespace HaruCar::CPU;

void CPUProvider::SetMemory(std::vector<int> &ref_memory)
{
	mMemory = ref_memory;
}

size_t CPUProvider::AddRegister(const std::string &ref_register_name, const ProviderRegister & register_value)
{	
	mRegisterNames.emplace_back( ref_register_name );
	mRegisters.emplace_back( register_value );

	assert( mRegisterNames.size() == mRegisters.size() );

	return mRegisters.size() - 1;
}

void CPUProvider::UpdateRegister(size_t index, int register_value)
{
	assert( mRegisters.size() > index );

	mRegisters[index].register_value = register_value;
}

size_t CPUProvider::AddFlag(const std::string &ref_flag_name, bool flag_value)
{
	mFlagNames.emplace_back( ref_flag_name );
	mFlags.emplace_back( flag_value );

	assert( mFlagNames.size() == mFlags.size() );

	return mFlags.size() - 1;
}

void CPUProvider::UpdateFlag(size_t index, bool flag_value)
{
	assert( mFlags.size() > index );

	mFlags[index] = flag_value;
}

size_t CPUProvider::AddInstruction(const std::string &ref_instruction, int op_code)
{
	mInstructions.emplace_back( ref_instruction );
	mOpCodes.emplace_back( op_code );

	assert( mInstructions.size() == mOpCodes.size() );

	return mOpCodes.size() - 1;
}

const std::vector<std::string> & CPUProvider::GetRegisterNames() const
{
	return mRegisterNames;
}

const std::vector<ProviderRegister>& CPUProvider::GetRegisterValues() const
{
	return mRegisters;
}

const std::vector<std::string> & CPUProvider::GetFlagNames() const
{
	return mFlagNames;
}

const std::vector<bool> & CPUProvider::GetFlags() const
{
	return mFlags;
}

const std::vector<std::string> &  CPUProvider::GetInstructions() const
{
	return mInstructions;
}

const std::vector<int> & CPUProvider::GetOpCodes() const
{
	return mOpCodes;
}

const std::string & CPUProvider::GetRegisterName(size_t index) const
{
	if ( mRegisterNames.size() <= index ) {  throw std::exception("Register Names Out of Index"); }

	return mRegisterNames[index];
}

const ProviderRegister & CPUProvider::GetRegisterValue(size_t index) const
{
	if ( mRegisters.size() <= index ) { throw std::exception("Register Out of Index"); }

	return mRegisters[index];
}

int CPUProvider::FindRegisterIndex(const std::string &register_name) const
{
	for( int i = 0; i < mRegisterNames.size(); i++ )
	{
		if( mRegisterNames[i] == register_name ) { return i; }
	}

	return -1;
}

const std::string & CPUProvider::GetFlagName(size_t index) const
{
	if ( mFlagNames.size() <= index ) { throw std::exception("Flag Names Out of Index"); }

	return mFlagNames[index];
}

bool CPUProvider::GetFlag(size_t index) const
{
	if ( mFlags.size() <= index ) { throw std::exception("Flag Out of Index"); }

	return mFlags[index];
}

// 100개 이상 들어오면 바꾸기
int CPUProvider::FindFlagIndex(const std::string &flag_name) const
{
	for( int i = 0; i < mFlagNames.size(); i++ )
	{
		if( mFlagNames[i] == flag_name ) { return i; }
	}

	return -1;
}

const std::string & CPUProvider::GetInstruction(size_t index) const
{
	if ( mInstructions.size() <= index ) { throw std::exception("Instruction Out of Index`"); }

	return mInstructions[index];
}

int CPUProvider::GetOpCode(size_t index) const
{
	if ( mOpCodes.size() <= index ) { throw std::exception("OpCode Out of Index`"); }

	return mOpCodes[index];
}

const std::string &CPUProvider::GetLastInstruction() const
{
	if( mInstructions.empty() ) { throw std::exception("SIZE ZERO, BUT ACCESS TO LAST INSTRUCTION"); }
	return mInstructions[ mInstructions.size() - 1 ];
}

int CPUProvider::GetLastOpCode() const
{
	if( mOpCodes.empty() ) { throw std::exception("SIZE ZERO, BUT ACCESS TO LAST OPCODES"); }
	return mOpCodes[ mOpCodes.size() - 1 ];
}

int CPUProvider::FindInstructionIndex(const std::string &instruction_name) const
{
	if ( mInstructions.empty() ) { return -1; }

	for( int i = static_cast<int>(mInstructions.size() - 1); i >= 0; --i )
	{
		if( instruction_name == mInstructions[ i ] ) { return i; }
	}

	return -1;
}

std::vector<int> CPUProvider::FindInstructionIndices(const std::string &instruction_name) const
{
	std::vector<int> indices;

	for (int i = static_cast<int>(mInstructions.size() - 1); i >= 0; --i)
	{
		if (instruction_name == mInstructions[i])
		{
			indices.emplace_back( i );
		}
	}

	return indices;
}
