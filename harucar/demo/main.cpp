#include "imgui.h"
#include "imgui-SFML.h"

#include "cpu/cpu_viewer.h"
#include "cpu/cpu_provider.h"

#include "common/common_logger.h"
#include "common/common_element.h"

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>


using namespace HaruCar;
using namespace HaruCar::CPU;


static void add_instructions( std::shared_ptr<CPUProvider> & provider_ptr )
{
	for( size_t i = 0; i < 60; i++ )
	{
		provider_ptr->AddInstruction( "NOP", 0x0 );
	}

	provider_ptr->AddInstruction("ADD", 0x24 );
	provider_ptr->AddInstruction( "MOV", 0x34 );
}

std::shared_ptr<CPUProvider> make_provider( )
{
	std::shared_ptr<CPUProvider> provider_ptr = std::make_shared<CPUProvider>();

	// Flags..s
	provider_ptr->AddFlag("Z", false);
	provider_ptr->AddFlag("C", false);
	provider_ptr->AddFlag("F", false);

	add_instructions( provider_ptr );

	// Registers..
	ProviderRegister register_AB;
	register_AB.register_value = 0x4250;
	register_AB.UseHiLo("A", "B", 2);

	ProviderRegister register_B;
	register_B.register_value = 0x4;

	provider_ptr->AddRegister("AB", register_AB);
	provider_ptr->AddRegister( "C", register_B );

	return provider_ptr;
}

void render_cpu( CPUViewer & viewer, std::shared_ptr<CPUProvider> & provider_ptr, std::shared_ptr<UI::Structure::UIEventProtocol> & protocol_ptr)
{
	viewer.Render(std::static_pointer_cast<Base::Interface::Provider>(provider_ptr), protocol_ptr);

	if( UI::Structure::UIEventHelperFunction::FireEvent( *protocol_ptr, "Injection" ) )
	{
		provider_ptr->AddInstruction( "NOPE", 0x00 );
	}

}


int main()
{
	std::shared_ptr<CPUProvider> provider_ptr = make_provider();
	std::shared_ptr<UI::Structure::UIEventProtocol> protocol_ptr = std::make_shared<UI::Structure::UIEventProtocol>();
	std::shared_ptr<Common::Structure::InputBuffer> input_buffer_ptr = std::make_shared<Common::Structure::InputBuffer>( 500 );

	CPUViewer viewer;
	viewer.SetInputBuffer( input_buffer_ptr );

	// 로거 데이터 세팅
	HaruCar::Common::Log::Logger logger;
	LOG(logger) << "This, Is. Log.";
	LOG(logger) << "This, Is. Log!" << " Yeah!";


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

		// Provi
		render_cpu( viewer, provider_ptr, protocol_ptr );

		// 로거
		ImGui::Begin("Logger");

		HaruCar::Common::Elements::RenderLog( logger.GetDatas() );

		ImGui::End();

		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}