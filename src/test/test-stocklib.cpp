#include <string.h>
#include <thread>

#include "test-stocklib.h"
#include <stocklib/stocklib_p.h>

StockLibTestFixture::StockLibTestFixture()
{

}

StockLibTestFixture::~StockLibTestFixture()
{

}

void StockLibTestFixture::setUp()
{
    stocklib_init();
}

void StockLibTestFixture::tearDown()
{
    stocklib_p_reset();
}

/**
 * Tests a basic problem solve.  
 */
void StockLibTestFixture::testInitialize()
{
    CPPUNIT_ASSERT(stocklib_p_open_handles()==0);
    CPPUNIT_ASSERT_THROW( stocklib_init(), std::logic_error );
}

void StockLibTestFixture::testReset()
{
    stocklib_p_reset();
    stocklib_init();
}

void StockLibTestFixture::testUnknownSymbolResponse()
{
    char buffer[32];
    stocklib_p_test_mode(true);
    stocklib_p_test_behavior( SLTBGibberishRequest );
    sl_result_t r = stocklib_fetch_synch("ANYTHING",buffer);
    CPPUNIT_ASSERT(r==SL_FAIL);
}

void StockLibTestFixture::testUnknownSymbolResponseAsync()
{
    char buffer[32];
    stocklib_p_test_mode(true);
    stocklib_p_test_behavior( SLTBGibberishRequest );

    SLHANDLE h  = stocklib_fetch_asynch("ANYTHING",buffer);
    sl_result_t r = stocklib_asynch_wait(h);
    stocklib_asynch_dispose(h);

    CPPUNIT_ASSERT( r==SL_FAIL );
    CPPUNIT_ASSERT( 0 == stocklib_p_open_handles() );
}

void StockLibTestFixture::testKnownSymbolResponse()
{
    char buffer[32];
    stocklib_p_test_mode(true);
    stocklib_p_test_behavior( SLTBNormalRequest );
    sl_result_t r = stocklib_fetch_synch("ANYTHING",buffer);
    CPPUNIT_ASSERT( strcmp(buffer,"99.99")==0 );
    CPPUNIT_ASSERT(r==SL_OK);
}

void StockLibTestFixture::testKnownSymbolResponseAsync()
{
    char buffer[32];
    stocklib_p_test_mode(true);
    stocklib_p_test_behavior( SLTBNormalRequest );

    SLHANDLE h  = stocklib_fetch_asynch("ANYTHING",buffer);
    sl_result_t r = stocklib_asynch_wait(h);
    stocklib_asynch_dispose(h);

    CPPUNIT_ASSERT( r==SL_OK );
    CPPUNIT_ASSERT( strcmp(buffer,"99.99")==0 );
    CPPUNIT_ASSERT( 0 == stocklib_p_open_handles() );
}

void StockLibTestFixture::testKnownSymbolResponseAsyncSimultaneous()
{
    char buffer1[32];
    char buffer2[32];

    stocklib_p_test_mode(true);
    stocklib_p_test_behavior( SLTBNormalRequest );

    SLHANDLE h1  = stocklib_fetch_asynch("ANYTHING",buffer1);
    
    std::this_thread::sleep_for( std::chrono::milliseconds(100) );

    SLHANDLE h2  = stocklib_fetch_asynch("NOTHING",buffer2);

    sl_result_t r1 = stocklib_asynch_wait(h1);
    stocklib_asynch_dispose(h1);

    sl_result_t r2 = stocklib_asynch_wait(h2);
    stocklib_asynch_dispose(h2);

    CPPUNIT_ASSERT( (r1==SL_OK) && (r2==SL_OK) );
    CPPUNIT_ASSERT( strcmp(buffer1,"99.99")==0 );
    CPPUNIT_ASSERT( strcmp(buffer2,"99.99")==0 );
    CPPUNIT_ASSERT( 0 == stocklib_p_open_handles() );
}

void StockLibTestFixture::testKnownSymbolResponseAsyncWithCallback()
{
    bool calledBack = false;
    char buffer[32];
    stocklib_p_test_mode(true);
    stocklib_p_test_behavior( SLTBNormalRequest );

    SLHANDLE h  = stocklib_fetch_asynch("ANYTHING",buffer);

    CPPUNIT_ASSERT( SL_OK == stocklib_asynch_register_callback( h,
								[](SLHANDLE h, void* pData)
								{
								    bool* pCalled = (bool*)pData;
								    *pCalled = true;
								},
								&calledBack
								) );

    sl_result_t r = stocklib_asynch_wait(h);
    stocklib_asynch_dispose(h);

    CPPUNIT_ASSERT( r==SL_OK );
    CPPUNIT_ASSERT( strcmp(buffer,"99.99")==0 );
    CPPUNIT_ASSERT( 0 == stocklib_p_open_handles() );
    CPPUNIT_ASSERT( calledBack );
}

void StockLibTestFixture::testNameCacheNormalNameLookup()
{
    char buffer[32];
    stocklib_p_test_mode(true);
    stocklib_p_test_behavior( SLTBNormalRequest );
    sl_result_t r = stocklib_fetch_synch("ANYTHING",buffer);

    CPPUNIT_ASSERT( SL_OK == r );

    const char* pName  = stocklib_p_namecache_resolve("ANYTHING");
    CPPUNIT_ASSERT( pName!=NULL );
    CPPUNIT_ASSERT( 0==strcmp(pName,stocklib_ticker_to_name("ANYTHING") ) );
}

void StockLibTestFixture::testNameCacheFailedNameLookup()
{
    char buffer[32];
    stocklib_p_test_mode(true);
    stocklib_p_test_behavior( SLTBGibberishRequest );
    sl_result_t r = stocklib_fetch_synch("ANYTHING",buffer);

    CPPUNIT_ASSERT( SL_FAIL == r );
    CPPUNIT_ASSERT( 0 == stocklib_p_namecache_count() );

    const char* pResult = stocklib_ticker_to_name("ANYTHING");
    CPPUNIT_ASSERT( pResult == NULL );
}

void StockLibTestFixture::testNameCacheClearOnReset()
{
    /* Check clear on init */
    CPPUNIT_ASSERT(0==stocklib_p_namecache_count());

    /* Add an entry, check not empty, reset */
    stocklib_p_namecache_insert("TEST","Test Company Inc.");
    CPPUNIT_ASSERT(1==stocklib_p_namecache_count());
    stocklib_p_reset();

    /* Check cache is clear again */
    CPPUNIT_ASSERT(0==stocklib_p_namecache_count());    
}

void StockLibTestFixture::testNameCacheIndirectCache()
{
    char buffer[32];
    stocklib_p_test_mode(true);
    stocklib_p_test_behavior( SLTBNormalRequest );

    CPPUNIT_ASSERT( 0==strcmp("Test Inc.",stocklib_ticker_to_name("ANYTHING") ) );
}
