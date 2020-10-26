//
// Created by nhy20 on 2020-10-24.
//

#include "common/common_structure.h"

using namespace HaruCar::Common::Structure;

InputBuffer::InputBuffer(size_t buffer_size) : buffer_size( buffer_size )
{
	ptr = new char[buffer_size];
	memset( ptr, 0, buffer_size );
}

std::string_view InputBuffer::GetViewString() const
{
	return ptr;
}

char * InputBuffer::GetBuffer()
{
	return ptr;
}

size_t InputBuffer::GetBufferSize() const
{
	return buffer_size;
}

InputBuffer::~InputBuffer()
{
	delete [] ptr;
}
