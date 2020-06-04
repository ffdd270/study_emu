//
// Created by nhy20 on 2020-06-03.
//

#ifndef GAMEBOY_GLPROGRAM_H
#define GAMEBOY_GLPROGRAM_H

#include <unordered_map>
#include <string>
#include <gl/gl3w.h>
#include <GLFW/glfw3.h>

namespace Angelica
{
	class GLProgram
	{
	public:
		GLProgram();

		bool init(GLuint program);
		bool isVoid() const;

		bool addAttrib(const std::string & attrib_name);
		bool addUniform(const std::string & attrib_name);

		GLuint getLocation(const std::string & attrib_key);
		GLuint getProgram()  const { return m_program; }


	private:
		std::unordered_map<std::string, GLuint> m_location_map;
		GLuint m_program = 0;
	};
}



#endif //GAMEBOY_GLPROGRAM_H
