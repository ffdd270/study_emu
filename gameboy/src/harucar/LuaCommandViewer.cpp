//
// Created by nhy20 on 2020-11-01.
//

#include "LuaCommandViewer.h"
#include "imgui.h"

void LuaCommandViewer::Render(std::shared_ptr<Base::Interface::Provider> provider_ptr,
							  std::shared_ptr<UI::Structure::UIEventProtocol> protocol_ptr)
{
	ImGui::Begin("Lua Command Viewer");

	if (ImGui::Button("Reload"))
	{
		protocol_ptr->AddEvent("Lua:Reload");
	}

	if (ImGui::Button("Execute"))
	{
		protocol_ptr->AddEvent("Lua:Execute");
	}

	ImGui::End();
}
