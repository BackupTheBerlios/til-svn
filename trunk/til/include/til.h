/*  til.h
 *
 *  This file is part of the Text Input Layer (TIL).
 *  Copyright (C) 2005 Pascal Maillard <pascalmaillard@web.de>
 *
 *  The Text Input Layer is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This Text Input Layer is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#ifndef _TIL_H_
#define _TIL_H_

#include <stdlib.h>
#include <glib.h>
#include <gmodule.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include <til/keyevent.h>
#include <til/commands.h>

typedef gint TIL_View;

gboolean til_init ();
gboolean til_cleanup ();
gint til_lastErrorCode ();
const gchar *til_lastErrorMessage (); //TODO: i18n

#define til_createPluginInfo() g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free)
#define til_destroyPluginInfo(pi) if (pi != NULL) g_hash_table_destroy (pi); else
gchar **til_getAvailableSysPlugins (); //TODO
gchar **til_getPluginsInDir (gchar * path);
/*const GHashTable* til_getPluginInfo( gchar* pluginID );*/
GHashTable *til_readPluginInfo (gchar * filename);

gboolean til_loadPlugin (gchar * filename, const gchar ** pPluginID);
gboolean til_loadSysPlugin (gchar * filename, const gchar ** pPluginID); //TODO
/*gboolean til_reloadPlugin (const gchar * pluginID);*/
gboolean til_unloadPlugin (const gchar * pluginID);
const gchar **til_getLoadedPlugins ();
gboolean til_unloadAllPlugins ();

gboolean til_registerView (TIL_View * pView, const gchar * pluginID);
gboolean til_unregisterView (TIL_View view);
gboolean til_unregisterAllViews ();
gboolean til_setDefaultPlugin (const gchar * pluginID);
gboolean til_changePlugin (TIL_View view, const gchar * pluginID);
gboolean til_processEvent (TIL_View view, const TIL_Keyevent * event, TIL_Cmd *** pCmdArray);
#define til_freeCmdArray(cmdArray) g_strfreev ((gchar **) cmdArray)

#ifdef __cplusplus
}
#endif
#endif
