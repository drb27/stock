#ifndef TEST_BUFFER_H
#define TEST_BUFFER_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

class BufferTestFixture : public CppUnit::TestFixture
{
public:
    BufferTestFixture();
    virtual ~BufferTestFixture();

    void setUp();
    void tearDown();

    /** @name Test Cases */
    // @{
    void testConstruction();
    void testContents();
    void testReset();
    void testMove();
    void testOverflow();
    // @}

    /** \cond internal */
    CPPUNIT_TEST_SUITE( BufferTestFixture );
    CPPUNIT_TEST( testConstruction );
    CPPUNIT_TEST( testContents );
    CPPUNIT_TEST( testReset );
    CPPUNIT_TEST( testMove );
    CPPUNIT_TEST( testOverflow );
    CPPUNIT_TEST_SUITE_END();
    /** \endcond */
};

#endif
