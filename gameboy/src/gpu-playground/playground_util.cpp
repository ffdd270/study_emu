//
// Created by HaruGakkaP on 2021-01-19.
//

#include "playground_util.h"

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <memory/GPU.h>


void ImGui_Texture_Draw( const char * name ,const sf::Texture * texture_handle, const std::shared_ptr<GPU>& ref_ptr_gpu )
{
	static int addr = 0;
	static int value = 0;
	ImGui::Begin( name );

	ImGui::Image( *texture_handle, sf::Vector2f(  GPUHelper::ScreenWidth  * 2,  GPUHelper::ScreenHeight *2  ) );

	if( ImGui::Button("DRAW NEXT!") )
	{
		ref_ptr_gpu->NextStep( GPUHelper::LinePerDots );
	}

	if( ImGui::Button("DRAW SCREEN!") )
	{
		ref_ptr_gpu->NextStep( GPUHelper::LinePerDots * GPUHelper::ScreenHeight );
	}

	if ( ImGui::InputInt( "INPUT ", &addr, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue ))
	{
		value = ref_ptr_gpu->Get( addr );
	}

	ImGui::SameLine();
	ImGui::Text("Value : %x", value );

	ImGui::Text("GetSelectBGTileMapDisplay : %x ", ref_ptr_gpu->GetSelectBGTileMapDisplay() );
	ImGui::Text("GetSelectBGAndWindowTileData : %x ", ref_ptr_gpu->GetSelectBGAndWindowTileData() );

	ImGui::End();
}

void payload_tile_data(std::shared_ptr<GPU> &ref_gpu_ptr, const std::array<BYTE, 16> &ref_tile_data, WORD tile_start_addr, WORD tile_num)
{
	// 타일이 실제로 있는 곳
	WORD tile_data_start = tile_start_addr + ( tile_num * 16 );


	for ( int i = 0; i < ref_tile_data.size(); i++ )
	{
		ref_gpu_ptr->Set( tile_data_start + i, ref_tile_data[i] ); // 이게 1번 타일
	}
}

constexpr std::array< std::array< BYTE, 3 >, 4 > MONO_RGB_VALUE ={
		std::array<BYTE, 3>({ 224, 248, 208 }),
		std::array<BYTE, 3>({ 136, 192, 112 }),
		std::array<BYTE, 3>({ 52, 104, 86 }),
		std::array<BYTE, 3>({ 8, 24, 32 }),
};

void render_mono(std::shared_ptr<GPU> &ref_gpu_ptr, Pixels &ref_pixels)
{
	const MonoScreenBits * ptr_bits = ref_gpu_ptr->GetMonoScreenData();
	const MonoScreenBits & ref_bits = (*ptr_bits);

	// 이게 Draw 로직임.
	for ( int y = 0; y < GPUHelper::ScreenHeight; y++ )
	{
		for( int x = 0; x < GPUHelper::ScreenWidth; x++ )
		{
			const GPUHelper::MonoPallet & ref_pallet = ref_bits[y][x];

			// RGBA
			// 0x1f를 -> 0xff로 사상. 8배 차이니 8 곱해줌.
			// 4는? 4비트라서.
			ref_pixels[ ( y * GPUHelper::ScreenWidth * 4 ) + ( x * 4 ) + 0 ] = MONO_RGB_VALUE[static_cast<BYTE>(ref_pallet)][0];
			ref_pixels[ ( y * GPUHelper::ScreenWidth * 4 ) + ( x * 4 ) + 1 ] = MONO_RGB_VALUE[static_cast<BYTE>(ref_pallet)][1];
			ref_pixels[ ( y * GPUHelper::ScreenWidth * 4 ) + ( x * 4 ) + 2 ] = MONO_RGB_VALUE[static_cast<BYTE>(ref_pallet)][2];
			ref_pixels[ ( y * GPUHelper::ScreenWidth * 4 ) + ( x * 4 ) + 3 ] = 255;
		}
	}
}