#include <stocklib/black-scholes.h>
#include "test-black-scholes.h"

BlackScholesTestFixture::BlackScholesTestFixture()
{

}

BlackScholesTestFixture::~BlackScholesTestFixture()
{

}

void BlackScholesTestFixture::setUp()
{
}

void BlackScholesTestFixture::tearDown()
{
}

/**
 * Tests a basic problem solve.  
 */
void BlackScholesTestFixture::testInitialize()
{
    auto fn = bs_bind_normal(0,1);
    auto y = fn(0);
    CPPUNIT_ASSERT( (y>0.398d) && (y<0.399d) );

    fn = bs_bind_normal(7,1);
    y = fn(7);
    CPPUNIT_ASSERT( (y>0.398d) && (y<0.399d) );

    fn = bs_bind_normal(4,3);
    y = fn(4);
    CPPUNIT_ASSERT( (y>0.132d) && (y<0.134d) );

    y=fn(5);
    CPPUNIT_ASSERT( (y>0.125d) && (y<0.127d) );

    y=bs_cndf(0,1,5);
    CPPUNIT_ASSERT( (y>0.998d) && (y<1.0d) );

    y=bs_cndf(0,1,0);
    CPPUNIT_ASSERT( (y>0.499d) && (y<0.501d) );

    y=bs_cndf(3,1,3);
    CPPUNIT_ASSERT( (y>0.499d) && (y<0.501d) );
}

void BlackScholesTestFixture::testCallPriceITMAtExpiry()
{
    double p = bs_call_price(52.0,45.0,0.002,0.03,0.2);
    CPPUNIT_ASSERT( (p>7.0) && (p<7.01) );
}

void BlackScholesTestFixture::testCallPriceOTMAtExpiry()
{
    double p = bs_call_price(52.0,55.0,0.002,0.03,0.2);
    CPPUNIT_ASSERT( (p>0.0) && (p<0.05) );
}

void BlackScholesTestFixture::testCallPriceATMAtExpiry()
{
    double p = bs_call_price(52.0,52.0,0.002,0.03,0.2);
    CPPUNIT_ASSERT( (p>0.0) && (p<0.20) );
}

void BlackScholesTestFixture::testCallPriceITMAtOneMonth()
{
    double p = bs_call_price(52.0,45.0,0.083,0.03,0.2);
    CPPUNIT_ASSERT( (p>7.0) && (p<7.2) );
}

void BlackScholesTestFixture::testCallPriceOTMAtOneMonth()
{
    double p = bs_call_price(52.0,55.0,0.083,0.03,0.2);
    CPPUNIT_ASSERT( (p>0.29) && (p<0.31) );
}

void BlackScholesTestFixture::testCallPriceATMAtOneMonth()
{
    double p = bs_call_price(52.0,52.0,0.083,0.03,0.2);
    CPPUNIT_ASSERT( (p>1.2) && (p<1.3) );
}
