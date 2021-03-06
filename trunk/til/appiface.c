/*  appiface.c
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
#include <til.h>

#include "plugin.h"

#define NUM_VIEWS 256

static TIL_const_PluginID _viewPluginMap[NUM_VIEWS] = {
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};

static TIL_View _nextView = 0;

static TIL_const_PluginID _defaultPlugin = NULL;

gboolean
til_registerView (TIL_View * pView, TIL_const_PluginID id)
{
	if (pView == NULL)
		return FALSE;
	if (_nextView == -1)
		return FALSE;
	if (id == NULL)
		id = _defaultPlugin;
	if (id == NULL)
		return FALSE;

	/* register the view in the plugin */
	const Plugin *plugin = lockPlugin (id);
	if (plugin == NULL)
		return FALSE;
	gboolean added = plugin->addView (_nextView);
	unlockPlugin (id);

	if (!added)
		return FALSE;

	/* insert the view in the map */
	_viewPluginMap[_nextView] = id;

	/* give the view to the user */
	*pView = _nextView;

	/* advance the counter to the next free view */
	TIL_View i = (_nextView + 1) % NUM_VIEWS;
	while (_viewPluginMap[i] != NULL && i != _nextView)
	{
		i = (i + 1) % NUM_VIEWS;
	}
	if (i == _nextView)
		_nextView = -1;
	else
		_nextView = i;

	return TRUE;
}

gboolean
til_unregisterView (TIL_View view)
{
	if (view < 0 || view >= NUM_VIEWS)
		return FALSE;

	/* instruct the plugin to remove the view */
	TIL_const_PluginID id = _viewPluginMap[view];
	if (id == NULL)
		return FALSE;
	const Plugin *plugin = lockPlugin (id);
	if (plugin == NULL)
		return FALSE;
	gboolean removed = plugin->removeView (view);
	unlockPlugin (id);

	if (!removed)
		return FALSE;

	/* remove the view from the map */
	_viewPluginMap[view] = NULL;

	/* adjust _nextView if all views were previously occupied */
	if (_nextView == -1)
		_nextView = view;

	return TRUE;
}

gboolean
til_unregisterAllViews ()
{
	gboolean success = TRUE;

	for (int i=0; i<NUM_VIEWS; i++)
	{
		if (_viewPluginMap[i] != NULL)
			if (!til_unregisterView (i))
				success = FALSE;
	}
	return success;
}

gboolean
til_setDefaultPlugin (TIL_const_PluginID id)
{
	_defaultPlugin = id;
	return TRUE;
}

gboolean
til_changePlugin (TIL_View view, TIL_const_PluginID id)
{
	if (view < 0 || view >= NUM_VIEWS)
		return FALSE;
	if (id == NULL)
		id = _defaultPlugin;

	/* remove from old plugin */
	TIL_const_PluginID fromID = _viewPluginMap[view];
	const Plugin *plugin = NULL;
	if (fromID != NULL)
	{
		plugin = lockPlugin (fromID);
		if (plugin == NULL)
			return FALSE;
		gboolean removed = plugin->removeView (view);
		unlockPlugin (fromID);

		if (!removed)
			return FALSE;
	}

	/* add to new plugin */
	plugin = lockPlugin (id);
	if (plugin == NULL)
		return FALSE;
	gboolean added = plugin->addView (view);
	unlockPlugin (id);

	if (!added)
		return FALSE;

	return TRUE;
}

gboolean
til_processEvent (TIL_View view, const TIL_Keyevent * event, TIL_Cmd *** pCmds)
{
	if (view < 0 || view >= NUM_VIEWS || pCmds == NULL)
		return FALSE;
	TIL_const_PluginID id = _viewPluginMap[view];
	if (id == NULL)
		return FALSE;

	const Plugin *plugin = lockPlugin (id);
	if (plugin == NULL)
		return FALSE;
	gboolean success = plugin->processEvent (view, event, pCmds);
	unlockPlugin (id);

	if (!success)
		return FALSE;

	return TRUE;
}
