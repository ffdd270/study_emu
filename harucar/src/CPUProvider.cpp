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

	return mRegisters.size();
}

size_t CPUProvider::AddFlag(const std::string &ref_flag_name, bool flag_value)
{
	mFlagNames.push_back( ref_flag_name );
	mFlags.push_back( flag_value );

	assert( mFlagNames.size() == mFlags.size() );

	return mFlags.size();
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

const std::string *CPUProvider::GetRegisterName(size_t index) const
{
	if ( mRegisterNames.size() <= index ) { return nullptr; }

	return &mRegisterNames[index];
}

int CPUProvider::GetRegisterValue(size_t index) const
{
	if ( mRegisters.size() <= index ) { assert(false); return -765; }

	return mRegisters[index];
}

const std::string *CPUProvider::GetFlagName(size_t index) const
{
	if ( mFlagNames.size() <= index ) { return nullptr; }

	return &mFlagNames[index];
}

bool CPUProvider::GetFlag(size_t index) const
{
	if ( mFlags.size() <= index ) { assert(false); return false; }

	return mRegisters[index];
}

const std::string *CPUProvider::GetInstruction(size_t index) const
{
	if ( mInstructions.size() <= index ) { return nullptr; }

	return &mInstructions[index];
}

int CPUProvider::GetOpCode(size_t index) const
{
	if ( mOpCodes.size() <= index ) { assert(false); return mOpCodes[index]; }

	return mOpCodes[index];
}

