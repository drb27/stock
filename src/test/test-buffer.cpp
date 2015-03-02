#include "test-buffer.h"
#include <stocklib/buffer.h>

BufferTestFixture::BufferTestFixture()
{
}

BufferTestFixture::~BufferTestFixture()
{

}

void BufferTestFixture::setUp()
{
}

void BufferTestFixture::tearDown()
{
}

/**
 * @brief Tests object creation 
 */
void BufferTestFixture::testConstruction()
{
    buffer b(1024);
    CPPUNIT_ASSERT(1024==b.remaining_bytes());
    char a='\0';
    CPPUNIT_ASSERT(b.append(&a,1));
    CPPUNIT_ASSERT(1023==b.remaining_bytes());
    CPPUNIT_ASSERT(!b.append(&a,1024));
}
