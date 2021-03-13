//
// Created by nhy20 on 2021-03-13.
//

#include "Timer.h"

Clock::Clock(size_t clock_div) : mClock( 0 ), mClockDiv( clock_div )
{


}

void Clock::SetClockDiv(size_t div_value)
{
	mClockDiv = div_value;
}

void Clock::Update(size_t clock)
{
	mClock += clock;
}

void Clock::Reset()
{
	mClock = 0;
}

BYTE Clock::GetTimerValue() const
{
	return (mClock / mClockDiv) % 0x100;
}

// ---- 여기서부터는 타이머


Timer::Timer() : mDivClock( 256 ), mTimerClock( 1 )
{

}

void Timer::NextStep(size_t clock)
{
	mDivClock.Update( clock );
}


BYTE Timer::Get(size_t mem_addr) const
{
	switch ( mem_addr )
	{
		case 0xff04u:
			return mDivClock.GetTimerValue();
		case 0xff05u:
			break;
		case 0xff06u:
			break;
		case 0xff07u:
			break;
		default:
			break;
	}

	return 0;
}

void Timer::Set(size_t mem_addr, BYTE value)
{
	switch ( mem_addr )
	{
		case 0xff04u:
			mDivClock.Reset();
			break;
		case 0xff05u:
			break;
		case 0xff06u:
			break;
		case 0xff07u:
			break;
		default:
			break;
	}
}

bool Timer::IsReportedInterrupt() const
{
	return false;
}

std::vector<InterruptsType> Timer::GetReportedInterrupts() const
{
	return MemoryInterface::GetReportedInterrupts();
}

void Timer::ResolveInterrupt(InterruptsType resolve_interrupt_address)
{

}

