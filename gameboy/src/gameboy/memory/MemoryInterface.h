//
// Created by nhy20 on 2020-12-04.
//

#ifndef GAMEBOY_MEMORYINTERFACE_H
#define GAMEBOY_MEMORYINTERFACE_H


#include "typedef.h"
#include <vector>
/*
 * 이건 메모리다.
 */

class MemoryInterface
{
public:
	[[nodiscard]] virtual BYTE Get(size_t mem_addr) const = 0;
	virtual void Set(size_t mem_addr, BYTE value) = 0;

	void SetWord( size_t mem_addr, WORD value )
	{
		BYTE hi = static_cast<WORD>( 0xFF00u & value ) >> 8u; // hi byte,
		BYTE lo = 0x00FFu & value; // lo byte

		Set( mem_addr, lo );
		Set( mem_addr + 1, hi );
	}

	virtual void Reset() = 0; 

	// IsReportedInterrupt -> 마더보드에게 처리를 요청할 인터럽트가 있었나?
	[[nodiscard]] virtual bool IsReportedInterrupt() const = 0;

	// 해결 
	virtual void ResolveInterrupt(InterruptsType resolve_interrupt_address) = 0;

	// GetReportedInterrupts ->  보고된 인터럽트의 ENUM 리스트. 이 리스트 기반으로 마더보드가 문제를 처리함.
	[[nodiscard]] virtual std::vector<InterruptsType> GetReportedInterrupts() const { return {}; }

	virtual ~MemoryInterface() {};
};


#endif //GAMEBOY_MEMORYINTERFACE_H
