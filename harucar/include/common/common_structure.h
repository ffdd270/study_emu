//
// Created by nhy20 on 2020-10-24.
//

#ifndef HARUCAR_COMMON_STRUCTURE_H
#define HARUCAR_COMMON_STRUCTURE_H

#include <string>
#include <string_view>

namespace HaruCar::Common::Structure
{
	class InputBuffer
	{
	public:
		explicit InputBuffer( size_t buffer_size );

		[[nodiscard]] std::string_view GetViewString() const;

		char * GetBuffer();
		[[nodiscard]] size_t GetBufferSize() const;

		~InputBuffer();
	private:
		char * ptr;
		size_t buffer_size;
	};
}


#endif //HARUCAR_COMMON_STRUCTURE_H
