/*  til.c
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
init_tests ()
{
	return til_init ()? 0 : 1;
}

int
cleanup_tests ()
{
	return til_cleanup ()? 0 : 1;
}

void
base_test ()
{
	gchar const **list;

	// no plugins should be loaded now
	list = til_getLoadedPlugins ();
	CU_ASSERT_PTR_NULL (*list);
	g_free (list);
}

void
pluginInfo_test ()
{
	// destroy with NULL pointer
	til_destroyPluginInfo (NULL);

	// create and destroy
	GHashTable *pi = til_createPluginInfo ();
	til_destroyPluginInfo (pi);
}

void
isDummyPlugin (const GHashTable * pi)
{
	CU_ASSERT_PTR_NOT_NULL (pi);
	if (pi == NULL)
		return;
	CU_ASSERT_STRING_EQUAL (g_hash_table_lookup ((GHashTable *) pi, "id"), "til.dummy");
	CU_ASSERT_STRING_EQUAL (g_hash_table_lookup ((GHashTable *) pi, "name"), "Dummy");
}

void
plugin_readInfo_test ()
{
	// pass NULL filename
	til_readPluginInfo (NULL);
	
	// read info of dummy plugin
	GHashTable *pi = til_readPluginInfo ("plugins/dummy/dummy");
	isDummyPlugin (pi);
	til_destroyPluginInfo (pi);
}

void
plugin_locate_test ()
{
	gchar **list;

	// pass NULL path
	list = til_getPluginsInDir (NULL);
	CU_ASSERT_PTR_NOT_NULL_FATAL (list);
	CU_ASSERT_PTR_NULL (list[0]);
	g_strfreev (list);

	// get the available system plugins, currently, there are none
	list = til_getAvailableSysPlugins ();
	CU_ASSERT_PTR_NOT_NULL_FATAL (list);
	CU_ASSERT_PTR_NULL (list[0]);
	g_strfreev (list);

	// get the available plugins in the build dir
	list = til_getPluginsInDir ("plugins/dummy");
	CU_ASSERT_PTR_NOT_NULL_FATAL (list);
	CU_ASSERT (list[0] != NULL && list[1] == NULL);
	if (list[0] != NULL)
	{
		CU_ASSERT_STRING_EQUAL (list[0], "dummy");
	}
	g_strfreev (list);
}

void
plugin_load_test ()
{
	const gchar *id = NULL;

	// pass bogus parameters
	til_loadPlugin (NULL, NULL);
	til_loadPlugin ("plugins/dummy/dummy", NULL);
	til_loadPlugin (NULL, &id);
	til_loadPlugin ("aolrechoaeu", NULL);
	til_loadPlugin ("aolrechoaeu", &id);
	til_unloadPlugin (NULL);

	// load dummy plugin
	CU_ASSERT_FATAL (til_loadPlugin ("plugins/dummy/dummy", &id));
	CU_ASSERT_PTR_NOT_NULL_FATAL (id);
	CU_ASSERT_STRING_EQUAL (id, "til.dummy");

	// test if it is in the list of loaded plugins
	const gchar **pluginList = til_getLoadedPlugins ();
	CU_ASSERT_PTR_NOT_NULL_FATAL (pluginList);
	CU_ASSERT (pluginList[0] != NULL && pluginList[1] == NULL);
	if (pluginList[0] != NULL)
	{
		CU_ASSERT_STRING_EQUAL (pluginList[0], "til.dummy");
	}
	g_free (pluginList);

	// unload plugin
	CU_ASSERT (til_unloadPlugin (id));

	// check that plugin list is empty
	pluginList = til_getLoadedPlugins ();
	CU_ASSERT_PTR_NOT_NULL_FATAL (pluginList);
	CU_ASSERT_PTR_NULL (pluginList[0]);
	g_free (pluginList);

	// load it again
	CU_ASSERT_FATAL (til_loadPlugin ("plugins/dummy/dummy", &id));

	// unload all plugins
	CU_ASSERT (til_unloadAllPlugins ());

	// check that plugin list is empty
	pluginList = til_getLoadedPlugins ();
	CU_ASSERT_PTR_NOT_NULL_FATAL (pluginList);
	CU_ASSERT_PTR_NULL (pluginList[0]);
	g_free (pluginList);
}

#define NUM_VIEWS 256

void
register_view_test ()
{
	// pass bogus parameters
	til_registerView (NULL, NULL);
	til_unregisterView (-1);
	til_unregisterView (NUM_VIEWS);
	
	// load the dummy plugin and set it as default
	const gchar *id = NULL;
	CU_ASSERT_FATAL (til_loadPlugin ("plugins/dummy/dummy", &id));
	CU_ASSERT (til_setDefaultPlugin (id));
	
	// register and unregister a view
	TIL_View view = -1;
	CU_ASSERT (til_registerView (&view, NULL));
	CU_ASSERT (view != -1);
	CU_ASSERT (til_unregisterView (view));

	// register maximum number of views
	for (int i=0; i<NUM_VIEWS; i++)
	{
		CU_ASSERT (til_registerView (&view, NULL));
	}

	// the next register should fail
	CU_ASSERT_FALSE (til_registerView (&view, NULL));

	// unregister a view, then register a view again
	CU_ASSERT (til_unregisterView (view));
	CU_ASSERT (til_registerView (&view, NULL));

	// unregister all views
	CU_ASSERT (til_unregisterAllViews ());
	
	// unload plugins
	CU_ASSERT (til_unloadAllPlugins());
}

static CU_TestInfo tests[] = {
	{"init and cleanup", base_test},
	{"plugin info create/destroy", pluginInfo_test},
	{"read plugin info", plugin_readInfo_test},
	{"find available plugins", plugin_locate_test},
	{"load and unload plugins", plugin_load_test},
	{"register views", register_view_test},
	CU_TEST_INFO_NULL,
};

CU_SuiteInfo tests_til_suiteInfo = { "TIL test suite", init_tests, cleanup_tests, tests };
