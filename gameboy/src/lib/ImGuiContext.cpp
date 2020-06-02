//
// Created by HaruGakkaP on 2020-05-29.
//

#include "ImGuiContext.h"

#include <imgui.h>

void Angelica::ImGuiContext::init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO & io = ImGui::GetIO();
	ImGui::StyleColorsDark();

}
