AC_PREREQ([2.56])

AC_INIT([Text Input Layer],[alpha version],[pascalmaillard@web.de],[til])
AC_CONFIG_SRCDIR([til/appiface.c])

# check for standard headers
AC_HEADER_STDC

# check for glib>=2.4 and set GLIB_CFLAGS, GLIB_LIBS properly
AM_PATH_GLIB_2_0([2.4.0],[],[AC_MSG_ERROR([GLIB >= 2.4 not found. Please install it.])], [gmodule gthread])

# check if the unit tests shall be enabled
AC_ARG_ENABLE([unittests], [AC_HELP_STRING([--enable-unittests], [enable if unit tests shall be built (for developers)])])
if test x"$enable_unittests" == xyes; then
	UNITTESTS_ENABLE=yes;
else 
	UNITTESTS_ENABLE=no;
fi
AC_SUBST(UNITTESTS_ENABLE)

# if unittests are enabled: check for CUnit 2.0 and ncurses lib 
if test x$UNITTESTS_ENABLE == xyes; then
	AC_CHECK_HEADER([CUnit/CUnit.h], [], [AC_MSG_ERROR([CUnit headers >= 2.0 not found. Please install them.])], [ ])
	AC_SUBST([HAVE_CUNIT_CUNIT_H])
	AC_CHECK_DECL([CU_ASSERT], [], [AC_MSG_ERROR([CUnit headers >= 2.0 not found. Please install them.])], [#include <CUnit/CUnit.h>])
	AC_CHECK_LIB([cunit], [CU_initialize_registry], [], [AC_MSG_ERROR([CUnit library >= 2.0 not found. Please install it.])])

	UNITTESTS_CURSES=yes

	# check if Curses.h from CUnit is installed
	AC_CHECK_HEADERS([CUnit/Curses.h], [], [UNITTESTS_CURSES=no], [ ])
	AC_CHECK_LIB([ncurses], [initscr], [], [UNITTESTS_CURSES=no])
	AC_SUBST([UNITTESTS_CURSES])

	if test $UNITTESTS_CURSES != yes; then
		AC_MSG_WARN([The curses unittests will be disabled.])
	fi
fi

# on Windows, the path to the source directory must be written with backslashes
win_top_srcdir=`echo $srcdir|sed -e 's@/@\\\\\\\\@g'`
AC_SUBST([win_top_srcdir])

AC_CONFIG_FILES([Jamfile])

AC_OUTPUT
