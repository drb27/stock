#include <iostream>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "test-buffer.h"

CPPUNIT_TEST_SUITE_REGISTRATION(BufferTestFixture);

int main(int argc, char* argv[] )
{
    CppUnit::TextUi::TestRunner runner;
    CppUnit::Test* test = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

    runner.addTest(test);
    runner.run();

    return 0;
}
