#include "test-problem.h"
#include <stocklib/problem.h>

ProblemTestFixture::ProblemTestFixture()
{
}

ProblemTestFixture::~ProblemTestFixture()
{

}

void ProblemTestFixture::setUp()
{
}

void ProblemTestFixture::tearDown()
{
}

/**
 * Tests a basic problem solve.  
 */
void ProblemTestFixture::testSimpleProblem()
{
    auto p = problem<int,int>( [](int x){ return x+x; }, 8 );
    CPPUNIT_ASSERT(16==p.solve());
}

/**
 * Tests a basic problem solve.  
 */
void ProblemTestFixture::testFunctor()
{
    /* Basic functionality */
    auto p = problem<int,int>( [](int x){ return x+x; }, 8 );
    CPPUNIT_ASSERT(16==p());

    /* Can make a std::function */
    auto f = std::function<int(void)>(p);
    CPPUNIT_ASSERT(16==f());
}

/**
 * Tests a basic problem solve.  
 */
void ProblemTestFixture::testCopyConstructor()
{
    auto f = std::function<int(int)>([](int x){ return x+x; });
    auto p = problem<int,int>( f, 8 );

    auto q = p;
    f = [](int x){ return x+x+1; };
    auto r = problem<int,int>(f,8);

    /* Because the constructor takes a *copy* of the function, we
       expect both results to be 16, i.e. setting f has no effect */
    CPPUNIT_ASSERT(16==p.solve());
    CPPUNIT_ASSERT(16==q.solve());

    /* HOWEVER, r should evaluate using the new function */
    CPPUNIT_ASSERT(17==r.solve());
}

