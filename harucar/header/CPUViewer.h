//
// Created by HaruGakkaP on 2020-10-12.
//

#ifndef HARUCAR_CPUVIEWER_H
#define HARUCAR_CPUVIEWER_H

#include <vector>
#include <string>

#include "Viewer.h"

class CPUViewer : public IViewer
{
public:
	CPUViewer();

	void render(std::weak_ptr<IProvider> provider_ptr) override;
};

#endif //HARUCAR_CPUVIEWER_H
