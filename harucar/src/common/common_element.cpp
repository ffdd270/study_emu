//
// Created by nhy20 on 2020-10-24.
//

#include <common/common_element.h>
#include <imgui.h>

using namespace HaruCar;
using namespace HaruCar::Common;


void Common::Elements::RenderInputTextBox(Structure::InputBuffer &ref_in_out_buffer)
{
	ImGui::InputTextMultiline( "##input_source", ref_in_out_buffer.GetBuffer(), ref_in_out_buffer.GetBufferSize() );
}