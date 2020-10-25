//
// Created by HaruGakkaP on 2020-10-12.
//

#include "cpu/cpu_viewer.h"
#include "cpu/cpu_provider.h"
#include "cpu/cpu_element.h"
#include "common/common_element.h"
#include "imgui.h"


void CPUViewer::Render(std::shared_ptr<IProvider> provider_ptr, std::shared_ptr<UIEventProtocol> protocol_ptr)
{
	if ( provider_ptr == nullptr ) { return; }

	std::shared_ptr<CPUProvider> cpu_provider_ptr = std::static_pointer_cast<CPUProvider>( provider_ptr );

	ImGui::Begin("CPU Viewer");

	if( ImGui::TreeNode("Flags") )
	{
		Elements::CPU::RenderFlags( cpu_provider_ptr->GetFlagNames(), cpu_provider_ptr->GetFlags() );

		ImGui::TreePop();
	}

	if ( ImGui::TreeNode("Registers") )
	{
		Elements::CPU::RenderRegister( cpu_provider_ptr->GetRegisterNames(), cpu_provider_ptr->GetRegisterValues() );

		ImGui::TreePop();
	}

	if ( ImGui::TreeNode("Instructions"))
	{
		Elements::CPU::RenderInstructions( cpu_provider_ptr->GetInstructions(), cpu_provider_ptr->GetOpCodes() );

		ImGui::TreePop();
	}

	if ( mPtrInputBuffer != nullptr && ImGui::TreeNode("Input") )
	{
		Elements::Common::RenderInputTextBox( *mPtrInputBuffer );

		ImGui::TreePop();
	}

	if( ImGui::Button("Injection") )
	{
		if ( protocol_ptr != nullptr )
		{
			protocol_ptr->AddEvent("Injection");
		}
	}


	ImGui::End();
}

void CPUViewer::SetInputBuffer(std::shared_ptr<Structure::InputBuffer> &ptr_set_input_buffer)
{
	mPtrInputBuffer = ptr_set_input_buffer;
}

