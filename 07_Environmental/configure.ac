AC_PREREQ([2.69])
AC_INIT([rhasher], [0.1.0], [craaaaaachind@gmail.com])
AC_CONFIG_SRCDIR([rhasher.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_PROG_CC

# librhash is mandatory
AC_CHECK_HEADER([rhash.h], [],
    [AC_MSG_ERROR([rhash.h not found, install librhash development files])])
AC_CHECK_LIB([rhash], [rhash_library_init], [],
    [AC_MSG_ERROR([librhash not found, install librhash development files])])

# GNU readline is optional; --without-readline forces getline()
AC_ARG_WITH([readline],
    [AS_HELP_STRING([--without-readline],
        [read commands with getline() even if GNU readline is available])],
    [], [with_readline=check])

have_readline=no
AS_IF([test "x$with_readline" != xno], [
    AC_CHECK_HEADER([readline/readline.h],
        [AC_CHECK_LIB([readline], [readline], [have_readline=yes])])
    AS_IF([test "x$have_readline" = xyes], [
        AC_DEFINE([USE_READLINE], [1],
            [Define to 1 to read commands with GNU readline.])
        LIBS="-lreadline $LIBS"
    ], [test "x$with_readline" = xyes], [
        AC_MSG_FAILURE([--with-readline was given, but GNU readline was not found])
    ])
])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

AC_MSG_NOTICE([GNU readline support: $have_readline])
