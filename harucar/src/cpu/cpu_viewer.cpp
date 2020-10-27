//
// Created by HaruGakkaP on 2020-10-12.
//

#include "cpu/cpu_viewer.h"
#include "cpu/cpu_provider.h"
#include "cpu/cpu_element.h"
#include "common/common_element.h"
#include "imgui.h"

using namespace HaruCar;
using namespace HaruCar::CPU;
using namespace HaruCar::Base::Interface;

void CPUViewer::Render(std::shared_ptr<Provider> provider_ptr, std::shared_ptr<UI::Structure::UIEventProtocol> protocol_ptr)
{
	if ( provider_ptr == nullptr ) { return; }

	std::shared_ptr<CPUProvider> cpu_provider_ptr = std::static_pointer_cast<CPUProvider>( provider_ptr );

	ImGui::Begin("CPU Viewer");

	if( ImGui::TreeNode("Flags") )
	{
		CPU::Elements::RenderFlags(cpu_provider_ptr->GetFlagNames(), cpu_provider_ptr->GetFlags() );

		ImGui::TreePop();
	}

	if ( ImGui::TreeNode("Registers") )
	{
		CPU::Elements::RenderRegister(cpu_provider_ptr->GetRegisterNames(), cpu_provider_ptr->GetRegisterValues() );

		ImGui::TreePop();
	}

	if ( ImGui::TreeNode("Instructions"))
	{
		CPU::Elements::RenderInstructions(cpu_provider_ptr->GetInstructions(), cpu_provider_ptr->GetOpCodes() );

		ImGui::TreePop();
	}

	if ( mPtrInputBuffer != nullptr && ImGui::TreeNode("Input") )
	{
		Common::Elements::RenderInputTextBox(*mPtrInputBuffer );

		if( ImGui::Button("Injection") )
		{
			if ( protocol_ptr != nullptr )
			{
				protocol_ptr->AddEvent("Injection");
			}
		}

		ImGui::TreePop();
	}


	if ( ImGui::Button("Next Step" ) )
	{
		if ( protocol_ptr != nullptr )
		{
			protocol_ptr->AddEvent("Next Step");
		}
	}



	ImGui::End();
}

void CPUViewer::SetInputBuffer(std::shared_ptr<Common::Structure::InputBuffer> &ptr_set_input_buffer)
{
	mPtrInputBuffer = ptr_set_input_buffer;
}

