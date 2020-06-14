#include "lib/GLRenderer.h"
#include "lib/Window.h"
#include "lib/typedef.h"
#include "gameboy/GameboyCPU.h"
#include <iostream>


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
					{-1.0f, 1.0f,  0.0f, 1.0f },
			};

	Angelica::Matrix4x4 mat( ortho_projection );
	Angelica::GLProgram program = renderer.getDefaultProgram();
	program.addAttrib( "position");
	program.addUniform("proj_ortho");

	static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	while( !window.windowShouldClose() )
	{
		window.pollEvents();

		glClearBufferfv(GL_COLOR, 0, black);

		program.useProgram();
		glPointSize( 10.0f );

		program.setUniformMatrix4x4( "proj_ortho", mat );
		program.setVertexAttrib2f( "position", {  320, 160  } );
		glDrawArrays( GL_POINTS, 0, 1 );

		window.swapBuffers();
	}

}