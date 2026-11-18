AC_PREREQ([2.69])
AC_INIT([growable-buf], [0.1.0], [craaaaaachind@gmail.com])
AC_CONFIG_SRCDIR([buf.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AM_PROG_AR
LT_INIT([disable-static])
PKG_CHECK_MODULES([CHECK], [check >= 0.9.10])

AC_ARG_ENABLE([coverage],
    [AS_HELP_STRING([--enable-coverage], [instrument a separate test build for gcov])],
    [], [enable_coverage=no])
AC_PATH_PROG([GCOV], [gcov])
AS_IF([test "x$enable_coverage" = xyes], [
    AS_IF([test -z "$GCOV"], [AC_MSG_ERROR([gcov is required for coverage])])
    CFLAGS="$CFLAGS -O0 -g --coverage"
    LDFLAGS="$LDFLAGS --coverage"
])
AM_CONDITIONAL([ENABLE_COVERAGE], [test "x$enable_coverage" = xyes])

AC_CONFIG_FILES([Makefile growable-buf.pc])
AC_OUTPUT
