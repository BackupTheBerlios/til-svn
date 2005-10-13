/*  plugin_modern.c
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

#include <CUnit/CUnit.h>
#include <til.h>
#include <glib.h>

int
plugin_modern_init_tests ()
{
	if (!til_init ())
		return 1;
	til_addPluginDirectory ("plugins/modern");
	if (!til_loadPlugin ("til.modern"))
		return 1;
	til_setDefaultPlugin ("til.modern");
	return 0;
}

int
plugin_modern_cleanup_tests ()
{
	return til_cleanup () ? 0 : 1;
}

void
view_test ()
{
	/* register and unregister some views */
	TIL_View view;
	CU_ASSERT (til_registerView (&view, NULL));
	CU_ASSERT (til_registerView (&view, NULL));
	CU_ASSERT (til_registerView (&view, NULL));
	CU_ASSERT (til_unregisterView (view));
	CU_ASSERT (til_registerView (&view, NULL));
	CU_ASSERT (til_unregisterView (view));
	CU_ASSERT (til_unregisterAllViews ());
}

static CU_TestInfo tests[] = {
	{"manage views", view_test},
	CU_TEST_INFO_NULL,
};

CU_SuiteInfo tests_plugin_modern_suiteInfo = { "Modern plugin test suite", plugin_modern_init_tests, plugin_modern_cleanup_tests, tests };
