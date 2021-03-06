/*  main.c
 *
 *  This file is part of the Text Input Layer (TIL).
 *  Copyright (C) 2005 Pascal Maillard <pascalmaillard@web.de>
 *
 *  The Text Input Layer is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  The Text Input Layer is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/
#include <glib.h>
#include <CUnit/CUnit.h>

extern void run_tests ();

extern CU_SuiteInfo tests_til_suiteInfo;
extern CU_SuiteInfo tests_plugin_modern_suiteInfo;
extern CU_SuiteInfo tests_plugintest_suiteInfo;

int
main ()
{
	/* init test registry */
	CU_initialize_registry ();

	/* register suites and tests */
	CU_SuiteInfo suites[] = {
		tests_til_suiteInfo,
		tests_plugin_modern_suiteInfo,
		tests_plugintest_suiteInfo,
		CU_SUITE_INFO_NULL,
	};
	CU_register_suites (suites);

	/* run the tests */
	run_tests ();

	/* cleanup */
	CU_cleanup_registry ();

	return 0;
}
