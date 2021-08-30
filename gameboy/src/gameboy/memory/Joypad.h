//
// Created by nhy20 on 2021-08-30.
//

#ifndef GAMEBOY_JOYPAD_H
#define GAMEBOY_JOYPAD_H

#include "MemoryInterface.h"

enum class JoypadKey {
	Right  = 0b00000001,
	Left   = 0b00000010,
	Up     = 0b00000100,
	Down   = 0b00001000,
	A      = 0b00010000,
	B      = 0b00100000,
	Select = 0b01000000,
	Start  = 0b10000000,
	};

class Joypad : public MemoryInterface
{
public:

	[[nodiscard]] BYTE Get( size_t mem_addr ) const override;
	void Set( size_t mem_addr, BYTE value ) override;

	void Reset() override;

	void ResolveInterrupt(InterruptsType reslove_interrupt_address) override;
	[[nodiscard]] bool IsReportedInterrupt() const override;

	// GetReportedInterrupts ->  보고된 인터럽트의 ENUM 리스트. 이 리스트 기반으로 마더보드가 문제를 처리함.
	[[nodiscard]] std::vector<InterruptsType> GetReportedInterrupts() const override { return {InterruptsType::JOYPAD}; }

	void KeyUp(JoypadKey key);
	void KeyDown(JoypadKey key);
private:
	bool interrupted = false;
	BYTE joypad_vector = 0;
	BYTE select_status = 1;
};


#endif //GAMEBOY_JOYPAD_H
