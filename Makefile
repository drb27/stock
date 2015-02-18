CCFLAGS= -pthread -std=c++11 -Wl,--no-as-needed
COVERAGEFLAGS=-fprofile-arcs -O0 -ftest-coverage -fno-inline -ggdb
LIBS=-lcurl -ljansson
GPP=g++-4.9
MODULES=$(wildcard *.cpp)
TEST_MODULES=$(wildcard test/*.cpp)
TEST_OBJS=$(TEST_MODULES:.cpp=.o)
OBJS=$(MODULES:.cpp=.o)
TEST_TARGETS=buffer.o

TEST_TARGETS_COVERAGE=$(patsubst %.o,%_cov.o,$(TEST_TARGETS)) 

-include $(MODULES:.cpp=.d)
-include $(TEST_MODULES:.cpp=.d)

all: stock test/stock-tests

stock: $(OBJS)
	$(GPP) $(OBJS) -O2 -o stock -pthread $(CCFLAGS) $(LIBS)

test/stock-tests: $(TEST_OBJS) $(TEST_TARGETS_COVERAGE)
	$(GPP) $(TEST_OBJS) $(TEST_TARGETS_COVERAGE) -o test/stock-tests -pthread $(CCFLAGS) $(COVERAGEFLAGS) $(INCLUDE) $(LIBS) -lcppunit -lgcov

test: test/stock-tests
	./test/stock-tests

%.o: %.cpp
	$(GPP) -c $*.cpp $(CCFLAGS) -o $@
	$(GPP) -c $*.cpp -MM -MT $@ $(CCFLAGS) > $*.d

%_cov.o: %.cpp
	$(GPP) -c $*.cpp $(CCFLAGS) $(COVERAGEFLAGS) -o $@
	$(GPP) -c $*.cpp -MM -MT $@ $(CCFLAGS) $(COVERAGEFLAGS) > $*.d

coverage: test/stock-tests
	rm -rf cov
	rm -f *.gcda
	mkdir cov
	./test/stock-tests
	cd cov; lcov --capture --output-file app.info -b $(CURDIR) --directory $(CURDIR)
	cd cov; lcov --remove app.info "/usr*" -o app.info
	cd cov; genhtml app.info --demangle-cpp
	firefox cov/index.html &

clean:
	rm -f stock test/stock-tests *.o *.gcda *.gcno *~ *.d test/*.o test/*.d test/*.gcda test/*.gcno test/*~
	rm -rf cov html latex
