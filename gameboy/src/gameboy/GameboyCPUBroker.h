//
// Created by HaruGakkaP on 2020-10-13.
//

#ifndef GAMEBOY_GAMEBOYCPUBROKER_H
#define GAMEBOY_GAMEBOYCPUBROKER_H

#include <cpu/cpu_provider.h>
#include "GameboyCPU.h"
#include <array>

class GameboyCPUBroker
{
public:
	GameboyCPUBroker();

	std::shared_ptr<HaruCar::CPU::CPUProvider> MakeProvider(GameboyCPU & cpu);
	void UpdateProvider(GameboyCPU & cpu, std::shared_ptr<HaruCar::CPU::CPUProvider> & provider_ref_ptr );
	static void Close(GameboyCPU & cpu);
private:
	size_t mIndexC, mIndexH, mIndexZ, mIndexN;
	size_t mIndexAF, mIndexBC, mIndexDE, mIndexHL, mIndexSP, mIndexPC;
	std::array<size_t, 8> mRegisterIndices;

	std::vector<std::string> mAddedInstructions;
	std::vector<int> mAddedOpCodes;
};


#endif //GAMEBOY_GAMEBOYCPUBROKER_H
