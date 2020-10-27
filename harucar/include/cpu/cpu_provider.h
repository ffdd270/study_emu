//
// Created by HaruGakkaP on 2020-10-12.
//

#ifndef HARUCAR_CPU_PROVIDER_H
#define HARUCAR_CPU_PROVIDER_H

#include "base/provider_interface.h"
#include <vector>
#include <string>


namespace HaruCar::CPU
{
	struct ProviderRegister
	{
		[[nodiscard]] bool IsHiLo() const { return use_hi_lo; }

		void UseHiLo(const std::string & ref_hi_reg_name, const std::string & ref_log_reg_name, size_t reg_byte_size )
		{
			if ( ( reg_byte_size % 2 ) == 1 )  { throw std::exception("WAS reg_byte_size % 2 == 1."); }
			if ( ( ( reg_byte_size / 2 ) % 2 ) == 1 && ( reg_byte_size != 2 ) ) { throw std::exception("SHOULD BE 2, 4, .... 2^N"); }

			hi_register_name = ref_hi_reg_name;
			lo_register_name = ref_log_reg_name;
			register_byte_size = reg_byte_size;

			use_hi_lo = true;
		}

		[[nodiscard]] int GetHigh() const
		{
			if( !use_hi_lo ) { throw std::exception("NO USE HI LO, BUT ACCESS HI."); }
			if ( ( register_byte_size % 2 ) == 1 )  { throw std::exception("WAS reg_byte_size % 2 == 1."); }
			if ( ( register_byte_size / 2 ) % 2 == 1 && ( register_byte_size != 2 ) ) { throw std::exception("SHOULD BE 2, 4, .... 2^N"); }
			size_t half = register_byte_size / 2;

			int and_value = getBitMaskValue(half);
			and_value = and_value << (half * 8);

			return (register_value & and_value) >> (half * 8);
		}


		[[nodiscard]] int GetLow() const
		{
			if( !use_hi_lo ) { throw std::exception("NO USE HI LO, BUT ACCESS HI."); }
			if ( ( register_byte_size % 2 ) == 1 )   { throw std::exception("WAS reg_byte_size % 2 == 1."); }
			if ( ( register_byte_size / 2 ) % 2 == 1 && ( register_byte_size != 2 ) ) { throw std::exception("SHOULD BE 2, 4, .... 2^N"); }
			size_t half = register_byte_size / 2;

			int or_value = getBitMaskValue(half);

			return (register_value & or_value);
		}

		int register_value = 0;
		std::string hi_register_name;
		std::string lo_register_name;
	private:
		static int getBitMaskValue(size_t half)
		{
			constexpr int _8BIT = 0xff;
			int mask_value = 0;

			for( int i = 0; i < half; i++ )
			{
				mask_value |=  ( _8BIT << (i * 8) ); // byte to bit. so *8.
			}

			return mask_value;
		}
	private:
		size_t register_byte_size;

		bool use_hi_lo = false;
	};

	class CPUProvider : public Base::Interface::Provider
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

		[[nodiscard]] const std::vector<std::string> & GetRegisterNames() const;
		[[nodiscard]] const std::vector<ProviderRegister> & GetRegisterValues() const;

		[[nodiscard]] const std::vector<std::string> & GetFlagNames() const;
		[[nodiscard]] const std::vector<bool> & GetFlags() const;

		[[nodiscard]] const std::vector<std::string> & GetInstructions() const;
		[[nodiscard]] const std::vector<int> & GetOpCodes() const;

		[[nodiscard]] const std::string & GetRegisterName(size_t index) const;
		[[nodiscard]] const ProviderRegister & GetRegisterValue(size_t index) const;
		// 없으면 -1.
		[[nodiscard]] int FindRegisterIndex(const std::string & register_name) const;

		[[nodiscard]] const std::string & GetFlagName(size_t index) const;
		[[nodiscard]] bool GetFlag(size_t index) const;
		// 없으면 -1.
		[[nodiscard]] int FindFlagIndex(const std::string & flag_name) const;

		[[nodiscard]] const std::string & GetInstruction(size_t index) const;
		[[nodiscard]] int GetOpCode(size_t index) const;

		[[nodiscard]] const std::string & GetLastInstruction() const;
		[[nodiscard]] int GetLastOpCode() const;
		size_t GetInstructionsLength() { return mInstructions.size(); }

		// 존재하는 것들중 가장 최신으로. 없으면 -1.
		[[nodiscard]] int FindInstructionIndex(const std::string & instruction_name) const;
		// 최신부터 오래된 것까지
		[[nodiscard]] std::vector<int> FindInstructionIndices(const std::string & instruction_name) const;

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

}
#endif //HARUCAR_CPU_PROVIDER_H
