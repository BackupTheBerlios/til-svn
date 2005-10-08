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

static GHashTable *_pluginTable = NULL;

void
destroyPlugin (void *p)
{
	Plugin *plugin = (Plugin *) p;
	g_free (plugin);
}

gboolean
plugin_init ()
{
	_pluginTable = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);
	return _pluginTable != NULL;
}

gboolean
plugin_cleanup ()
{
	if (!til_unloadAllPlugins ())
		return FALSE;
	if (_pluginTable != NULL)
		g_hash_table_destroy (_pluginTable);
	return TRUE;
}

gchar **
til_getAvailableSysPlugins ()
{
	return g_malloc0 (sizeof (gchar *));
}

gchar **
til_getPluginsInDir (gchar * path)
{
	GSList *filelist = NULL;
	int count = 0;
	gchar **array = NULL;

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
						filelist = g_slist_append (filelist, g_strndup (filename, n));
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

	/* copy the filenames to the array */
	array = g_malloc (sizeof (gchar *) * (count + 1));
	GSList *work = filelist;
	int i = 0;
	while (work)
	{
		array[i++] = work->data;
		work = g_slist_next (work);
	}
	array[i] = NULL;
	g_slist_free (filelist);

	return array;
}

GHashTable *
til_readPluginInfo (gchar * filename)
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

gboolean
til_loadPlugin (gchar * filename, const gchar ** pPluginID)
{
	if (filename == NULL || pPluginID == NULL)
		return FALSE;
	gboolean freefilename = FALSE;
	gboolean success = FALSE;
	/* retrieve plugin information */
	GHashTable *pi = til_readPluginInfo (filename);
	if (pi == NULL)
		return FALSE;
	gchar *pluginID = g_hash_table_lookup (pi, "id");
	if (pluginID != NULL)
	{
		/* check that the plugin hasn't been loaded yet */
		if (g_hash_table_lookup (_pluginTable, pluginID) == NULL)
		{
			/* load the library module */
			if (g_str_has_suffix (filename, "." TIL_PLUGIN_SUFFIX))
			{
				gsize n = strlen (filename) - strlen ("." TIL_PLUGIN_SUFFIX);
				filename = g_strndup (filename, n);
				freefilename = TRUE;
			}
			gchar *sysfilename = g_filename_from_utf8 (filename, -1, NULL, NULL, NULL);
			gchar *sysbasename = g_path_get_basename (sysfilename);
			gchar *sysdirname = g_path_get_dirname (sysfilename);
			gchar *sysmodulepath = g_module_build_path (sysdirname, sysbasename);
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
					pluginID = g_strdup (pluginID);
					g_hash_table_insert (_pluginTable, pluginID, plugin);

					/* present the plugin id to the user */
					*pPluginID = pluginID;

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
	}

	/* clean up */
	til_destroyPluginInfo (pi);
	if (freefilename)
		g_free (filename);

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
til_unloadPlugin (const gchar * pluginID)
{
	if (pluginID == NULL)
		return FALSE;
	Plugin *plugin = g_hash_table_lookup (_pluginTable, pluginID);
	if (plugin == NULL)
		return FALSE;
	if (closePlugin (plugin))
	{
		g_hash_table_remove (_pluginTable, pluginID);
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
	if (_pluginTable == NULL)
		return TRUE;
	/* remove all the plugins from the hash table that could be unloaded and 
	 * return TRUE if and only if all plugins could be unloaded */
	guint tableSize = g_hash_table_size (_pluginTable);
	return tableSize == g_hash_table_foreach_remove (_pluginTable, closePluginHelperFunc, NULL);
}

void
addKeyToArray (gpointer key, gpointer value, gpointer pArray)
{
	gchar ***pWork = pArray;
	**pWork = key;
	(*pWork)++;
}

const gchar **
til_getLoadedPlugins ()
{
/*	if( _pluginTable == NULL )
		return NULL;*/
	gchar **ids = g_malloc (sizeof (gchar *) * (g_hash_table_size (_pluginTable) + 1));
	gchar **work = ids;
	g_hash_table_foreach (_pluginTable, addKeyToArray, &work);
	*work = NULL;
	return (const gchar **) ids;
}

const Plugin *
lockPlugin (const gchar * pluginID)
{
	return g_hash_table_lookup (_pluginTable, pluginID);
}

void
unlockPlugin (const gchar * pluginID)
{
}
