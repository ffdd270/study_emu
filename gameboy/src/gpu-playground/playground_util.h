//
// Created by HaruGakkaP on 2021-01-19.
//

#ifndef GAMEBOY_PLAYGROUND_UTIL_H
#define GAMEBOY_PLAYGROUND_UTIL_H

#include <memory>

namespace sf
{
	class Texture;
}

class GPU;

void ImGui_Texture_Draw( const char * name ,const sf::Texture * texture_handle, const std::shared_ptr<GPU>& ref_ptr_gpu );

#endif //GAMEBOY_PLAYGROUND_UTIL_H
