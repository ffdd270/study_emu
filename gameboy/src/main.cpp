#include <iostream>

#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "util/util_string.h"
#include "gameboy/GameboyCPU.h"
#include "gameboy/GameboyCPUBroker.h"
#include "cpu/cpu_viewer.h"
#include "harucar/LuaCommandViewer.h"

#include "lua-binding/LuaContext.h"
#include "color_text_edit/TextEditor.h"

using namespace HaruCar::UI;
using namespace HaruCar::CPU;
using namespace HaruCar::UI::Structure;
using namespace HaruCar::Common::Structure;

void InputEvents( GameboyCPU & cpu,
				  GameboyCPUBroker & broker,
				  LuaContext & context,
				  TextEditor & editor,
			  std::shared_ptr<InputBuffer> & input_buffer_ptr,
			  std::shared_ptr<CPUProvider> & provider_ptr,
			  std::shared_ptr<UIEventProtocol> & protocol_ptr )
{
	if ( UIEventHelperFunction::FireEvent( *protocol_ptr, "Injection" ) )
	{
		std::vector<std::string> parse_result = HaruCar::Util::string_split( input_buffer_ptr->GetViewString() );

		try
		{
			std::vector<BYTE> instructions;
			instructions.reserve( parse_result.size() );

			for(const auto & i : parse_result)
			{
				instructions.emplace_back( std::stoi( i, nullptr, 16 ) );
			}

			for(const BYTE & value : instructions )
			{
				cpu.InjectionMemory( value );
			}
		}
		catch(std::invalid_argument & invalid_argument)
		{
			std::cout << "WRONG INPUT" << std::endl;
		}

		broker.UpdateProvider( cpu, provider_ptr );
	}

	if ( UIEventHelperFunction::FireEvent( *protocol_ptr, "Next Step") )
	{
		cpu.NextStep();

		broker.UpdateProvider( cpu, provider_ptr );
	}

	if ( UIEventHelperFunction::FireEvent( *protocol_ptr, "Lua:Reload" ) )
	{
		context.Reload();
		if(!context.ExecuteString( editor.GetText() ))
		{
			std::cout << context.GetLastError() << std::endl;
			assert( false );
		}
	}

	if ( UIEventHelperFunction::FireEvent( *protocol_ptr, "Lua:Execute" ) )
	{
		if ( !context.ExecuteFunction("execute_func") )
		{
			std::cout << context.GetLastError() << std::endl;
			assert( false );
		}
	}
}

int main()
{
	GameboyCPU cpu;
	GameboyCPUBroker broker;
	TextEditor editor;
	LuaCommandViewer command_viewer;

	editor.SetLanguageDefinition( TextEditor::LanguageDefinition::Lua() );

	std::shared_ptr<UIEventProtocol> protocol_ptr = std::make_shared<UIEventProtocol>();
	std::shared_ptr<CPUProvider> provider_ptr = broker.MakeProvider( cpu );
	std::shared_ptr<InputBuffer> input_buffer_ptr = std::make_shared<InputBuffer>( 300 );

	CPUViewer viewer;

	viewer.SetInputBuffer( input_buffer_ptr );

	cpu.InjectionMemory( 0b00111110 ); // LD A, imm8
	cpu.InjectionMemory( 0x0 );
	cpu.NextStep();

	cpu.InjectionMemory( 0xD6 ); //SUB N
	cpu.InjectionMemory( 1 );
	cpu.NextStep();

	broker.UpdateProvider( cpu, provider_ptr );
	//Carry!

	LuaContext lua_context {};

	sf::RenderWindow window(sf::VideoMode(640, 480), "Gameboy");
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);

	sf::Clock deltaClock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		ImGui::Begin("Hello, world!");
		ImGui::Button("Look at this pretty button");
		ImGui::End();

		viewer.Render( provider_ptr, protocol_ptr );
		command_viewer.Render( nullptr, protocol_ptr );
		editor.Render( "Absoulte" );

		InputEvents( cpu, broker, lua_context, editor, input_buffer_ptr, provider_ptr, protocol_ptr ) ;


		if ( !lua_context.ExecuteFunction("lua_test") )
		{
			std::cout << "LAST ERROR" << lua_context.GetLastError() << std::endl;
			assert( false );
		}


		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}