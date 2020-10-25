//
// Created by HaruGakkaP on 2020-10-12.
//

#ifndef HARUCAR_VIEWER_H
#define HARUCAR_VIEWER_H

#include "provider.h"
#include "ui/ui_event_protocol.h"
#include <memory>

class IViewer
{
public:
	virtual void Render(std::shared_ptr<IProvider> provider_ptr, std::shared_ptr<UIEventProtocol> protocol_ptr = nullptr) = 0;
};

#endif //HARUCAR_VIEWER_H
