#ifndef TEST_STOCKLIB_H
#define TEST_STOCKLIB_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

class StockLibTestFixture : public CppUnit::TestFixture
{
public:
    StockLibTestFixture();
    virtual ~StockLibTestFixture();

    void setUp();
    void tearDown();

    /** @name Test Cases */
    // @{
    void testInitialize();
    void testReset();
    void testUnknownSymbolResponse();
    void testUnknownSymbolResponseAsync();
    void testKnownSymbolResponse();
    void testKnownSymbolResponseAsync();
    void testKnownSymbolResponseAsyncSimultaneous();
    void testKnownSymbolResponseAsyncWithCallback();
    // @}

    /** \cond internal */
    CPPUNIT_TEST_SUITE( StockLibTestFixture );
    CPPUNIT_TEST( testInitialize );
    CPPUNIT_TEST( testReset );
    CPPUNIT_TEST( testUnknownSymbolResponse );
    CPPUNIT_TEST( testUnknownSymbolResponseAsync );
    CPPUNIT_TEST( testKnownSymbolResponse );
    CPPUNIT_TEST( testKnownSymbolResponseAsync );
    CPPUNIT_TEST( testKnownSymbolResponseAsyncSimultaneous );
    CPPUNIT_TEST( testKnownSymbolResponseAsyncWithCallback );
    CPPUNIT_TEST_SUITE_END();
    /** \endcond */
};

#endif
