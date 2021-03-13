//
// Created by nhy20 on 2021-03-13.
//

#ifndef GAMEBOY_TIMER_H
#define GAMEBOY_TIMER_H

#include "memory/MemoryInterface.h"

class Clock
{
public:
	Clock(size_t clock_div);

	void SetClockDiv( size_t div_value );
	void ResetByTimerModulo(BYTE timer_modulo);
	void Update(size_t clock);
	void Reset();
	[[nodiscard]] BYTE GetTimerValue() const;
private:
	size_t mClock;
	size_t mClockDiv;
};

class Timer final : public MemoryInterface
{
public:
	Timer();

	[[nodiscard]] BYTE Get(size_t mem_addr) const override;
	void Set(size_t mem_addr, BYTE value) override;

	void Reset() override { }

	[[nodiscard]] bool IsReportedInterrupt() const override;
	[[nodiscard]] std::vector<InterruptsType> GetReportedInterrupts() const override;
	void ResolveInterrupt(InterruptsType resolve_interrupt_address) override;

	void NextStep(size_t clock);

	~Timer() final = default;
private:
	void updateTimerControlValues();
private:
	Clock mDivClock;
	Clock mTimerClock;
	
	BYTE mTimerControl;
	bool mTimerEnable;

	BYTE mTimerModulo;
};


#endif //GAMEBOY_TIMER_H
