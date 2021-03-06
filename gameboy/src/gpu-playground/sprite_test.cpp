#include "sprite_test.h"
#include "playground_util.h"

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

// A라고 그려져 있음
constexpr std::array< BYTE, 16 > TILE_TEST_DATA = {
		0x7C, 0x7C, 0x00, 0xC6,
		0xC6, 0x00, 0x00, 0xFE,
		0xC6, 0xC6, 0x00, 0xC6,
		0xC6, 0x00, 0x00, 0x00
};

constexpr std::array<BYTE, 16> TILE_WINDOW_TEST_DATA = {
		0xFF, 0x00, 0x7E, 0xFF,
		0x85, 0x81, 0x89, 0x83,
		0x93, 0x85, 0xA5, 0x8B,
		0xC9, 0x97, 0x7E, 0xFF,
};

inline void set_attr( std::shared_ptr<GPU> & ref_gpu_ptr, BYTE index, GPUHelper::ObjectAttribute attr )
{
	WORD base_object_attribute_memory_addr = 0xfe00;
	WORD object_attribute_memory_addr = base_object_attribute_memory_addr + ( index * 4 );

	ref_gpu_ptr->Set( object_attribute_memory_addr + 0, attr.data[ 0 ] );
	ref_gpu_ptr->Set( object_attribute_memory_addr + 1, attr.data[ 1 ] );
	ref_gpu_ptr->Set( object_attribute_memory_addr + 2, attr.data[ 2 ] );
	ref_gpu_ptr->Set( object_attribute_memory_addr + 3, attr.data[ 3 ] );
}

class SpriteTest
{
public:
	SpriteTest()
	{
		mGPUPtr = std::make_shared<GPU>();
		mTexture.create( GPUHelper::ScreenWidth, GPUHelper::ScreenHeight );
		mPixels.fill( 255 );

		// OAM 세팅
		GPUHelper::ObjectAttribute attr {};
		attr.attributes.data = 0;
		attr.y_position = 50;
		attr.x_position = 50;
		attr.sprite_tile_number = 1;

		set_attr( mGPUPtr, 0, attr );

		for( int i = 1; i <= 19; i++ )
		{
			attr.y_position = 200;
			set_attr( mGPUPtr, i, attr );
		}

		payload_tile_data( mGPUPtr, TILE_TEST_DATA, 0x8000, 0 );
		payload_tile_data( mGPUPtr, TILE_WINDOW_TEST_DATA,  0x8000, 1 );


		// BG Pallet
		mGPUPtr->Set( 0xff47, 0b1110001 );


		// Object 2
		// 4->BLACK
		// 3->어두운 초록색
		// 2-> 밝은 초록색
		// 1-> 투명
		mGPUPtr->Set( 0xff48, 0b11100100 );
	}

	void renderMono( )
	{
		render_mono( mGPUPtr, mPixels );
	}

	void render()
	{
		renderMono();
		mTexture.update( mPixels.data() );
	}


	std::shared_ptr<GPU> getSharedPtrGPU()
	{
		return mGPUPtr;
	}

	const sf::Texture * getTexture() const
	{
		return &mTexture;
	}



private:
	Pixels mPixels;
	std::shared_ptr<GPU> mGPUPtr;
	sf::Texture mTexture;
};

SpriteTest * test_ptr = nullptr;

void sprite_test()
{
	if ( test_ptr == nullptr )
	{
		test_ptr = new SpriteTest();
	}

	test_ptr->render();
	ImGui_Texture_Draw("Background Test", test_ptr->getTexture(), test_ptr->getSharedPtrGPU() );
}
