#include <stdlib.h>
#include <memory.h>
#include "buffer.h"


buffer::buffer(unsigned long sz) : size(sz)
{
    _buffer = reinterpret_cast<char*>(malloc(sz));
    reset();
}

buffer::buffer( buffer&& o ) : size(o.size)
{
    _buffer = o._buffer;
    index = o.index;
    o._buffer = nullptr;
    o.index=0;
}

buffer::~buffer()
{
    if ( _buffer != nullptr )
    {
	free(_buffer);
	_buffer = nullptr;
    }
}

void buffer::reset(void)
{
    index=_buffer;
}

unsigned long buffer::remaining_bytes() const
{

    return (unsigned long)(size-(index-_buffer));
}

bool buffer::append(const void* pData, unsigned long sz)
{
    if (sz <= remaining_bytes() )
    {
	memcpy(index,pData,sz);
	index += sz;
    }
    else
	return false;

    return true;
}
 
const char* buffer::contents()
{
    return _buffer;
}
