#include <iostream>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "test-buffer.h"
#include "test-problem.h"
#include "test-state.h"
#include "test-task.h"
#include "test-stocklib.h"
#include "test-black-scholes.h"

CPPUNIT_TEST_SUITE_REGISTRATION(BufferTestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(ProblemTestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(StateTestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(TaskTestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(StockLibTestFixture);
CPPUNIT_TEST_SUITE_REGISTRATION(BlackScholesTestFixture);

int main(int argc, char* argv[] )
{
    CppUnit::TextUi::TestRunner runner;
    CppUnit::Test* test = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    runner.addTest(test);
    bool wasSuccessful = runner.run();
    return (wasSuccessful)?0:1;

}
