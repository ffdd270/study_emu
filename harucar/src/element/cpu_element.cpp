//
// Created by nhy20 on 2020-10-12.
//

#include "element/cpu_element.h"
#include "CPUProvider.h"
#include "imgui.h"

void Elements::CPU::RenderFlags(const std::vector<std::string> &flag_names, const std::vector<bool> & flags)
{
	if( flag_names.size() != flags.size() )
	{
		throw std::exception("FLAG NAMES SIZE SHOULD BE SAME AS FLAGS.");
	}

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

void Elements::CPU::RenderRegister(const std::vector<std::string> &register_names, const std::vector<ProviderRegister> &values)
{
	if( register_names.size() != values.size() )
	{
		throw std::exception("FLAG NAMES SIZE SHOULD BE SAME AS FLAGS.");
	}

	if ( register_names.empty() )
	{
		return;
	}

	ImGui::Text( "CPU Registers" );
	ImGui::Columns( register_names.size(), "CPU Register Names" );
	ImGui::Separator();

	for ( int i = 0; i < register_names.size(); i++)
	{
		ImGui::Text("%s", register_names[i].c_str());
		ImGui::NextColumn();
	}

	ImGui::Separator();

	for ( int i = 0; i < values.size(); i++ )
	{
		ImGui::Text( "%d", values[i].register_value );

		if( values[i].IsHiLo() )
		{

		}

		ImGui::NextColumn();
	}

	ImGui::Columns(1);
	ImGui::Separator();
}
