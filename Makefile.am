AM_CXXFLAGS=-std=c++11 -pthread
bin_PROGRAMS=stock
stock_SOURCES=src/stock/main.cpp \
	src/stock/buffer.cpp \
	src/stock/deathrattle.cpp \
	src/stock/stock_fetcher.cpp \
	src/stock/worker_base.cpp \
	src/stock/buffer.h \
	src/stock/deathrattle.h \
	src/stock/stock_fetcher.h \
	src/stock/sweepup.h \
	src/stock/worker_base.h

TESTS=stock_tests
check_PROGRAMS=stock_tests
stock_tests_SOURCES = src/test/main.cpp \
	src/test/test-buffer.cpp \
	src/test/test-buffer.h \
	src/stock/buffer.cpp \
	src/stock/buffer.h

stock_tests_CXXFLAGS=-O0 -fprofile-arcs -ftest-coverage -std=c++11 -pthread
CXX=g++-4.9

coverage: check
	rm -rf cov
	mkdir cov
	cd cov; lcov --capture --output-file app.info -b $(CURDIR)/src --directory $(CURDIR)/src
	cd cov; lcov --remove app.info "/usr*" -o app.info
	cd cov; lcov --remove app.info "src/test*" -o app.info
	cd cov; genhtml app.info --demangle-cpp
	rm -f src/stock/*.gcda

