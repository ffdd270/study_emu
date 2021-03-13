//
// Created by nhy20 on 2021-03-13.
//

#include "Timer.h"
#include <array>

Clock::Clock(size_t clock_div) : mClock( 0 ), mClockDiv( clock_div )
{


}

void Clock::SetClockDiv(size_t div_value)
{
	BYTE timer_value = GetTimerValue();
	mClockDiv = div_value;
	mClock = timer_value * mClockDiv;
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

void Clock::ResetByTimerModulo(BYTE timer_modulo)
{
	mClock = ( timer_modulo * mClockDiv );
}

bool Clock::IsTimerOverflow() const
{
	return ( mClock > mClockDiv ) && GetTimerValue() == 0;
}

void Clock::SetClockValue(BYTE value)
{
	mClock = ( mClockDiv * value );
}

// ---- 여기서부터는 타이머


Timer::Timer() : mDivClock( 256 ), mTimerClock( 1 ), mTimerControl( 0 ), mTimerModulo( 0 ), mTimerEnable( false )
{
	updateTimerControlValues();
}

void Timer::NextStep(size_t clock)
{
	mDivClock.Update( clock );

	if ( mTimerEnable )
	{
		mTimerClock.Update( clock );

		if (mTimerClock.IsTimerOverflow())
		{
			mTimerClock.ResetByTimerModulo( mTimerModulo );
		}
	}
}


BYTE Timer::Get(size_t mem_addr) const
{
	switch ( mem_addr )
	{
		case 0xff04u:
			return mDivClock.GetTimerValue();
		case 0xff05u:
			return mTimerClock.GetTimerValue();
		case 0xff06u:
			return mTimerModulo;
		case 0xff07u:
			return mTimerControl;
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
			mTimerClock.SetClockValue( value );
			break;
		case 0xff06u:
			mTimerModulo = value;
			break;
		case 0xff07u:
			mTimerControl = value & 0x07u; // 비트 0~2.
			updateTimerControlValues();
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

constexpr std::array<size_t, 4> CLOCK_DIV_VALUES = {
		1024, 16, 64, 256
};

void Timer::updateTimerControlValues()
{
	mTimerEnable = ( mTimerControl & 0x04u ) != 0; // BIT 2 is True?
	mTimerClock.SetClockDiv( CLOCK_DIV_VALUES[mTimerControl & 0x03u] );
}

