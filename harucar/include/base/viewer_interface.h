//
// Created by HaruGakkaP on 2020-10-12.
//

#ifndef HARUCAR_VIEWER_INTERFACE_H
#define HARUCAR_VIEWER_INTERFACE_H

#include "provider_interface.h"
#include "ui/ui_event_protocol_structure.h"
#include <memory>

namespace HaruCar::Base::Interface
{
	class Viewer
	{
	public:
		virtual void Render(std::shared_ptr<Provider> provider_ptr, std::shared_ptr<UI::Structure::UIEventProtocol> protocol_ptr) = 0;
	};
}

#endif //HARUCAR_VIEWER_INTERFACE_H
