#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([stock], [0.1], [david.bradshaw.usa@gmail.com])
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([src/stock/main.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_AR

AC_PROG_RANLIB

# Check for Doxygen
AC_CHECK_PROGS([DOXYGEN],[doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - disabling Doxygen support])
fi

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcppunit':
AC_CHECK_LIB([cppunit], [main])
# FIXME: Replace `main' with a function in `-lcurl':
AC_CHECK_LIB([curl], [main])
# FIXME: Replace `main' with a function in `-lgcov':
AC_CHECK_LIB([gcov], [main])
# FIXME: Replace `main' with a function in `-ljansson':
AC_CHECK_LIB([jansson], [main])

# Checks for header files.
AC_CHECK_HEADERS([memory.h stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

#Check fot GTK3
AM_PATH_GTK_3_0
PKG_CHECK_MODULES(GTK,gtk+-3.0)

# Checks for library functions.
AC_FUNC_MALLOC

# Set up Doxygen support
AM_CONDITIONAL([HAVE_DOXYGEN],[test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN],[AC_CONFIG_FILES([docs/Doxyfile])])

# Generate the makefiles
AC_CONFIG_FILES([Makefile docs/Makefile src/stockgui/window.ui])

# Output the results
AC_OUTPUT
