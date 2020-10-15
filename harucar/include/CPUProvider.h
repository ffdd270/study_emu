//
// Created by HaruGakkaP on 2020-10-12.
//

#ifndef HARUCAR_CPUPROVIDER_H
#define HARUCAR_CPUPROVIDER_H

#include "Provider.h"
#include <vector>
#include <string>


struct ProviderRegister
{
	bool IsHiLo() const { return use_hi_lo; }

	void UseHiLo( const std::string & hi_register_name, const std::string & lo_register_name, size_t register_byte_size )
	{
		if ( ( register_byte_size % 2 ) == 1 ) { throw std::exception("SHOULD BE 2, 4, .... 짝수."); }

		this->hi_register_name = hi_register_name;
		this->lo_register_name = lo_register_name;

		use_hi_lo = true;
	}

	int GetHigh() const
	{
		if( !use_hi_lo ) { throw std::exception("NO USE HI LO, BUT ACCESS HI."); }
		size_t half = register_byte_size / 2;
		if ( ( half % 2 ) == 1 ) { throw std::exception("SHOULD BE 2, 4, .... 짝수."); }

		int and_value = getBitMaskValue(half);
		and_value = and_value < (half * 8);

		return (register_value & and_value) > (half * 4);
	}


	int GetLow() const
	{
		if( !use_hi_lo ) { throw std::exception("NO USE HI LO, BUT ACCESS HI."); }
		size_t half = register_byte_size / 2;
		if ( ( half % 2 ) == 1 ) { throw std::exception("SHOULD BE 2, 4, .... 짝수."); }

		int or_value = getBitMaskValue(half);

		return (register_value & or_value);
	}

	int register_value = 0;
	std::string hi_register_name = "";
	std::string lo_register_name = "";
private:
	int getBitMaskValue(size_t half) const
	{

		constexpr int _8BIT = 0xff;
		int mask_value = 0;

		for( int i = 0; i < half; i++ )
		{
			mask_value |= _8BIT < (i * 8); // byte to bit. so *4.
		}
	}
private:
	size_t register_byte_size;

	bool use_hi_lo = false;
};

class CPUProvider : public IProvider
{
public:
	void SetMemory( std::vector<int> & ref_memory );

	// 인덱스 발급.
	size_t AddRegister(const std::string & ref_register_name, const ProviderRegister & register_value);
	void UpdateRegister(size_t index, int register_value);

	// 인덱스 발급
	size_t AddFlag(const std::string & ref_flag_name, bool flag_value);
	void UpdateFlag(size_t index, bool flag_value);

	// 인덱스 발급
	size_t AddInstruction(const std::string & ref_instruction, int op_code);

	const std::vector<std::string> & GetRegisterNames() const;
	const std::vector<ProviderRegister>& GetRegisterValues() const;

	const std::vector<std::string> & GetFlagNames() const;
	std::vector<bool> GetFlags() const;


	const std::vector<std::string> & GetInstructions() const;
	std::vector<int> GetOpCodes() const;

	std::string GetRegisterName(size_t index) const;
	ProviderRegister GetRegisterValue(size_t index) const;
	// 없으면 -1.
	int FindRegisterIndex(const std::string & register_name) const;

	std::string GetFlagName(size_t index) const;
	bool GetFlag(size_t index) const;
	// 없으면 -1.
	int FindFlagIndex(const std::string & flag_name) const;

	std::string GetInstruction(size_t index) const;
	int GetOpCode(size_t index) const;

private:
	std::vector<int> mMemory;

	// mRegisterNames[i] == mRegisters[i] of Name.
	std::vector<std::string> mRegisterNames;
	std::vector<ProviderRegister> mRegisters;

	// mFlags[i] == mFlagNames[i]
	std::vector<std::string> mFlagNames;
	std::vector<bool> mFlags;

	// mInstructions[i] == mOpCodes[i] of Instruction name.
	std::vector<std::string> mInstructions;
	std::vector<int> mOpCodes;
};

#endif //HARUCAR_CPUPROVIDER_H
