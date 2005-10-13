/*  plugin.c
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

#include <string.h>
#include <til.h>
#include <glib.h>
#include <gmodule.h>

#include "plugin.h"

static GHashTable *_loadedPluginTable = NULL;
static GSList *_pluginDirs = NULL;

void
destroyPlugin (void *p)
{
	Plugin *plugin = (Plugin *) p;
	g_free (plugin);
}

gint
pluginCmp (const GHashTable *plugin, TIL_const_PluginID id)
{
	return strcmp (g_hash_table_lookup ((GHashTable *)plugin, "id"), id);
}

GSList *
getPluginsInDir (gchar * path)
{
	GSList *filelist = NULL;
	int count = 0;

	/* open the directory */
	GDir *dir = NULL;
	gchar *syspath = NULL;
	if (path != NULL)
	{
		syspath = g_filename_from_utf8 (path, -1, NULL, NULL, NULL);
		dir = g_dir_open (syspath, 0, NULL);
	}
	if (dir != NULL)
	{
		const gchar *sysfilename = NULL;
		do
		{
			/* read the next dir entry */
			sysfilename = g_dir_read_name (dir);
			if (sysfilename != NULL)
			{
				/* test if it is a regular file */
				gchar *filepath = g_build_filename (syspath, sysfilename, NULL);
				if (g_file_test (filepath, G_FILE_TEST_IS_REGULAR))
				{
					gchar *filename = g_filename_to_utf8 (sysfilename, -1, NULL, NULL, NULL);
					/* test if it has the right suffix */
					if (g_str_has_suffix (filename, "." TIL_PLUGIN_SUFFIX))
					{
						/* add the filename to the list (without suffix) */
						gsize n = strlen (filename) - strlen ("." TIL_PLUGIN_SUFFIX);
						filename[n] = '\0';
						filelist = g_slist_append (filelist, g_build_filename (path, filename, NULL));
						count++;
					}
					g_free (filename);
				}
				g_free (filepath);
			}
		}
		while (sysfilename != NULL);
		g_dir_close (dir);
	}
	g_free (syspath);

	return filelist;
}

GHashTable *
readPluginInfo (gchar * filename)
{
	if (filename == NULL)
		return NULL;
	GHashTable *pi = til_createPluginInfo ();
	gboolean freefilename = FALSE;
	if (!g_str_has_suffix (filename, ".tilplugin"))
	{
		filename = g_strconcat (filename, ".tilplugin", NULL);
		freefilename = TRUE;
	}
	gchar *sysfilename = g_filename_from_utf8 (filename, -1, NULL, NULL, NULL);
	GIOChannel *ch = g_io_channel_new_file (sysfilename, "r", NULL);
	if (ch != NULL)
	{
		gchar *line;
		while (g_io_channel_read_line (ch, &line, NULL, NULL, NULL) == G_IO_STATUS_NORMAL)
		{
			gchar **v = g_strsplit (line, "=", 2);
			if (v[0] != NULL && v[1] != NULL)
			{
				g_strstrip (v[0]);
				g_strstrip (v[1]);
				g_hash_table_insert (pi, g_strdup (v[0]), g_strdup (v[1]));
			}
			g_strfreev (v);
			g_free (line);
		}
		g_io_channel_shutdown (ch, FALSE, NULL);
		g_io_channel_unref (ch);
	}
	else
	{
		til_destroyPluginInfo (pi);
		pi = NULL;
	}

	g_free (sysfilename);
	if (freefilename)
		g_free (filename);

	return pi;
}

void
destroyPluginInfoList (GSList *list)
{
	for (; list != NULL; list = g_slist_delete_link (list, list))
		til_destroyPluginInfo (list->data);
}

/* PUBLIC FUNCTIONS */

gboolean
plugin_init ()
{
	_loadedPluginTable = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	return _loadedPluginTable != NULL;
}

gboolean
plugin_cleanup ()
{
	if (!til_unloadAllPlugins ())
		return FALSE;
	if (_loadedPluginTable != NULL)
		g_hash_table_destroy (_loadedPluginTable);
	for (; _pluginDirs != NULL; _pluginDirs = g_slist_delete_link (_pluginDirs, _pluginDirs))
		g_free (_pluginDirs->data);
	return TRUE;
}

