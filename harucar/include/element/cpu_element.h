//
// Created by nhy20 on 2020-10-12.
//

#ifndef HARUCAR_CPU_ELEMENT_H
#define HARUCAR_CPU_ELEMENT_H

#include <vector>
#include <string>

namespace Elements
{
	namespace CPU
	{
		void RenderFlags(const std::vector<std::string> & flag_names, const std::vector<bool> & flags);
		void RenderRegister(const std::vector<std::string> & register_names, const std::vector<int> & values);
	}
}


#endif //HARUCAR_CPU_ELEMENT_H
