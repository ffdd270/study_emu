#include "background_test.h"

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "../lua-binding/gameboy_luabinding.h"
#include "playground_util.h"

// https://www.huderlem.com/demos/gameboy2bpp.html  여기 최하단에 있는 걸로 만든
// CGB 기준으로
// 1 => LIGHT_GRAY 8.
// 2 => DARK_GRAY 8,
// 3 => WHITE 8
// 4 => BLACK 8
// 5 => LIGHT_GRAY 4, BLACK_GRAY 4
// 6 => BLACK_GRAY 4, LIGHT_GRAY 4
// 7 => LIGHT_GRAY 4, BLACK GRAY 4
// 8 => WHITE 4, BLACK 4

constexpr std::array< BYTE, 16 > TILE_TEST_DATA = {0xFF, 0x00,
												   0x00, 0xFF,
												   0x00, 0x00,
												   0xFF, 0xFF,
												   0xF0, 0x0F,
												   0x0F, 0xF0,
												   0xF0, 0x0F,
												   0x0F, 0x0F};
};
class TextureTest
{
public:
	TextureTest()
	{
		gpu = std::make_shared<GPU>();
		mTexture.create( GPUHelper::ScreenWidth, GPUHelper::ScreenHeight );
		mPixels.fill( 255 );

		// 타일 맵
		WORD tile_map_start = gpu->GetSelectBGTileMapDisplay();

		// BG 타일 맵은 32 * 32다. 개당 8비트를 표현하는 타일맵이니 256*256이 됨.
		for ( int i = 0; i < 32 * 32; i++ )
		{
			gpu->Set( tile_map_start + i, 0 ); // 1번 타일
		}

		payload_tile_data( gpu, TILE_TEST_DATA, gpu->GetSelectBGAndWindowTileData(), 0 );
		gameboy_lua_binding_gpu( gpu );
	}

	using Pixels = std::array< BYTE, GPUHelper::ScreenWidth * GPUHelper::ScreenHeight * 4 >;

	void renderColor()
	{
		const ColorScreenBits * ptr_bits = gpu->GetColorScreenData();
		const ColorScreenBits & ref_bits = (*ptr_bits);

		// 이게 Draw 로직임.
		for ( int y = 0; y < GPUHelper::ScreenHeight; y++ )
		{
			for( int x = 0; x < GPUHelper::ScreenWidth; x++ )
			{
				const GPUHelper::ColorPallet & ref_pallet = ref_bits[y][x];

				// RGBA
				// 0x1f를 -> 0xff로 사상. 8배 차이니 8 곱해줌.
				// 4는? 4비트라서.
				mPixels[ ( y * GPUHelper::ScreenWidth * 4 ) + ( x * 4 ) + 0 ] = ref_pallet.Red() * 8;
				mPixels[ ( y * GPUHelper::ScreenWidth * 4 ) + ( x * 4 ) + 1 ] = ref_pallet.Blue() * 8;
				mPixels[ ( y * GPUHelper::ScreenWidth * 4 ) + ( x * 4 ) + 2 ] = ref_pallet.Green() * 8;
				mPixels[ ( y * GPUHelper::ScreenWidth * 4 ) + ( x * 4 ) + 3 ] = 255;
			}
		}
	}

	void renderMono( )
	{
		render_mono( gpu, mPixels );
	}

	void render()
	{
		renderMono();
		mTexture.update( mPixels.data() );
	}

	const sf::Texture * getTexture() const
	{
		return &mTexture;
	}

	GPU & getGpu()
	{
		return (*gpu);
	}

	std::shared_ptr<GPU> getSharedPtrGPU()
	{
		return gpu;
	}

private:
	Pixels mPixels;
	std::shared_ptr<GPU> gpu;
	sf::Texture mTexture;
	sf::Sprite mSprite;
};


TextureTest * ptr_texture_test = nullptr;

void texture_test()
{
	if( ptr_texture_test == nullptr )
	{
		ptr_texture_test = new TextureTest();
	}

	ptr_texture_test->render();
	ImGui_Texture_Draw("Background Test", ptr_texture_test->getTexture(), ptr_texture_test->getSharedPtrGPU() );
}