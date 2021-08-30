//
// Created by nhy20 on 2021-08-30.
//

#include "Joypad.h"


BYTE Joypad::Get(size_t mem_addr) const
{
	if ( ( select_status & 0b00010000 ) == 0)
	{
		return select_status | (~joypad_vector & 0x0f);
	}

	if ( ( select_status & 0b00100000 )  == 0 )
	{
		return select_status | ((~joypad_vector & 0xf0) >> 4);
	}

	return select_status;
}


void Joypad::Set(size_t mem_addr, BYTE value)
{
	select_status = value;
}

void Joypad::Reset()
{
	joypad_vector = false;
}


void Joypad::ResolveInterrupt(InterruptsType reslove_interrupt_address)
{
	interrupted = false;
}

bool Joypad::IsReportedInterrupt() const
{
	return interrupted;
}

void Joypad::KeyDown(JoypadKey key)
{
	interrupted = true;
	joypad_vector |= static_cast<BYTE>(key);
}

void Joypad::KeyUp(JoypadKey key)
{
	joypad_vector &= ~static_cast<BYTE>(key);
}
