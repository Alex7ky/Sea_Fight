#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Sea_Fight], [1.0])
m4_include([m4/ax_pthread.m4])
m4_include([m4/ax_with_curses.m4])

AM_MAINTAINER_MODE([disable])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/client/client.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
AX_PTHREAD
AX_PTHREAD([], [AC_MSG_ERROR([requires pthread library])])
AX_WITH_CURSES
if test "x$ax_cv_ncurses" != xyes; then
  AC_MSG_ERROR([requires Ncurses library])
fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h stdio.h unistd.harpa/inet.h netinet/in.h sys/socket.h locale.h malloc.h string.h sys/time.h fcntl.h stdbool.h ])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_BIGENDIAN
AC_TYPE_UID_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([bzero inet_ntoa inet_aton memset select socket])

AC_CONFIG_FILES([Makefile 
		 src/Makefile
		 src/client/Makefile
		 src/server/Makefile])

AC_OUTPUT
