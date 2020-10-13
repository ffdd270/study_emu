#include "imgui.h"
#include "imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "gameboy/GameboyCPU.h"
#include "gameboy/GameboyCPUBroker.h"
#include "CPUViewer.h"

int main()
{
	GameboyCPU cpu;
	GameboyCPUBroker broker;
	std::shared_ptr<CPUProvider> provider_ptr = broker.MakeProvider( cpu );
	CPUViewer viewer;

	cpu.InjectionMemory( 0b00111110 ); // LD A, imm8
	cpu.InjectionMemory( 0x0 );
	cpu.NextStep();

	cpu.InjectionMemory( 0xD6 ); //SUB N
	cpu.InjectionMemory( 1 );
	cpu.NextStep();

	broker.UpdateProvider( cpu, provider_ptr );
	//Carry!


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

		viewer.render( provider_ptr );

		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}