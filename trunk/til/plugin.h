/*  plugin.h
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

#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <glib.h>
#include <gmodule.h>
#include <til.h>

#define TIL_PLUGIN_SUFFIX "tilplugin"

typedef gboolean (*ViewFunc) (TIL_View);
typedef gboolean (*ProcessEventFunc) (TIL_View, const TIL_Keyevent *, TIL_Cmd ***);

typedef struct _Plugin
{
	GModule *module;
	ViewFunc addView;
	ViewFunc removeView;
	ProcessEventFunc processEvent;
} Plugin;

gboolean plugin_init ();
gboolean plugin_cleanup ();

GSList *getPluginsInDir (gchar * path);
const Plugin *lockPlugin (TIL_const_PluginID id);
void unlockPlugin (TIL_const_PluginID id);

#endif
