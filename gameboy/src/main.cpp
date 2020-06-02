#include "lib/GLRenderer.h"
#include "lib/Window.h"


int main()
{
	Angelica::Window window;
	Angelica::GLRenderer renderer;

	window.create( "I Will Keep Comming", 640, 320 );
	renderer.init();


	while( !window.windowShouldClose() )
	{
		window.pollEvents();

	}

}