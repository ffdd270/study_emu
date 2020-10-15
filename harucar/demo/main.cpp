#include "imgui.h"
#include "imgui-SFML.h"

#include "CPUViewer.h"
#include "CPUProvider.h"

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>



int main()
{
	std::shared_ptr<CPUProvider> provider_ptr = std::make_shared<CPUProvider>();

	// Flags..s
	provider_ptr->AddFlag("Z", false);
	provider_ptr->AddFlag("C", false);
	provider_ptr->AddFlag("F", false);

	// Registers..
	ProviderRegister register_AB;
	register_AB.register_value = 0x4250;
	register_AB.UseHiLo("A", "B", 2);

	ProviderRegister register_B;
	register_B.register_value = 0x4;

	provider_ptr->AddRegister("AB", register_AB);
	provider_ptr->AddRegister( "C", register_B );


	CPUViewer viewer;

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

		viewer.render( std::static_pointer_cast<IProvider>( provider_ptr ) );

		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}