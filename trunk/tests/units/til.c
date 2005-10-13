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

#include "error.h"

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
set_error_test ()
{
	// check for correct return values at the beginning
	CU_ASSERT_EQUAL (til_lastErrorCode (), 0);
	CU_ASSERT_PTR_NULL (til_lastErrorMessage ());

	// set errors and check if they are returned
	setError (-1, "testerror %d", 1);
	CU_ASSERT_EQUAL (til_lastErrorCode (), -1);
	const gchar *msg = til_lastErrorMessage ();
	CU_ASSERT_PTR_NOT_NULL (msg);
	if (msg != NULL)
	{
		CU_ASSERT_STRING_EQUAL (msg, "testerror 1");
	}

	setError (-2, "testerror %d", 2);
	CU_ASSERT_EQUAL (til_lastErrorCode (), -2);
	msg = til_lastErrorMessage ();
	CU_ASSERT_PTR_NOT_NULL (msg);
	if (msg != NULL)
	{
		CU_ASSERT_STRING_EQUAL (msg, "testerror 2");
	}
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

extern GHashTable *readPluginInfo (gchar * filename);

void
plugin_readInfo_test ()
{
	// pass NULL filename
	readPluginInfo (NULL);
	
	// read info of dummy plugin
	GHashTable *pi = readPluginInfo ("plugins/dummy/dummy");
	isDummyPlugin (pi);
	til_destroyPluginInfo (pi);
}


extern GSList *getPluginsInDir (gchar * path);

void
freeList (GSList *list)
{
	for (; list != NULL; list = g_slist_delete_link (list,list))
		g_free (list->data);
}

void
plugin_locate_test ()
{
	GSList *list;

	// test internal getPluginsInDir function

	// pass NULL path
	list = getPluginsInDir (NULL);
	CU_ASSERT_PTR_NULL (list);

	// get the available plugins in the dummy dir
	list = getPluginsInDir ("plugins/dummy");
	CU_ASSERT (g_slist_length (list) == 1);
	CU_ASSERT (list->data != NULL);
	if (list->data != NULL)
	{
		CU_ASSERT_STRING_EQUAL (list->data, "plugins/dummy/dummy");
	}
	freeList (list);

	// check til_getAvailablePlugins and til_getPluginInfo
	til_addPluginDirectory ("plugins/dummy");
	GHashTable *info = til_getPluginInfo ("til.dummy");
	CU_ASSERT (info != NULL);
	til_destroyPluginInfo (info);
	til_removePluginDirectory ("plugins/dummy");


#if 0
 * NOTE: The function til_getAvailablePlugins is not entirely tested using unit tests, because
 * it interacts too much with the system
 *
	// test high level til_getAvailablePlugins function, note: only the first 3 locations
	// (environment vars, user's config file, app-defined directory) are checked

	// 1.: assure that there are no til configuration files or plugins in the system
	gchar *filepath = g_build_filename (g_get_user_config_dir (), ".til.conf", NULL);
	if (g_file_test (filepath, G_FILE_TEST_EXISTS))
		CU_FAIL_FATAL ("The personal configuration file \".til.conf\" exists.");
	g_free (filepath);

#ifdef G_OS_UNIX
	filepath = g_build_filename (SYSCONFDIR, "til.conf", NULL);
	if (g_file_test (filepath, G_FILE_TEST_EXISTS))
		CU_FAIL_FATAL ("The systemwide configuration file \"til.conf\" exists.");
	g_free (filepath);
#endif

	if (g_file_test (PLUGINDIR, G_FILE_TEST_DIR))
		CU_FAIL_FATAL ("The system's default plugin directory exists.");
#endif
}

void
plugin_load_test ()
{
	// pass bogus parameters
	til_loadPlugin (NULL);
	til_loadPlugin ("aolrechoaeu");
	til_unloadPlugin (NULL);

	// load dummy plugin
	til_addPluginDirectory ("plugins/dummy");
	CU_ASSERT_FATAL (til_loadPlugin ("til.dummy"));

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
	CU_ASSERT (til_unloadPlugin ("til.dummy"));

	// check that plugin list is empty
	pluginList = til_getLoadedPlugins ();
	CU_ASSERT_PTR_NOT_NULL_FATAL (pluginList);
	CU_ASSERT_PTR_NULL (pluginList[0]);
	g_free (pluginList);

	// load it again
	CU_ASSERT_FATAL (til_loadPlugin ("til.dummy"));

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
	CU_ASSERT_FATAL (til_loadPlugin ("til.dummy"));
	CU_ASSERT (til_setDefaultPlugin ("til.dummy"));
	
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
	{"error functions", set_error_test},
	{"init and cleanup", base_test},
	{"plugin info create/destroy", pluginInfo_test},
	{"read plugin info", plugin_readInfo_test},
	{"find available plugins", plugin_locate_test},
	{"load and unload plugins", plugin_load_test},
	{"register views", register_view_test},
	CU_TEST_INFO_NULL,
};

CU_SuiteInfo tests_til_suiteInfo = { "TIL test suite", init_tests, cleanup_tests, tests };
