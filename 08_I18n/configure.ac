AC_PREREQ([2.69])
AC_INIT([guess-number], [0.1.0], [craaaaaachind@gmail.com])
AC_CONFIG_SRCDIR([guess-number.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign])

AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CC
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_REQUIRE_VERSION([0.21])

AC_PATH_PROG([PYTHON], [python3])
AS_IF([test -z "$PYTHON"],
    [AC_MSG_ERROR([python3 is required to run the tests])])

AC_CONFIG_FILES([Makefile po/Makefile.in])
AC_OUTPUT
