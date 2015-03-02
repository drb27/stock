/**
 * @file
 */

/*
The MIT License (MIT)

Copyright (c) 2015 David Bradshaw

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
