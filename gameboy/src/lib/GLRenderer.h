//
// Created by HaruGakkaP on 2020-05-29.
//

#ifndef GAMEBOY_GLRENDERER_H
#define GAMEBOY_GLRENDERER_H


#include <gl/gl3w.h>
#include <GLFW/glfw3.h>
#include <string>


namespace Angelica
{
	class GLRenderer
	{
	public:
		void init();

		GLuint createVertexShader( const std::string & filename );
		GLuint createFragmentShader( const std::string & filename );
		GLuint createProgram(GLuint vertex_shader, GLuint fragment_shader);

	private:
		static std::string getShaderSourceCode( const std::string & filename );

	private:
		GLuint default_program;
	};

}


#endif //GAMEBOY_GLRENDERER_H