void
til_addPluginDirectory (const gchar * directory)
{
	GSList *elem = g_slist_find_custom (_pluginDirs, directory, (GCompareFunc) strcmp);
	if (elem == NULL)
		_pluginDirs = g_slist_append (_pluginDirs, g_strdup (directory));
}

void
til_removePluginDirectory (const gchar * directory)
{
	GSList *elem = g_slist_find_custom (_pluginDirs, directory, (GCompareFunc) strcmp);
	if (elem != NULL)
	{
		g_free (elem->data);
		_pluginDirs = g_slist_delete_link (_pluginDirs, elem);
	}
}

#ifdef G_OS_UNIX
#define PATH_SEP ":"
#elif defined G_OS_WIN32
#define PATH_SEP ";"
#endif

GSList *
til_getAvailablePlugins ()
{
	GSList *pluginFiles = NULL;
	gchar **dirs = NULL;

	/* collect the available plugin files */
	
	/* 1.: The environment variable TIL_PLUGIN_DIRS */
	const gchar *envvar = g_getenv ("TIL_PLUGIN_DIRS");
	if (envvar != NULL)
	{
		dirs = g_strsplit (envvar, PATH_SEP, 0);
		for (gchar **dir = dirs; *dir != NULL; dir++)
			pluginFiles = g_slist_concat (pluginFiles, getPluginsInDir (*dir));
		g_strfreev (dirs);
	}

	/* 2.: The file .til.conf in user's configuration directory */
	gchar *filepath = g_build_filename (g_get_user_config_dir (), ".til.conf", NULL);
	GIOChannel *file = g_io_channel_new_file (filepath, "r", NULL);
	g_free (filepath);
	if (file != NULL)
	{
		gchar *line = NULL;
		g_io_channel_read_line (file, &line, NULL, NULL, NULL);
		g_strstrip (line);
		dirs = g_strsplit (line, PATH_SEP, 0);
		for (gchar **dir = dirs; *dir != NULL; dir++)
			pluginFiles = g_slist_concat (pluginFiles, getPluginsInDir (*dir));
		g_strfreev (dirs);
		g_free (line);
		g_io_channel_close (file);
	}

	/* 3.: The directories in _pluginDirs */
	for (GSList *dir = _pluginDirs; dir != NULL; dir = g_slist_next (dir))
		pluginFiles = g_slist_concat (pluginFiles, getPluginsInDir (dir->data));

	/* 4.: The directories from the system's configuration */
#ifdef G_OS_UNIX
	filepath = g_build_filename (SYSCONFDIR, "til.conf", NULL);
	file = g_io_channel_new_file (filepath, "r", NULL);
	g_free (filepath);
	if (file != NULL)
	{
		gchar *line = NULL;
		g_io_channel_read_line (file, &line, NULL, NULL, NULL);
		g_strstrip (line);
		dirs = g_strsplit (line, PATH_SEP, 0);
		for (gchar **dir = dirs; *dir != NULL; dir++)
			pluginFiles = g_slist_concat (pluginFiles, getPluginsInDir (*dir));
		g_strfreev (dirs);
		g_free (line);
		g_io_channel_close (file);
	}
#endif

	/* 5.: The default directories */
	pluginFiles = g_slist_concat (pluginFiles, getPluginsInDir (PLUGINDIR));


	/* store the information about unique plugins in a list */
	GSList *ids = NULL, *infos = NULL;
	for (GSList *work = pluginFiles; work != NULL; work = g_slist_next (work))
	{
		GHashTable *info = readPluginInfo (work->data);
		if (info != NULL)
		{
			TIL_PluginID id = g_hash_table_lookup (info, "id");
			if (id != NULL && g_slist_find_custom (ids, id, (GCompareFunc) strcmp) == NULL)
			{
				/* add the file name to the info */
				g_hash_table_insert (info, g_strdup ("__filename__"), g_strdup (work->data));
				ids = g_slist_append (ids, id);
				infos = g_slist_append (infos, info);
			}
		}
		g_free (work->data);
	}
	g_slist_free (ids);
	g_slist_free (pluginFiles);

	return infos;
}

GHashTable *
til_getPluginInfo (TIL_const_PluginID id)
{
	GSList *plugins = til_getAvailablePlugins ();
	GSList *p = g_slist_find_custom (plugins, id, (GCompareFunc) pluginCmp);
	if (p == NULL)
		return NULL;
	GHashTable *info = p->data;
	plugins = g_slist_delete_link (plugins, p);
	destroyPluginInfoList (plugins);
		
	return info;
}

