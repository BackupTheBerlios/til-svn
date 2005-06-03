/*  iface.c
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

#include <til.h>
#include <glib.h>
#include <string.h>

G_MODULE_EXPORT gboolean
addView (TIL_View * pView)
{
	return TRUE;
}

G_MODULE_EXPORT gboolean
removeView (TIL_View view)
{
	return TRUE;
}

G_MODULE_EXPORT gboolean
processEvent (TIL_View view, const TIL_Keyevent * event, TIL_Cmd *** pCmds)
{
	GSList *cmdlist = NULL;
	TIL_Cmd *pCmd = NULL;
	gint numCmds = 0;
	/* printable characters - replace the selected text with them */
	if (event->text[0] != '\0')
	{
		gunichar c = g_utf8_get_char (event->text);
		if (g_unichar_isprint (c))
		{
			size_t size = sizeof(TIL_Cmd) + strlen (event->text) + 1
			pCmd = g_malloc (size);
			pCmd->size = size;
			pCmd->id = TIL_Cmd_Replace;
			memcpy (pCmd->args, event->text, textlen + 1);
			cmdlist = g_slist_append (cmdlist, pCmd);
			numCmds++;
		}
	}

	/* convert the list into an array */
	TIL_Cmd ** cmds = g_malloc (sizeof(TIL_Cmd*) * (numCmds + 1));
	GSList *temp = cmdlist;
	int i=0;
	while (temp != NULL)
	{
		cmds[i++] = temp->data;
		temp = g_slist_next(temp);
	}
	cmds[i] = NULL;
	*pCmds = cmds;
	g_slist_free (cmdlist);

	return TRUE;
}

