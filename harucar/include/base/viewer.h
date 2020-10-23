//
// Created by HaruGakkaP on 2020-10-12.
//

#ifndef HARUCAR_VIEWER_H
#define HARUCAR_VIEWER_H

#include "provider.h"
#include <memory>

class IViewer
{
public:
	virtual void Render(std::weak_ptr<IProvider> provider_ptr) = 0;
};

#endif //HARUCAR_VIEWER_H
