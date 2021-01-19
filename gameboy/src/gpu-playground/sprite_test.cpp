#include "sprite_test.h"
#include "../gameboy/memory/GPU.h"

class SpriteTest
{
public:
	SpriteTest()
	{
		mGPUPtr = std::make_shared<GPU>();
	}

	void render()
	{
	}

private:
	std::shared_ptr<GPU> mGPUPtr;
};


void sprite_test()
{



}
