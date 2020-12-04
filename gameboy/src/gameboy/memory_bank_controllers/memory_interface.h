//
// Created by nhy20 on 2020-12-04.
//

#ifndef GAMEBOY_MEMORY_INTERFACE_H
#define GAMEBOY_MEMORY_INTERFACE_H


#include "typedef.h"

/*
 * 이건 메모리다.
 */

class MemoryInterface
{
public:
	[[nodiscard]] virtual BYTE Get(size_t mem_addr) const = 0;
	virtual void Set(size_t mem_addr, BYTE value) = 0;
};


#endif //GAMEBOY_MEMORY_INTERFACE_H
