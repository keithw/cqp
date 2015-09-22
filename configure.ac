#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([cqp], [0.01], [bad-project@cs.stanford.edu])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/cqp.cc])
AC_CONFIG_HEADERS([config.h])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Add picky CXXFLAGS
CXX14_FLAGS="-std=c++14 -pthread"
basic_cxxflags="-Wall -Wpedantic -Wextra -Werror"
PICKY_CXXFLAGS="$basic_cxxflags -Weffc++ -Wconversion -Wsign-conversion"
TOLERANT_CXXFLAGS="$basic_cxxflags"
AC_SUBST([CXX14_FLAGS])
AC_SUBST([PICKY_CXXFLAGS])
AC_SUBST([TOLERANT_CXXFLAGS])

# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB

# Checks for libraries.
PKG_CHECK_MODULES([PANGOCAIRO], [pangocairo])
PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
		 src/Makefile])

AC_OUTPUT