gboolean
til_loadPlugin (TIL_const_PluginID id)
{
	/* check that the plugin hasn't been loaded yet */
	if (id == NULL || g_hash_table_lookup (_loadedPluginTable, id) != NULL)
		return FALSE;

	gboolean success = FALSE;

	/* find plugin and retrieve its information */
	GSList *plugins = til_getAvailablePlugins ();
	GSList *p = g_slist_find_custom (plugins, id, (GCompareFunc) pluginCmp);
	gchar *filename = NULL;
	if (p != NULL)
	{
		filename = g_hash_table_lookup (p->data, "__filename__");

		/* load the library module */
		TIL_PluginID sysfilename = g_filename_from_utf8 (filename, -1, NULL, NULL, NULL);
		TIL_PluginID sysbasename = g_path_get_basename (sysfilename);
		TIL_PluginID sysdirname = g_path_get_dirname (sysfilename);
		TIL_PluginID sysmodulepath = g_module_build_path (sysdirname, sysbasename);
		GModule *module = g_module_open (sysmodulepath,
				G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
		if (module != NULL)
		{
			Plugin *plugin = g_malloc (sizeof (Plugin));
			/* load the necessary symbols */
			gboolean symsOk = TRUE;
			if (symsOk)
				symsOk =
					g_module_symbol (module, "addView", (gpointer *) & plugin->addView);
			if (symsOk)
				symsOk =
					g_module_symbol (module, "removeView",
							(gpointer *) & plugin->removeView);
			if (symsOk)
				symsOk =
					g_module_symbol (module, "processEvent",
							(gpointer *) & plugin->processEvent);

			if (symsOk)
			{
				/* insert the plugin in the list of the loaded plugins */
				plugin->module = module;
				g_hash_table_insert (_loadedPluginTable, g_strdup (id), plugin);

				/* indicate success */
				success = TRUE;
			}
			else
			{
				g_module_close (module);
				g_free (plugin);
			}
		}
		g_free (sysfilename);
		g_free (sysbasename);
		g_free (sysdirname);
		g_free (sysmodulepath);
	}

	/* clean up */
	destroyPluginInfoList (plugins);

	return success;
}

/* closes a plugin without removing it from the table */
gboolean
closePlugin (Plugin * plugin)
{
	g_module_close (plugin->module);
	return TRUE;
}

gboolean
til_unloadPlugin (TIL_const_PluginID id)
{
	if (id == NULL)
		return FALSE;
	Plugin *plugin = g_hash_table_lookup (_loadedPluginTable, id);
	if (plugin == NULL)
		return FALSE;
	if (closePlugin (plugin))
	{
		g_hash_table_remove (_loadedPluginTable, id);
		return TRUE;
	}
	else
		return FALSE;
}

gboolean
closePluginHelperFunc (gpointer key, gpointer value, gpointer user_data)
{
	return closePlugin ((Plugin *) value);
}

gboolean
til_unloadAllPlugins ()
{
	if (_loadedPluginTable == NULL)
		return TRUE;
	/* remove all the plugins from the hash table that could be unloaded and 
	 * return TRUE if and only if all plugins could be unloaded */
	guint tableSize = g_hash_table_size (_loadedPluginTable);
	return tableSize == g_hash_table_foreach_remove (_loadedPluginTable, closePluginHelperFunc, NULL);
}

void
addKeyToArray (gpointer key, gpointer value, gpointer pArray)
{
	TIL_PluginID **pWork = pArray;
	**pWork = key;
	(*pWork)++;
}

TIL_const_PluginID *
til_getLoadedPlugins ()
{
/*	if( _loadedPluginTable == NULL )
		return NULL;*/
	TIL_PluginID *ids = g_malloc (sizeof (TIL_PluginID) * (g_hash_table_size (_loadedPluginTable) + 1));
	TIL_PluginID *work = ids;
	g_hash_table_foreach (_loadedPluginTable, addKeyToArray, &work);
	*work = NULL;
	return (TIL_const_PluginID *) ids;
}

const Plugin *
lockPlugin (TIL_const_PluginID pluginID)
{
	return g_hash_table_lookup (_loadedPluginTable, pluginID);
}

void
unlockPlugin (TIL_const_PluginID pluginID)
{
}
