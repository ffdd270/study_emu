//
// Created by nhy20 on 2020-11-01.
//

#ifndef GAMEBOY_LUACOMMANDVIEWER_H
#define GAMEBOY_LUACOMMANDVIEWER_H

#include "base/viewer_interface.h"

using namespace HaruCar;


class LuaCommandViewer : public HaruCar::Base::Interface::Viewer
{
public:
	LuaCommandViewer() = default;

	void Render( std::shared_ptr<Base::Interface::Provider> provider_ptr, std::shared_ptr<UI::Structure::UIEventProtocol> protocol_ptr ) override;
};


#endif //GAMEBOY_LUACOMMANDVIEWER_H
