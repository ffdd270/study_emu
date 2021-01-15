#include "texture_test.h"

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <memory/GPU.h>


class TextureTest
{
public:
	TextureTest()
	{
		mTexture.create( GPUHelper::ScreenWidth, GPUHelper::ScreenHeight );
		mPixels.fill( 255 );

	}

	using Pixels = std::array< UINT8, GPUHelper::ScreenWidth * GPUHelper::ScreenHeight * 4 >;
	using ScreenLine = std::array<GPUHelper::ColorPallet, GPUHelper::ScreenWidth>;

	void render()
	{
		gpu.NextStep( 1 );


		const ColorScreenBits * ptr_bits = gpu.GetScreenData();
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