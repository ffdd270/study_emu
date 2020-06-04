//
// Created by HaruGakkaP on 2020-05-29.
//

#include "GLRenderer.h"

#include <fstream>
#include <cassert>

using namespace Angelica;

void GLRenderer::init()
{
	gl3wInit();

	default_program = createProgram( createVertexShader( "shader/basic_vert.glsl" ), createFragmentShader("shader/basic_frag.glsl") );

	assert(!default_program.isVoid());
}

// return 0 is failed.
GLuint GLRenderer::createVertexShader(const std::string & filename)
{
	std::string source_code = getShaderSourceCode(filename);

	if( source_code.empty() )
	{
		return 0;
	}

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);

	const char * source_code_c_str = source_code.c_str();

	glShaderSource(vs, 1, &source_code_c_str, nullptr );
	glCompileShader(vs);

	GLint ok;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);

	if( !ok )
	{
		return 0;
	}

	return vs;
}

GLuint GLRenderer::createFragmentShader(const std::string & filename )
{
	std::string source_code = getShaderSourceCode(filename);

	if( source_code.empty() )
	{
		return 0;
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	const char * source_code_c_str = source_code.c_str();

	glShaderSource(fs, 1, &source_code_c_str, nullptr );
	glCompileShader(fs);

	GLint ok;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);

	if( !ok )
	{
		return 0;
	}

	return fs;
}

GLProgram GLRenderer::createProgram(GLuint vertex_shader, GLuint fragment_shader)
{
	GLProgram glProgram;

	if( vertex_shader == 0 || fragment_shader == 0 ) { return glProgram; }

	GLuint program = glCreateProgram();

	glAttachShader( program, vertex_shader );
	glAttachShader( program, fragment_shader );

	glLinkProgram( program );

	GLint ok;
	glGetProgramiv( program, GL_LINK_STATUS, &ok );

	if(!ok)
	{
		return glProgram;
	}

	glGenVertexArrays(1, &vertex_arrays);
	glBindVertexArray(vertex_arrays);

	glProgram.init( program );

	return glProgram;
}

std::string GLRenderer::getShaderSourceCode(const std::string &filename)
{
	std::fstream fs;

	fs.open( filename, std::fstream::in );

	if( !fs.is_open() )
	{
		return "";
	}

	std::string shader_code;
	std::string line;


	while( std::getline( fs, line ) )
	{
		shader_code += line + "\n";
	}

	fs.close();

	return shader_code;
}
