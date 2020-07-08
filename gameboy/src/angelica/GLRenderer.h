//
// Created by HaruGakkaP on 2020-05-29.
//

#ifndef GAMEBOY_GLRENDERER_H
#define GAMEBOY_GLRENDERER_H


#include "GLProgram.h"


namespace Angelica
{
	class GLRenderer
	{
	public:
		void init();

		GLuint createVertexShader( const std::string & filename );
		GLuint createFragmentShader( const std::string & filename );
		GLProgram createProgram(GLuint vertex_shader, GLuint fragment_shader);

		GLProgram getDefaultProgram() { return default_program; }

	private:
		static std::string getShaderSourceCode( const std::string & filename );

	private:

		GLuint vertex_arrays;
		GLProgram default_program;
	};

}


#endif //GAMEBOY_GLRENDERER_H
