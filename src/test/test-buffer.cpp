#include <string.h>
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
 * Tests object creation 
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

/**
 * Tests 
 */
void BufferTestFixture::testContents()
{
    buffer b(1024);
    const char* s = "Hello";

    b.append(s,1+strlen(s));
    CPPUNIT_ASSERT( b.remaining_bytes() == (1024-(1+strlen(s))));
    CPPUNIT_ASSERT( strcmp(s,b.contents())==0 );
    
}

/**
 * Tests 
 */
void BufferTestFixture::testReset()
{
    buffer b(1024);
    char ib[10];
    b.append(ib,10);
    CPPUNIT_ASSERT( b.remaining_bytes() == 1014 );
    b.reset();
    CPPUNIT_ASSERT( b.remaining_bytes() == 1024 );
}

/**
 * Tests 
 */
void BufferTestFixture::testMove()
{
    buffer b(1024);
    b.append("Hello",6);

    const char* originalLocation = b.contents();

    buffer c = std::move(b);

    CPPUNIT_ASSERT(b.contents()==nullptr);
    CPPUNIT_ASSERT(c.contents()==originalLocation);
    CPPUNIT_ASSERT(c.remaining_bytes()==1018);
}

/**
 * Tests
 */
void BufferTestFixture::testOverflow()
{
    buffer b(1024);
    char ib[1025];
    CPPUNIT_ASSERT( b.append(ib,1024) );
    CPPUNIT_ASSERT( b.remaining_bytes() == 0 );
    CPPUNIT_ASSERT( !b.append(ib,1) );
    CPPUNIT_ASSERT( b.remaining_bytes() == 0 );

    buffer c(1024);
    CPPUNIT_ASSERT( !c.append(ib,1025) );
    CPPUNIT_ASSERT( c.remaining_bytes() == 1024 );
}
