//
// Created by HaruGakkaP on 2020-05-29.
//

#include "Window.h"

using namespace Angelica;


bool Window::create( const std::string & window_name,  int width, int height )
{
	glfwInit();

	// OpenGL Version 4.2
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// Use Core.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// No Resizable.
	glfwWindowHint(GLFW_RESIZABLE, false);

	// SAMPLES 4.
	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(width, height, window_name.c_str(), nullptr, nullptr);

	if ( window == nullptr )
	{
		return false;
	}

	glfwMakeContextCurrent(window);
	return true;
}

bool Window::windowShouldClose()
{
	if(!window) { return true; }

	return glfwWindowShouldClose( window );
}

bool Window::pollEvents()
{
	if(!window) { return false; }

	glfwPollEvents();

	return true;
}

bool Window::swapBuffers()
{
	glfwSwapBuffers( window );
	return true;
}
