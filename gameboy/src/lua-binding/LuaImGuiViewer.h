//
// Created by HaruGakkaP on 2020-11-04.
//

#ifndef GAMEBOY_LUAIMGUIVIEWER_H
#define GAMEBOY_LUAIMGUIVIEWER_H

#include <base/viewer_interface.h>
#include "LuaContext.h"


class LuaImGuiViewer : public HaruCar::Base::Interface::Viewer
{
public:
	void SetLuaContext( std::shared_ptr<LuaContext> & lua_context );
	void AddCallback();
private:
	std::shared_ptr<LuaContext> mPtrLuaContext = nullptr;
};


#endif //GAMEBOY_LUAIMGUIVIEWER_H
