#include "rom_test.h"
#include "Motherboard.h"
#include "memory/GPU.h"

class RomTest
{
public:
	RomTest()
	{
		gpu = motherboard.GetInterface( Motherboard::Interface_GPU );
	}

	void render()
	{


	}


private:
	std::shared_ptr<GPU> gpu;
	Motherboard motherboard;
};



void rom_test()
{

}