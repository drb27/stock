AM_CXXFLAGS=-std=c++11 -pthread
bin_PROGRAMS=stock stockgui
stock_SOURCES=src/stock/main.cpp
stock_LDADD=libstock.a

stockgui_SOURCES=src/stockgui/main.cpp 
nodist_stockgui_SOURCES=src/stockgui/resources.c
stockgui_LDADD=libstock.a $(GTK_LIBS)
stockgui_CPPFLAGS = $(GTK_CFLAGS) -Isrc/stock/

noinst_LIBRARIES = libstock.a
libstock_a_SOURCES= \
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

EXTRA_DIST=src/stockgui/window.ui src/stockgui/stock.gresource.xml

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

src/stockgui/resources.c: src/stockgui/window.ui src/stockgui/stock.gresource.xml
	cd src/stockgui; glib-compile-resources stock.gresource.xml --target=resources.c --generate-source
