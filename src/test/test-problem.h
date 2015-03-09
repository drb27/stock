#ifndef TEST_PROBLEM_H
#define TEST_PROBLEM_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCase.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

class ProblemTestFixture : public CppUnit::TestFixture
{
public:
    ProblemTestFixture();
    virtual ~ProblemTestFixture();

    void setUp();
    void tearDown();

    /** @name Test Cases */
    // @{
    void testSimpleProblem();
    void testFunctor();
    void testContainedProblem();
    // @}

    /** \cond internal */
    CPPUNIT_TEST_SUITE( ProblemTestFixture );
    CPPUNIT_TEST( testSimpleProblem );
    CPPUNIT_TEST( testFunctor );
    CPPUNIT_TEST( testContainedProblem );
    CPPUNIT_TEST_SUITE_END();
    /** \endcond */
};

#endif
