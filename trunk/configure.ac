AC_PREREQ([2.59])

AC_INIT([Text Input Layer],[alpha version],[pascalmaillard@web.de],[til])
AC_CONFIG_SRCDIR([til/appiface.c])

# check for standard headers
AC_CHECK_HEADERS

# check for glib>=2.4 and set GLIB_CFLAGS, GLIB_LIBS properly
AM_PATH_GLIB_2_0([2.4.0],[],[AC_MSG_ERROR([GLIB >= 2.4 not found. Please install it.])], [gmodule gthread])

# check if the unit tests shall be enabled
AC_ARG_ENABLE([unittests], [AS_HELP_STRING([--enable-unittests], [enable if unit tests shall be built (for developers)])])
if test x"$enable_unittests" == xyes; then
	UNITTESTS_ENABLE=yes;
else 
	UNITTESTS_ENABLE=no;
fi
AC_SUBST(UNITTESTS_ENABLE)

# if unittests are enabled: check for CUnit 2.0 and ncurses lib 
if test x$UNITTESTS_ENABLE == xyes; then
	AC_CHECK_LIB([ncurses], [initscr], [], [AC_MSG_ERROR([ncurses library not found. Please install it.])])
	AC_CHECK_LIB([cunit], [CU_initialize_registry], [], [AC_MSG_ERROR([CUnit library >= 2.0 not found. Please install it.])])
	AC_CHECK_HEADER([CUnit/CUnit.h], [], [AC_MSG_ERROR([CUnit headers >= 2.0 not found. Please install it.])], [ ])
	AC_CHECK_DECL([CU_ASSERT], [], [AC_MSG_ERROR([CUnit headers >= 2.0 not found. Please install it.])], [#include <CUnit/CUnit.h>])
fi

AC_CONFIG_FILES([Jamfile])

AC_OUTPUT
