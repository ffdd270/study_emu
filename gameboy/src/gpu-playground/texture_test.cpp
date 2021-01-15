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

	void render()
	{
		mTexture.update( mPixels.data() );
	}

	UINT32 getTexture()
	{
		return mTexture.getNativeHandle();
	}

private:
	std::array< UINT8, GPUHelper::ScreenWidth * GPUHelper::ScreenHeight * 4 > mPixels;
	sf::Texture mTexture;
};


TextureTest * ptr_texture_test = nullptr;

void ImGui_Texture_Draw( UINT32 native_texture_handle )
{
	ImGui::Begin("Texture Test");

	ImGui::Image( static_cast<ImTextureID>(&native_texture_handle), ImVec2( GPUHelper::ScreenWidth, GPUHelper::ScreenHeight ) );

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