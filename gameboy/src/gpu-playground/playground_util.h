//
// Created by HaruGakkaP on 2021-01-19.
//

#ifndef GAMEBOY_PLAYGROUND_UTIL_H
#define GAMEBOY_PLAYGROUND_UTIL_H

#include <array>
#include <memory>
#include "../gameboy/typedef.h"
#include <memory/GPU.h>

namespace sf
{
	class Texture;
}

void ImGui_Texture_Draw( const char * name ,const sf::Texture * texture_handle, const std::shared_ptr<GPU>& ref_ptr_gpu );
void payload_tile_data( std::shared_ptr<GPU> & ref_gpu_ptr, const std::array<BYTE, 16> & ref_tile_data, WORD tile_start_addr, WORD tile_num = 0 );
void render_mono( std::shared_ptr<GPU> & ref_gpu_ptr, Pixels & ref_pixels );

#endif //GAMEBOY_PLAYGROUND_UTIL_H
