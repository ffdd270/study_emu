//
// Created by nhy20 on 2020-12-04.
//

#ifndef GAMEBOY_MEMORYUNIT_H
#define GAMEBOY_MEMORYUNIT_H

#include "MemoryInterface.h"

class MemoryUnit : public MemoryInterface
{
public:
	[[nodiscard]] BYTE Get(size_t mem_addr) const override;
 	void Set(size_t mem_addr, BYTE value) override;

private:

};


#endif //GAMEBOY_MEMORYUNIT_H
