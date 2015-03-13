#ifndef TEST_BLACK_SCHOLES_H
#define TEST_BLACK_SCHOLES_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

class BlackScholesTestFixture : public CppUnit::TestFixture
{
public:
    BlackScholesTestFixture();
    virtual ~BlackScholesTestFixture();

    void setUp();
    void tearDown();

    /** @name Test Cases */
    // @{
    void testInitialize();
    void testCallPriceITMAtExpiry();
    void testCallPriceOTMAtExpiry();
    void testCallPriceATMAtExpiry();

    void testCallPriceITMAtOneMonth();
    void testCallPriceOTMAtOneMonth();
    void testCallPriceATMAtOneMonth();
    // @}

    /** \cond internal */
    CPPUNIT_TEST_SUITE( BlackScholesTestFixture );
    CPPUNIT_TEST( testInitialize );

    CPPUNIT_TEST( testCallPriceITMAtExpiry );
    CPPUNIT_TEST( testCallPriceOTMAtExpiry );
    CPPUNIT_TEST( testCallPriceATMAtExpiry );

    CPPUNIT_TEST( testCallPriceITMAtOneMonth );
    CPPUNIT_TEST( testCallPriceOTMAtOneMonth );
    CPPUNIT_TEST( testCallPriceATMAtOneMonth );
    CPPUNIT_TEST_SUITE_END();
    /** \endcond */
};

#endif
