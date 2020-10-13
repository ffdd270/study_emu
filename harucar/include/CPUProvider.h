//
// Created by HaruGakkaP on 2020-10-12.
//

#ifndef HARUCAR_CPUPROVIDER_H
#define HARUCAR_CPUPROVIDER_H

#include "Provider.h"
#include <vector>
#include <string>


class CPUProvider : public IProvider
{
public:
	void SetMemory( std::vector<int> & ref_memory );

	// 인덱스 발급.
	size_t AddRegister(const std::string & ref_register_name, int register_value);
	void UpdateRegister(size_t index, int register_value);

	// 인덱스 발급
	size_t AddFlag(const std::string & ref_flag_name, bool flag_value);
	void UpdateFlag(size_t index, bool flag_value);

	// 인덱스 발급
	size_t AddInstruction(const std::string & ref_instruction, int op_code);

	const std::vector<std::string> & GetRegisterNames() const;
	std::vector<int> GetRegisterValue() const;

	const std::vector<std::string> & GetFlagNames() const;
	std::vector<bool> GetFlags() const;

	const std::vector<std::string> & GetInstructions() const;
	std::vector<int> GetOpCodes() const;

	const std::string * GetRegisterName(size_t index) const;
	int GetRegisterValue(size_t index) const;

	const std::string * GetFlagName(size_t index) const;
	bool GetFlag(size_t index) const;

	const std::string * GetInstruction(size_t index) const;
	int GetOpCode(size_t index) const;

private:
	std::vector<int> mMemory;

	// mRegisterNames[i] == mRegisters[i] of Name.
	std::vector<std::string> mRegisterNames;
	std::vector<int> mRegisters;

	// mFlags[i] == mFlagNames[i]
	std::vector<std::string> mFlagNames;
	std::vector<bool> mFlags;

	// mInstructions[i] == mOpCodes[i] of Instruction name.
	std::vector<std::string> mInstructions;
	std::vector<int> mOpCodes;
};

#endif //HARUCAR_CPUPROVIDER_H
