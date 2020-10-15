//
// Created by HaruGakkaP on 2020-10-12.
//

#include <CPUProvider.h>
#include <assert.h>

void CPUProvider::SetMemory(std::vector<int> &ref_memory)
{
	mMemory = ref_memory;
}

size_t CPUProvider::AddRegister(const std::string &ref_register_name, int register_value)
{
	mRegisterNames.push_back( ref_register_name );
	mRegisters.push_back( register_value );

	assert( mRegisterNames.size() == mRegisters.size() );

	return mRegisters.size() - 1;
}

void CPUProvider::UpdateRegister(size_t index, int register_value)
{
	assert( mRegisters.size() > index );

	mRegisters[index] = register_value;
}

size_t CPUProvider::AddFlag(const std::string &ref_flag_name, bool flag_value)
{
	mFlagNames.push_back( ref_flag_name );
	mFlags.push_back( flag_value );

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
	mInstructions.push_back( ref_instruction );
	mOpCodes.push_back( op_code );

	assert( mInstructions.size() == mOpCodes.size() );

	return mOpCodes.size();
}

const std::vector<std::string> & CPUProvider::GetRegisterNames() const
{
	return mRegisterNames;
}

std::vector<int> CPUProvider::GetRegisterValue() const
{
	std::vector<int> view_array;

	for ( auto & ref_value : mRegisters )
	{
		view_array.push_back( ref_value );
	}

	return view_array;
}

const std::vector<std::string> & CPUProvider::GetFlagNames() const
{
	return mFlagNames;
}

std::vector<bool> CPUProvider::GetFlags() const
{
	std::vector<bool> view_array;

	for ( auto ref_flag : mFlags )
	{
		view_array.push_back( ref_flag );
	}

	return view_array;
}


const std::vector<std::string> &  CPUProvider::GetInstructions() const
{
	return mInstructions;
}

std::vector<int> CPUProvider::GetOpCodes() const
{
	std::vector<int> view_array;

	for ( auto ref_value : mOpCodes )
	{
		view_array.push_back( ref_value );
	}

	return view_array;
}

std::string CPUProvider::GetRegisterName(size_t index) const
{
	if ( mRegisterNames.size() <= index ) {  throw std::exception("Register Names Out of Index"); }

	return mRegisterNames[index];
}

int CPUProvider::GetRegisterValue(size_t index) const
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

std::string CPUProvider::GetFlagName(size_t index) const
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

std::string CPUProvider::GetInstruction(size_t index) const
{
	if ( mInstructions.size() <= index ) { throw std::exception("Instruction Out of Index`"); }

	return mInstructions[index];
}

int CPUProvider::GetOpCode(size_t index) const
{
	if ( mOpCodes.size() <= index ) { throw std::exception("OpCode Out of Index`"); }

	return mOpCodes[index];
}
