//
// Created by nhy20 on 2020-06-03.
//

#include "GLProgram.h"

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
	if(m_location_map.find(attrib_name ) != m_location_map.end() )
	{
		return false;
	}

	GLuint attrib_location = glGetAttribLocation( m_program, attrib_name.c_str() );
	m_location_map.insert(std::make_pair(attrib_name, attrib_location ) );
	return true;
}


bool Angelica::GLProgram::addUniform(const std::string &attrib_name)
{
	if(m_location_map.find(attrib_name ) != m_location_map.end() )
	{
		return false;
	}

	GLuint attrib_location = glGetAttribLocation( m_program, attrib_name.c_str() );
	m_location_map.insert(std::make_pair(attrib_name, attrib_location ) );
	return true;
}


GLuint Angelica::GLProgram::getLocation(const std::string &attrib_key)
{
	auto location_map_iter = m_location_map.find( attrib_key );

	if ( location_map_iter == m_location_map.end() )
	{

		return 0;
	}

	return location_map_iter->second;
}
