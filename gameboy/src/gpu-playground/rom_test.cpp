#include "rom_test.h"
#include "Motherboard.h"
#include "memory/GPU.h"
#include "memory/MBC1.h"

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "playground_util.h"
#include "../lua-binding/gameboy_luabinding.h"
#include <iostream>


#include "imgui.h"
#include "imgui-SFML.h"


class RomTest
{
public:
	RomTest()
	{
		mPtrGPU = std::static_pointer_cast<GPU>(mMotherboard.GetInterface( Motherboard::Interface_GPU ));

		Cartridge cartridge;
		cartridge.Load("roms/dmg-acid2.gb");

		std::cout << " cartridge.GetCartridgeType() : " << cartridge.GetCartridgeType() << std::endl;

		mPtrMBC1 = std::make_shared<MBC1>( std::move( cartridge ) );
		mMotherboard.SetCartridge( std::static_pointer_cast<MemoryInterface>(mPtrMBC1) );

		mTexture.create( GPUHelper::ScreenWidth, GPUHelper::ScreenHeight );
		mPixels.fill( 255 );

		gameboy_lua_binding_cpu( mMotherboard.GetCPU() );
	}

	void render()
	{
		render_mono( mPtrGPU, mPixels );
		mTexture.update( mPixels.data() );
	}

	const sf::Texture * getTexture() const
	{
		return &mTexture;
	}

	std::shared_ptr<GPU> getSharedPtrGPU()
	{
		return mPtrGPU;
	}

	Motherboard & getMotherboard()
	{
		return mMotherboard;
	}


private:
	Pixels mPixels;
	std::shared_ptr<MBC1> mPtrMBC1;
	std::shared_ptr<GPU> mPtrGPU;
	Motherboard mMotherboard;
	sf::Texture mTexture;
};

RomTest * test_ptr = nullptr;


void ImGuiHelper( const char * name , RomTest * rom_test )
{
	static int addr = 0;
	static int value = 0;
	ImGui::Begin( name );

	ImGui::Image( *(rom_test->getTexture()), sf::Vector2f(  GPUHelper::ScreenWidth  * 4,  GPUHelper::ScreenHeight * 4  ) );

	Motherboard & ref_motherboard = rom_test->getMotherboard();


	if( ImGui::Button("STEP NEXT!") )
	{
		ref_motherboard.Step();
	}

	if( ImGui::Button("STEP DRAW SCREEN!") )
	{
		for ( int i = 0; i < GPUHelper::MaxScanline * GPUHelper::ScreenWidth; i++ )
		{
			ref_motherboard.Step();
		}
	}

	ImGui::End();
}

void rom_test()
{
	if(test_ptr == nullptr)
	{
		test_ptr = new RomTest();
	}

	test_ptr->render();
	ImGuiHelper("ROM Test", test_ptr );
}