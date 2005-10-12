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
	static gboolean selectMode = FALSE;
	static gboolean overrideMode = FALSE;

	GSList *cmdlist = NULL;
	/* printable characters - replace the selected text with them */
	if (event->text[0] != '\0')
	{
		gunichar c = g_utf8_get_char (event->text);
		if (g_unichar_isprint (c))
		{
			int textlen = strlen (event->text);
			size_t size = sizeof(TIL_Cmd) + sizeof (TIL_Cmd_Replace_Args) + textlen + 1;
			TIL_Cmd *pCmd = g_malloc (size);
			pCmd->size = size;
			pCmd->id = TIL_Cmd_Replace;
			TIL_Cmd_Replace_Args args;
			args.clipboard = FALSE;
			memcpy (pCmd->args, &args, sizeof (TIL_Cmd_Replace_Args));
			memcpy (pCmd->args + sizeof (TIL_Cmd_Replace_Args), event->text, textlen + 1);
			cmdlist = g_slist_append (cmdlist, pCmd);
			if (selectMode)
			{
				TIL_Cmd_Select_Args args = { TIL_Cmd_Select_None };
				cmdlist = g_slist_append (cmdlist, til_createCmd (TIL_Cmd_Select,
							&args, sizeof (args)));
				selectMode = FALSE;
			}
		}
	}

	/* movements */
	TIL_Cmd_Move_Args moveargs = { TIL_Cmd_Move_Row, 1, 0 };
	gboolean isMovement = FALSE;
	switch (event->keycode)
	{
		case TIL_Key_Up:
			moveargs.flags |= TIL_Cmd_Move_Backwards;
		case TIL_Key_Down:
			moveargs.entity = TIL_Cmd_Move_Row;
			moveargs.flags |= TIL_Cmd_Move_Screen;
			isMovement = TRUE;
			break;

		case TIL_Key_Left:
			moveargs.flags |= TIL_Cmd_Move_Backwards;
		case TIL_Key_Right:
			moveargs.entity = event->modifiers & TIL_Mod_Control ?
				TIL_Cmd_Move_Word : TIL_Cmd_Move_Column;
			isMovement = TRUE;
			break;

		case TIL_Key_Home:
		case TIL_Key_End:
			moveargs.entity = event->modifiers & TIL_Mod_Control ?
				TIL_Cmd_Move_Doc : TIL_Cmd_Move_Line;
			moveargs.flags = event->keycode == TIL_Key_Home ?
				TIL_Cmd_Move_Backwards : TIL_Cmd_Move_EndOf;
			if (!(event->modifiers & TIL_Mod_Control))
				moveargs.flags |= TIL_Cmd_Move_Screen;
			isMovement = TRUE;
			break;

		case TIL_Key_PageUp:
			moveargs.flags |= TIL_Cmd_Move_Backwards;
		case TIL_Key_PageDown:
			moveargs.entity = TIL_Cmd_Move_Page;
			if (!(event->modifiers & TIL_Mod_Control))
				moveargs.flags |= TIL_Cmd_Move_Screen;
			isMovement = TRUE;
			break;
		default:
			break;
	}

	if (isMovement)
	{
		TIL_Cmd_Select_Args selectargs = { TIL_Cmd_Select_Default };
		if (!selectMode && (event->modifiers & TIL_Mod_Shift))
		{
			selectMode = TRUE;
			cmdlist = g_slist_append (cmdlist, til_createCmd (TIL_Cmd_Select,
						&selectargs, sizeof (selectargs)));
		}
		else if (selectMode && !(event->modifiers & TIL_Mod_Shift))
		{
			selectMode = FALSE;
			selectargs.mode = TIL_Cmd_Select_None;
			cmdlist = g_slist_append (cmdlist, til_createCmd (TIL_Cmd_Select,
						&selectargs, sizeof (selectargs)));
		}
		cmdlist = g_slist_append (cmdlist, til_createCmd (TIL_Cmd_Move,
				   	&moveargs, sizeof (moveargs)));
	}

	/* backspace and delete */
	if (event->keycode == TIL_Key_Backspace ||
		   event->keycode == TIL_Key_Delete)
	{
		TIL_Cmd_Select_Args selectargs = { TIL_Cmd_Select_Default };
		if (!selectMode)
		{
			cmdlist = g_slist_append (cmdlist, til_createCmd (TIL_Cmd_Select,
						&selectargs, sizeof (selectargs)));
			TIL_Cmd_Move_Args moveargs = { TIL_Cmd_Move_Column, 1, 0 };
			if (event->keycode == TIL_Key_Backspace)
				moveargs.flags |= TIL_Cmd_Move_Backwards;
			if (event->modifiers & TIL_Mod_Control)
				moveargs.entity = TIL_Cmd_Move_Word;
			if (event->keycode == TIL_Key_Delete && event->modifiers & TIL_Mod_Control)
				moveargs.flags |= TIL_Cmd_Move_EndOf;
			cmdlist = g_slist_append (cmdlist, til_createCmd (TIL_Cmd_Move,
						&moveargs, sizeof (moveargs)));
		}
		cmdlist = g_slist_append (cmdlist, til_createCmd (TIL_Cmd_Delete, NULL, 0));

		selectargs.mode = TIL_Cmd_Select_None;
		cmdlist = g_slist_append (cmdlist, til_createCmd (TIL_Cmd_Select,
					&selectargs, sizeof (selectargs)));
		selectMode = FALSE;
	}


	/* override mode */
	if (event->keycode == TIL_Key_Insert && event->modifiers == 0)
	{
		overrideMode = overrideMode ? FALSE : TRUE;
		TIL_Cmd_Cursor_Args args;
		args.shape = overrideMode ? TIL_Cmd_Cursor_Block : TIL_Cmd_Cursor_Line;
		cmdlist = g_slist_append (cmdlist, til_createCmd (TIL_Cmd_Cursor,
					&args, sizeof (args)));
		cmdlist = g_slist_append (cmdlist, til_createCmd (TIL_Cmd_Override,
					&overrideMode, sizeof (gboolean)));
	}

	/* convert the list into an array */
	TIL_Cmd ** cmds = g_malloc (sizeof(TIL_Cmd*) * (g_slist_length (cmdlist) + 1));
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

