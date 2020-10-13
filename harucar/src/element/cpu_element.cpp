//
// Created by nhy20 on 2020-10-12.
//

#include "element/cpu_element.h"
#include "imgui.h"

void Elements::CPU::RenderFlags(const std::vector<std::string> &flag_names, const std::vector<bool> & flags)
{
	assert( flag_names.size() == flags.size() );

	ImGui::Text("CPU Flags");
	ImGui::Columns( 2, "CPU Flags Column" ); // Name, Flag
	ImGui::Separator();
	ImGui::Text("Flag Name"); ImGui::NextColumn();
	ImGui::Text("Flag"); ImGui::NextColumn();
	ImGui::Separator();	

	for ( int i = 0; i < flags.size(); i++ )
	{
		ImGui::Text( "%s", flag_names[i].c_str() ); ImGui::NextColumn();
		ImGui::Text( "%s", flags[i] ? "true" : "false"); ImGui::NextColumn();
	}

	ImGui::Columns(1);
	ImGui::Separator();
}
