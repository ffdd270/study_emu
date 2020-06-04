//
// Created by HaruGakkaP on 2020-05-29.
//

#ifndef GAMEBOY_WINDOW_H
#define GAMEBOY_WINDOW_H

#include <GLFW/glfw3.h>
#include <string>

namespace Angelica
{
	class Window
	{
	public:
		bool create(const std::string & window_name, int width, int height);


		bool windowShouldClose();
		bool pollEvents();
		bool swapBuffers();
	private:
		GLFWwindow * window = nullptr;
	};
}



#endif //GAMEBOY_WINDOW_H
