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