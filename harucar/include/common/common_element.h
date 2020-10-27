//
// Created by nhy20 on 2020-10-24.
//

#ifndef HARUCAR_COMMON_ELEMENT_H
#define HARUCAR_COMMON_ELEMENT_H

#include "common_structure.h"
#include "common_logger.h"

namespace HaruCar::Common::Elements
{
	void RenderInputTextBox(Structure::InputBuffer & ref_in_out_buffer );
	void RenderLog( const std::vector<Log::LogData> & logs );

}

#endif //HARUCAR_COMMON_ELEMENT_H
