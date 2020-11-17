#include <iostream>
#include <fstream>

#include "imgui.h"
#include "imgui-SFML.h"

#include "common/common_logger.h"
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "util/util_string.h"
#include "gameboy/GameboyCPU.h"
#include "gameboy/GameboyCPUBroker.h"

#include "cpu/cpu_viewer.h"
#include "harucar/LuaCommandViewer.h"

#include "lua-binding/LuaContext.h"
#include "lua-binding/gameboy_luabinding.h"
#include "lua-binding/LuaImGuiHandler.h"

#include "color_text_edit/TextEditor.h"

using namespace HaruCar::UI;
using namespace HaruCar::CPU;
using namespace HaruCar::UI::Structure;
using namespace HaruCar::Common::Structure;

using Logger = HaruCar::Common::Log::Logger;

void loadLuaFiles( LuaContext & ref_context )
{
	std::string base_path = "script/";
	std::fstream file( "script/init.txt" );
	std::string path;

	assert( file.is_open() );

	while( !file.eof() )
	{
		file >> path;

		bool flag = ref_context.ExecuteFile( base_path + path );

		if( !flag )
		{
			std::cout << ref_context.GetLastError() << std::endl;
		}
	}

	bool ok = ref_context.ExecuteFunction( "init_script" );

	if( !ok )
	{
		std::cout << ref_context.GetLastError() << std::endl;
		assert(false);
	}

}

void InputEvents( std::shared_ptr<GameboyCPU> & ref_ptr_cpu,
				  GameboyCPUBroker & broker,
				  LuaContext & context,
				  TextEditor & editor,
			  std::shared_ptr<InputBuffer> & input_buffer_ptr,
			  std::shared_ptr<CPUProvider> & provider_ptr,
			  std::shared_ptr<UIEventProtocol> & protocol_ptr,
			  std::shared_ptr<Logger> & logger_ptr,
			  std::shared_ptr<LuaImGuiHandler> & handler_ptr )
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
				ref_ptr_cpu->InjectionMemory(value );
			}
		}
		catch(std::invalid_argument & invalid_argument)
		{
			std::cout << "WRONG INPUT" << std::endl;
		}

		broker.UpdateProvider(*ref_ptr_cpu, provider_ptr );
	}

	if ( UIEventHelperFunction::FireEvent( *protocol_ptr, "Next Step") )
	{
		ref_ptr_cpu->NextStep();

		broker.UpdateProvider(*ref_ptr_cpu, provider_ptr );
	}

	if ( UIEventHelperFunction::FireEvent( *protocol_ptr, "Lua:Reload" ) )
	{
		context.Reload();
		loadLuaFiles( context );

		if(!context.ExecuteString( editor.GetText() ))
		{
			std::string last_error = context.GetLastError();
			logger_ptr->LogError( last_error );
			std::cout << last_error << std::endl;
		}

		handler_ptr->CleanUpOnLuaReload();
	}

	if ( UIEventHelperFunction::FireEvent( *protocol_ptr, "Lua:Execute" ) )
	{
		if(!context.ExecuteString( editor.GetText() ))
		{
			std::string last_error = context.GetLastError();
			logger_ptr->LogError( last_error );
			std::cout << last_error << std::endl;
		}
	}
}


int main()
{
	std::shared_ptr<GameboyCPU> cpu_ptr = std::make_shared<GameboyCPU>();
	GameboyCPUBroker broker;
	TextEditor editor;
	LuaCommandViewer command_viewer;

	editor.SetLanguageDefinition( TextEditor::LanguageDefinition::Lua() );
	gameboy_lua_binding_cpu( cpu_ptr );

	std::shared_ptr<UIEventProtocol> protocol_ptr = std::make_shared<UIEventProtocol>();
	std::shared_ptr<CPUProvider> provider_ptr = broker.MakeProvider( *cpu_ptr );
	std::shared_ptr<InputBuffer> input_buffer_ptr = std::make_shared<InputBuffer>( 300 );
	std::shared_ptr<Logger> logger_ptr = std::make_shared<Logger>();
	gameboy_lua_binding_logger( logger_ptr );

	CPUViewer viewer;

	viewer.SetInputBuffer( input_buffer_ptr );

	cpu_ptr->InjectionMemory(0b00111110 ); // LD A, imm8
	cpu_ptr->InjectionMemory(0x0 );
	cpu_ptr->NextStep();

	cpu_ptr->InjectionMemory(0xD6 ); //SUB N
	cpu_ptr->InjectionMemory(1 );
	cpu_ptr->NextStep();

	broker.UpdateProvider(*cpu_ptr, provider_ptr );
	//Carry!

	std::shared_ptr<LuaContext> lua_context_ptr = std::make_shared<LuaContext>();
	std::shared_ptr<LuaImGuiHandler> handler_ptr = std::make_shared<LuaImGuiHandler>( lua_context_ptr );
	gameboy_lua_binding_imgui_handler( handler_ptr );

	loadLuaFiles( *lua_context_ptr );

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

		handler_ptr->Render( nullptr, nullptr );
		if ( handler_ptr->IsRenderFailed() )
		{
			auto reasons = handler_ptr->GetRenderFailedReasons();

			for ( const auto & reason : reasons )
			{
				logger_ptr->LogError( reason );
				std::cout << reason << std::endl;
			}
		}

		handler_ptr->CleanUp();

		InputEvents(cpu_ptr, broker, *lua_context_ptr, editor,
			  input_buffer_ptr, provider_ptr, protocol_ptr, logger_ptr, handler_ptr ) ;

		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}