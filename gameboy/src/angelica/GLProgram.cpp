//
// Created by nhy20 on 2020-06-03.
//

#include "GLProgram.h"


Angelica::GLProgram::GLProgram()
{

}

bool Angelica::GLProgram::init(GLuint program)
{
	if ( program == 0 )
	{
		// TODO :  여기 에러 로그 박기
		return false;
	}

	this->m_program = program;
	return true;
}

bool Angelica::GLProgram::addAttrib(const std::string &attrib_name)
{
	if(m_location_map.find( attrib_name ) != m_location_map.end() )
	{
		return false;
	}

	GLuint attrib_location = glGetAttribLocation( m_program, attrib_name.c_str() );

	if ( attrib_location == static_cast<GLuint>(-1) ) { return false; }

	m_location_map.insert(std::make_pair(attrib_name, attrib_location ) );
	return true;
}


bool Angelica::GLProgram::addUniform(const std::string &attrib_name)
{
	if(m_location_map.find( attrib_name ) != m_location_map.end() )
	{
		return false;
	}

	GLuint attrib_location = glGetUniformLocation( m_program, attrib_name.c_str() );

	if ( attrib_location == static_cast<GLuint>(-1) ) { return false; }

	m_location_map.insert(std::make_pair(attrib_name, attrib_location ) );
	return true;
}


GLuint Angelica::GLProgram::getLocation(const std::string &attrib_key)
{
	auto location_map_iter = m_location_map.find( attrib_key );

	if ( location_map_iter == m_location_map.end() )
	{
		return -1;
	}

	return location_map_iter->second;
}

bool Angelica::GLProgram::isVoid() const
{
	return m_program == 0;
}


void Angelica::GLProgram::setUniformMatrix4x4(const std::string & uniform_name, const Angelica::Matrix4x4 &mat)
{
	GLuint matrix_location = getLocation( uniform_name );
	if ( matrix_location == GL_FAILED_UINT )
	{
		//TODO : LOG Here
		return;
	}

	glUniformMatrix4fv( matrix_location, 1, GL_FALSE, &mat.mat[0][0] );
}

void Angelica::GLProgram::setVertexAttrib2f(const std::string & attrib_name, const Angelica::Vec2 &vec)
{
	GLuint attrib_location = getLocation( attrib_name );
	if ( attrib_location == GL_FAILED_UINT )
	{
		//TODO : LOG Here
		return;

	}
	glVertexAttrib2f( attrib_location, vec.x, vec.y );
}

void Angelica::GLProgram::useProgram()
{
	glUseProgram( m_program );
}
