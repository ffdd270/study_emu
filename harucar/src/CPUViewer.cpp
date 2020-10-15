//
// Created by HaruGakkaP on 2020-10-12.
//

#include "CPUViewer.h"
#include "CPUProvider.h"
#include "element/cpu_element.h"

#include "imgui.h"


CPUViewer::CPUViewer()
{

}

void CPUViewer::render(std::weak_ptr<IProvider> provider_ptr)
{
	auto ptr = provider_ptr.lock();
	if ( ptr == nullptr ) { return; }

	std::shared_ptr<const CPUProvider> cpu_provider_ptr = std::static_pointer_cast<const CPUProvider>( ptr );

	ImGui::Begin("CPU Viewer");

	Elements::CPU::RenderFlags( cpu_provider_ptr->GetFlagNames(), cpu_provider_ptr->GetFlags() );
	Elements::CPU::RenderRegister( cpu_provider_ptr->GetRegisterNames(), cpu_provider_ptr->GetRegisterValue() );

	ImGui::End();
}

