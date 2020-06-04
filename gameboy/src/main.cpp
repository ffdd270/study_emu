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
					{-1.0f, 1.0f,  0.0f, 1.0f },
			};

	Angelica::GLProgram program = renderer.getDefaultProgram();
	program.addAttrib( "position");
	program.addUniform("proj_ortho");

	static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	while( !window.windowShouldClose() )
	{
		window.pollEvents();

		glClearBufferfv(GL_COLOR, 0, black);

		glUseProgram( program.getProgram() );
		glPointSize( 10.0f );
		glUniformMatrix4fv( program.getLocation("proj_ortho"), 1, GL_FALSE, &ortho_projection[0][0] );
		glVertexAttrib2f(program.getLocation("position"), 2, 2 );
		glDrawArrays(GL_POINTS, 0, 1);


		window.swapBuffers();
	}

}