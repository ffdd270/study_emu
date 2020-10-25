//
// Created by HaruGakkaP on 2020-10-12.
//

#ifndef HARUCAR_CPU_VIEWER_H
#define HARUCAR_CPU_VIEWER_H

#include <vector>
#include <string>

#include "common/common_structure.h"
#include "base/viewer.h"

class CPUViewer : public IViewer
{
public:
	CPUViewer() = default;

	void Render(std::shared_ptr<IProvider> provider_ptr, std::shared_ptr<UIEventProtocol> protocol_ptr) override;
	void SetInputBuffer( std::shared_ptr<Structure::InputBuffer> & ptr_set_input_buffer );
private:
	std::shared_ptr<Structure::InputBuffer> mPtrInputBuffer;

};

#endif //HARUCAR_CPU_VIEWER_H
