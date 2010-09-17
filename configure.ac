# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([TripleTriad], [0.2.0_pre1])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE(1.11 foreign)
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([src/tripletriad.cc])
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.
SDL_VERSION=1.2.14
AM_PATH_SDL($SDL_VERSION, :, AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!]))
CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

AC_CHECK_LIB([SDL_gfx], [pixelRGBA], LIBS="$LIBS -lSDL_gfx", AC_MSG_ERROR([*** SDL_gfx library not found!]))

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_SUBST(CXXFLAGS)
AC_SUBST(LIBS)

AC_CONFIG_FILES([Makefile
		 src/Makefile])
AC_OUTPUT
