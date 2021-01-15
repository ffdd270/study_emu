#include "texture_test.h"

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <memory/GPU.h>

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

constexpr std::array< std::array< BYTE, 3 >, 4 > MONO_RGB_VALUE ={
		std::array<BYTE, 3>({ 8, 24, 32 }),
		std::array<BYTE, 3>({ 52, 104, 86 }),
		std::array<BYTE, 3>({ 136, 192, 112 }),
		std::array<BYTE, 3>({ 224, 248, 208 }),
};

class TextureTest
{
public:
	TextureTest()
	{
		mTexture.create( GPUHelper::ScreenWidth, GPUHelper::ScreenHeight );
		mPixels.fill( 255 );
	}

	using Pixels = std::array< BYTE, GPUHelper::ScreenWidth * GPUHelper::ScreenHeight * 4 >;

	void renderColor()
	{
		const ColorScreenBits * ptr_bits = gpu.GetColorScreenData();
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

	}

	void render()
	{
		gpu.NextStep( 1 );
		mTexture.update( mPixels.data() );
	}

	const sf::Texture * getTexture() const
	{
		return &mTexture;
	}

private:
	Pixels mPixels;
	GPU gpu;
	sf::Texture mTexture;
	sf::Sprite mSprite;
};


TextureTest * ptr_texture_test = nullptr;

void ImGui_Texture_Draw( const sf::Texture * texture_handle )
{
	ImGui::Begin("Texture Test");

	ImGui::Image( *texture_handle );

	ImGui::End();
}

void texture_test()
{
	if( ptr_texture_test == nullptr )
	{
		ptr_texture_test = new TextureTest();
	}

	ptr_texture_test->render();
	ImGui_Texture_Draw(ptr_texture_test->getTexture() );
}