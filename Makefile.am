BUILDSTAMP=$(shell date)
AM_CXXFLAGS=-std=c++11 -pthread -DBUILDSTAMP="$(BUILDSTAMP)" -Isrc
bin_PROGRAMS=stock stockgui
stock_SOURCES=src/stock/main.cpp
stock_LDADD=libstock.a
stock_CPPFLAGS=-Isrc

stockgui_SOURCES=src/stockgui/main.cpp 
nodist_stockgui_SOURCES=src/stockgui/resources.c
stockgui_LDADD=libstock.a $(GTK_LIBS)
stockgui_CPPFLAGS = $(GTK_CFLAGS) -Isrc

noinst_LIBRARIES = libstock.a

libstock_a_SOURCES= \
	src/stocklib/problem.h \
	src/stocklib/buffer.cpp \
	src/stocklib/deathrattle.cpp \
	src/stocklib/buffer.h \
	src/stocklib/deathrattle.h \
	src/stocklib/sweepup.h \
	src/stocklib/state.h \
	src/stocklib/task.cpp \
	src/stocklib/task.h \
	src/stocklib/i_worker.h \
	src/stocklib/i_resultor.h \
	src/stocklib/tickerproblem.h \
	src/stocklib/tickerproblem.cpp \
	src/stocklib/stocklib.h \
	src/stocklib/stocklib_p.h \
	src/stocklib/stocklib.cpp \
	src/stocklib/urltask.h \
	src/stocklib/urltask.cpp \
	src/stocklib/black-scholes.cpp \
	src/stocklib/black-scholes.h

TESTS=stock_tests
check_PROGRAMS=stock_tests
stock_tests_SOURCES = src/test/main.cpp \
	src/test/test-buffer.cpp \
	src/test/test-buffer.h \
	src/stocklib/buffer.cpp \
	src/stocklib/buffer.h \
	src/test/test-problem.cpp \
	src/test/test-problem.h \
	src/stocklib/problem.h \
	src/test/test-state.cpp \
	src/test/test-state.h \
	src/stocklib/state.h \
	src/test/test-task.h \
	src/test/test-task.cpp \
	src/stocklib/task.h \
	src/stocklib/i_worker.h \
	src/stocklib/i_resultor.h \
	src/stocklib/tickerproblem.h \
	src/stocklib/tickerproblem.cpp \
	src/stocklib/stocklib.h \
	src/stocklib/stocklib_p.h \
	src/stocklib/stocklib.cpp \
	src/test/test-stocklib.h \
	src/test/test-stocklib.cpp \
	src/stocklib/urltask.h \
	src/stocklib/urltask.cpp \
	src/stocklib/black-scholes.cpp \
	src/stocklib/black-scholes.h \
	src/test/test-black-scholes.cpp \
	src/test/test-black-scholes.h

stock_tests_CXXFLAGS= -Isrc -O0 -fprofile-arcs -ftest-coverage -std=c++11 -pthread

EXTRA_DIST=src/stockgui/window.ui.in src/stockgui/stock.gresource.xml

CXX=g++-4.9
SUBDIRS=docs/
coverage: check
	rm -rf cov
	mkdir cov
	cd cov; lcov --capture --output-file app.info -b $(CURDIR)/src --directory $(CURDIR)/src
	cd cov; lcov --remove app.info "/usr*" -o app.info
	cd cov; lcov --remove app.info "src/test*" -o app.info
	cd cov; genhtml app.info --demangle-cpp
	rm -f src/stock/*.gcda
	rm -f src/stocklib/*.gcda
	rm -f src/test/*.gcda

src/stockgui/resources.c: src/stockgui/window.ui src/stockgui/stock.gresource.xml
	cd src/stockgui; glib-compile-resources stock.gresource.xml --target=resources.c --generate-source
