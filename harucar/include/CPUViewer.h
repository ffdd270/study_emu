//
// Created by HaruGakkaP on 2020-10-12.
//

#ifndef HARUCAR_CPUVIEWER_H
#define HARUCAR_CPUVIEWER_H

#include <vector>
#include <string>

#include "common_structure.h"
#include "Viewer.h"

class CPUViewer : public IViewer
{
public:
	CPUViewer();

	void Render(std::weak_ptr<IProvider> provider_ptr) override;
	void SetInputBuffer( std::shared_ptr<Structure::InputBuffer> & ptr_set_input_buffer );
private:
	std::shared_ptr<Structure::InputBuffer> mPtrInputBuffer;

};

#endif //HARUCAR_CPUVIEWER_H
