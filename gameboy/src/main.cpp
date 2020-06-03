#include "lib/GLRenderer.h"
#include "lib/Window.h"


int main()
{
	Angelica::Window window;
	Angelica::GLRenderer renderer;

	window.create( "I Will Keep Comming", 640, 320 );
	renderer.init();


	const float ortho_projection[4][4] =
			{
					{ 2.0f/ 640, 0.0f, 0.0f, 0.0f },
					{ 0.0f, 2.0f / -320, 0.0f, 0.0f },
					{ 0.0f, 0.0f, -1.0f, 0.0f },
					{-1.0f,                  1.0f,                   0.0f, 1.0f },
			};


	while( !window.windowShouldClose() )
	{
		window.pollEvents();

		GLuint program = renderer.getDefaultProgram();
		glUseProgram( program );
		glPointSize( 10.0f );
		glUniformMatrix4fv( ortho_projection, 1, GL_FALSE, &ortho_projection[0][0] );
		glVertexAttrib2f()
	}

}