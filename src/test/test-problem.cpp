#include "test-problem.h"
#include <stocklib/problem.h>
#include <stocklib/urltask.h>

class fact : public contained_problem<int,int>
{
public:
    fact(int x) : contained_problem<int,int>(x) {}

protected:
    virtual int do_work(int x);
};

int fact::do_work(int x)
{
    if (x==1)
	return 1;
    else
	return x * do_work(x-1);
}

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
    problem<int,int> p( [](int x){ return x+x; }, 8 );
    CPPUNIT_ASSERT(16==p.solve());
}

/**
 * Tests a basic problem solve.  
 */
void ProblemTestFixture::testFunctor()
{
    /* Basic functionality */
    problem<int,int> p( [](int x){ return x+x; }, 8 );
    CPPUNIT_ASSERT(16==p());

}

void ProblemTestFixture::testContainedProblem()
{
    {
	fact f(4);
	CPPUNIT_ASSERT(24==f.solve());
    }

    urltask t("http://www.scatterpic.com/api/motd.php");
 
    WorkResult r = t.perform_sync();

    CPPUNIT_ASSERT( t.ready() );
    CPPUNIT_ASSERT( r == WorkResult::Success );
}


