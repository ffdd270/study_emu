//
// Created by nhy20 on 2020-10-12.
//

#include "cpu/cpu_element.h"
#include "cpu/cpu_provider.h"
#include "imgui.h"

using namespace HaruCar;

void CPU::Elements::RenderFlags(const std::vector<std::string> &flag_names, const std::vector<bool> & flags)
{
	if( flag_names.size() != flags.size() )
	{
		throw std::exception("FLAG NAMES SIZE SHOULD BE SAME AS FLAGS.");
	}

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

void CPU::Elements::RenderRegisterHiLow(const HaruCar::CPU::ProviderRegister & ref_provider_register)
{
	ImGui::Columns( 2, "Hi - Lo Values" );
	ImGui::Separator();

	ImGui::Text( "%s", ref_provider_register.hi_register_name.c_str() ); ImGui::NextColumn();
	ImGui::Text( "%s", ref_provider_register.lo_register_name.c_str() );  ImGui::NextColumn();
	ImGui::Separator();
	ImGui::Text("%d", ref_provider_register.GetHigh() );  ImGui::NextColumn();
	ImGui::Text("%d", ref_provider_register.GetLow() ); ImGui::NextColumn();

	ImGui::Columns(1);
	ImGui::Separator();
}


void CPU::Elements::RenderRegister(const std::vector<std::string> &register_names, const std::vector<HaruCar::CPU::ProviderRegister> &values, float width, float height)
{
	if( register_names.size() != values.size() )
	{
		throw std::exception("FLAG NAMES SIZE SHOULD BE SAME AS FLAGS.");
	}

	if ( register_names.empty() )
	{
		return;
	}

	ImGui::BeginChild( "Registers Viewer", ImVec2( width, height ) );
	{
		ImGui::Columns( register_names.size(), "CPU Register Names" );
		{
			ImGui::Separator();

			for (const auto & register_name : register_names)
			{
				ImGui::Text("%s", register_name.c_str());
				ImGui::NextColumn();
			}

			ImGui::Separator();

			for (const auto & ref_value : values)
			{
				ImGui::Text( "%d", ref_value.register_value );
				ImGui::NextColumn();
			}

			for ( int i = 0; i < values.size(); i++ )
			{
				auto & ref_value = values[i];
				if( ref_value.IsHiLo() )
				{
					std::string str_id = "Hi Low Window" + std::to_string(i);
					ImGui::BeginChild(str_id.c_str(), ImVec2(0, 0 ), false );
					RenderRegisterHiLow( ref_value );
					ImGui::EndChild();
				}
				ImGui::NextColumn();
			}
		}
		ImGui::Columns(1);
		ImGui::Separator();
	}
	ImGui::EndChild();
}

void CPU::Elements::RenderInstructions(const std::vector<std::string> &instructions, const std::vector<int> &opcodes, float width, float height, int id)
{
	if( instructions.size() != opcodes.size() )
	{
		throw std::exception("OPCODES NAMES SIZE SHOULD BE SAME AS INSTRUCTIONS");
	}

	if ( instructions.empty() ) { return; }

	std::string str_id = "Instructions" + std::to_string( id );
	ImGui::Text("LAST INSTRUCTION : %s, 0x%x", instructions[ instructions.size() - 1 ].c_str(), opcodes[ opcodes.size() - 1 ] );

	bool is_visible = ImGui::BeginChild( "Instructions", ImVec2( 200.0f, 200.0f ), true );
	if ( is_visible )
	{
		ImGui::Columns( 2 );
		for( size_t i = 0; i < instructions.size(); i++ )
		{
			ImGui::Text( "%s" ,instructions[i].c_str() );
			ImGui::NextColumn();
			ImGui::Text( "0x%x" ,opcodes[i] );
			ImGui::NextColumn();
		}
		ImGui::Columns();
	}

	ImGui::EndChild();
}
