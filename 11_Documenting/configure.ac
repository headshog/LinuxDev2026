AC_PREREQ([2.69])
AC_INIT([guess-number], [0.2.0], [craaaaaachind@gmail.com])
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
AC_PATH_PROG([DOXYGEN], [doxygen])
AC_PATH_PROG([HELP2MAN], [help2man])
AC_PATH_PROG([DOT], [dot])
AS_IF([test -z "$PYTHON" || test -z "$DOXYGEN" || test -z "$HELP2MAN" || test -z "$DOT"],
    [AC_MSG_ERROR([python3, doxygen, help2man and graphviz are required])])

AC_CONFIG_FILES([Makefile po/Makefile.in Doxyfile])
AC_OUTPUT
