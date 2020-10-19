//
// Created by nhy20 on 2020-10-12.
//

#ifndef HARUCAR_CPU_ELEMENT_H
#define HARUCAR_CPU_ELEMENT_H

#include <vector>
#include <string>

struct ProviderRegister;

namespace Elements
{
	namespace CPU
	{
		void RenderFlags(const std::vector<std::string> & flag_names, const std::vector<bool> & flags);
		void RenderRegisterHiLow( const ProviderRegister & ref_provider_register );
		void RenderRegister(const std::vector<std::string> & register_names, const std::vector<ProviderRegister> & values);
		void RenderInstructions(const std::vector<std::string> & instructions, const std::vector<int> & opcodes, float width = 200.f, float height = 200.f, int id = 0 );
	}
}


#endif //HARUCAR_CPU_ELEMENT_H
