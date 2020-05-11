#include <iostream>
#include "hardware/Chip8.h"


int main()
{
	std::cout << "Hello, World!" << std::endl;
	Chip8 chip8;



	std::cout << "chip8.getNextOpCode(); : " << chip8.getNextOpCode() << std::endl;

	return 0;
}


