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

void Elements::RenderLog(const std::vector<Log::LogData> &logs)
{
	ImGui::BeginChild("Console Scrolling Region", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	for ( const Log::LogData & log_data : logs )
	{
		ImGui::Text( "%s", log_data.log.c_str() );
	}

	ImGui::EndChild();
}
