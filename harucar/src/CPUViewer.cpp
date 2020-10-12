//
// Created by HaruGakkaP on 2020-10-12.
//

#include "CPUViewer.h"
#include "imgui.h"

CPUViewer::CPUViewer()
{

}

void CPUViewer::render(std::weak_ptr<IProvider> provider_ptr)
{
	auto ptr = provider_ptr.lock();
	if ( ptr == nullptr ) { return; }

	ImGui::Begin("CPU Viewer");



}

