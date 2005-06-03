/*  parse.c
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

#include "parse.h"
#include <glib.h>
#include <til.h>
#include <string.h>
#include <stdio.h>

static GIOChannel *testFile = NULL;
static GString *line = NULL;
static int linecount = 0;
static TestPair *testPair = NULL;

gboolean
openTestFile (gchar * filename)
{
	if (testFile != NULL)
		return FALSE;

	testFile = g_io_channel_new_file (filename, "r", NULL);
	if (testFile == NULL)
		return FALSE;

	return TRUE;
}

void
closeTestFile ()
{
	if (testFile != NULL)
		g_io_channel_close (testFile);
	g_string_free (line, FALSE);
	linecount = 0;
}

TIL_Keycode
unescapeKey (gchar * escaped)
{
	return TIL_Key_unknown;
}

gint
keyToModifier (TIL_Keycode code)
{
	gint m = 0;
	switch (code)
	{
	case TIL_Key_Shift:
		m = TIL_Mod_Shift;
		break;
	case TIL_Key_Control:
		m = TIL_Mod_Control;
		break;
	case TIL_Key_Alt:
		m = TIL_Mod_Alt;
		break;
	case TIL_Key_Meta:
		m = TIL_Mod_Meta;
		break;
	}
	return m;
}

gunichar
keyToChar (TIL_Keycode code)
{
	gchar c = 0;
	switch (code)
	{
	case TIL_Key_Space:
		c = ' ';
		break;
	case TIL_Key_Tab:
		c = '\t';
		break;
	case TIL_Key_Return:
	case TIL_Key_Enter:
		c = '\n';
		break;
	}
	return (gunichar) c;
}

/* a (string,integer) pair, called mapping
 * a mapping with str == NULL is called a "null mapping"
 */
typedef struct
{
	const gchar *str;
	int val;
} Mapping;


/* Maps a string to an integer. It searches the mapping vector m for a string that is a
 * prefix of str and returns its associated value. If the string is not found in the
 * mapping vector the "val" field of the null mapping is returned. */
int
map (const Mapping * m, gchar * str)
{
	int val;
	while (m->str != NULL)
	{
		if (g_str_has_prefix (str, m->str))
		{
			val = m->val;
			break;
		}
		m++;
	}
	/* if the string wasn't found, return the value given by the null mapping */
	if (m->str == NULL)
		val = m->val;
	return val;
}

/* unescapes a string (\s->space, \t->tab, \n->newline, \\->backspace) */
gchar *
unescapeText (gchar * str)
{
	if (str == NULL)
		return NULL;
	gchar *res = g_malloc (strlen (str) + 1);
	gchar *s = str, *d = res; /* working pointers */
	while (*s != '\0')
	{
		if (*s != '\\')
			*d = *s;
		else
		{
			s++;
			switch (*s)
			{
			case 's':
				*d = ' ';
				break;
			case 't':
				*d = '\t';
				break;
			case 'n':
				*d = '\n';
				break;
			case '\\':
				*d = '\\';
				break;
			default:
				*d = *s;
			}
		}
		if (*s != '\0')
		{
			s++;
			d++;
		}
	}
	*d = '\0';
	return res;
}

	

/* Converts the string representation of a command into it's internal representation, or
 * NULL on failure. */
TIL_Cmd *
cmdFromString (gchar * str)
{
	Mapping cmdMap[] = {
		{"move", TIL_Cmd_Move},
		{"cursor", TIL_Cmd_Cursor},
		{"select", TIL_Cmd_Select},
		{"search", TIL_Cmd_Search},
		{"delete", TIL_Cmd_Delete},
		{"insert", TIL_Cmd_Insert},
		{"replace", TIL_Cmd_Replace},
		{"copy", TIL_Cmd_Copy},
		{"undo", TIL_Cmd_Undo},
		{"redo", TIL_Cmd_Redo},
		{"indent", TIL_Cmd_Indent},
		{"complete", TIL_Cmd_Complete},
		{"format", TIL_Cmd_Format},
		{"open", TIL_Cmd_Open},
		{"status", TIL_Cmd_Status},
		{NULL, TIL_Cmd_Unknown},
	};

	TIL_Cmd *cmd = NULL;

	g_strstrip (str);
	gchar **words = g_strsplit_set (str, " \t", -1);
	TIL_CmdID id;
	if (words[0] != NULL)
		id = (TIL_CmdID) map (cmdMap, words[0]);
	if (id == TIL_Cmd_Unknown)
		goto out;

	switch (id)
	{
	case TIL_Cmd_Move:
		{
			Mapping entityMap[] = {
				{"row", TIL_Cmd_Move_Row},
				{"col", TIL_Cmd_Move_Column},
				{"char", TIL_Cmd_Move_Character},
				{"word", TIL_Cmd_Move_Word},
				{"sent", TIL_Cmd_Move_Sentence},
				{"block", TIL_Cmd_Move_Block},
				{"par", TIL_Cmd_Move_Paragraph},
				{NULL, -1},
			};
			Mapping flagsMap[] = {
				{"back", TIL_Cmd_Move_Backwards},
				{"end", TIL_Cmd_Move_EndOf},
				{"abs", TIL_Cmd_Move_Absolute},
				{"nowrap", TIL_Cmd_Move_NoLineWrap},
				{"virt", TIL_Cmd_Move_Virtual},
				{NULL, 0},
			};

			if (words[1] == NULL)
				goto out;

			TIL_Cmd_Move_Args args = { TIL_Cmd_Move_Row, 1, 0 };
			args.entity = (TIL_Cmd_Move_Entity) map (entityMap, words[1]);
			if ((int) args.entity < 0)
				goto out;
			if (words[2] != NULL)
			{
				/* read the count and go out if it could not be read */
				if (sscanf (words[2], "%u", &args.count) != 1)
					goto out;
				if (words[3] != NULL)
				{
					/* the flags */
					gchar **flags = g_strsplit (words[3], "|", -1);
					gboolean error = FALSE;
					for (int i = 0; flags[i] != NULL; i++)
					{
						int flag = map (flagsMap, flags[i]);
						if (flag == 0)
							error = TRUE;
						else
							args.flags |= flag;
					}
					g_strfreev (flags);
					if (error)
						goto out;
					/* if there are additional parameters, go out */
					if (words[4] != NULL)
						goto out;
				}
			}

			/* allocate the command and init it with the args */
			size_t size = sizeof (TIL_Cmd) + sizeof (TIL_Cmd_Move_Args);
			cmd = g_malloc (size);
			cmd->size = size;
			memcpy (cmd->args, &args, sizeof (args));
		}
		break;

	case TIL_Cmd_Cursor:
		break;
	case TIL_Cmd_Select:
		{
			Mapping modeMap[] = {
				{"none", TIL_Cmd_Select_None},
				{"norm", TIL_Cmd_Select_Normal},
				{"line", TIL_Cmd_Select_Line},
				{"block", TIL_Cmd_Select_Block},
				{NULL, -1},
			};

			TIL_Cmd_Select_Args args = { TIL_Cmd_Select_Normal };

			if (words[1] != NULL)
			{
				args.mode = (TIL_Cmd_Select_Mode) map (modeMap, words[1]);
				if ((int) args.mode < 0)
					goto out;
				if (words[2] != NULL)
					goto out;
			}

			/* allocate the command and init it with the args */
			size_t size = sizeof (TIL_Cmd) + sizeof (TIL_Cmd_Select_Args);
			cmd = g_malloc (size);
			cmd->size = size;
			memcpy (cmd->args, &args, sizeof (args));
		}
		break;

	case TIL_Cmd_Search:
		break;
	case TIL_Cmd_Delete:

		/* delete takes no args */
		if (words[1] != NULL)
			goto out;

		cmd = g_malloc (sizeof (TIL_Cmd));
		cmd->size = sizeof (TIL_Cmd);
		break;

	case TIL_Cmd_Insert:
	case TIL_Cmd_Replace:
		{
			gboolean clipboard = TRUE;
			gchar *text = NULL;
			if (words[1] != NULL)
			{
				if (g_str_has_prefix (words[1], "c"))
				{ /* clipboard */
					if (words[2] != NULL)
						goto out;
				}
				else if (g_str_has_prefix (words[1], "t"))
				{ /* text */
					if (words[2] == NULL || words[3] != NULL)
						goto out;
					clipboard = FALSE;
					text = unescapeText (words[2]);
				}
				else
					goto out;
			}

			size_t size = sizeof(TIL_Cmd) + sizeof(TIL_Cmd_Insert_Args) +
					(text != NULL ? (strlen (text) + 1) : 0);
			cmd = g_malloc (size);
			cmd->size = size;
			((TIL_Cmd_Insert_Args*) cmd->args)->clipboard = clipboard;
			if (text != NULL)
			{
				strcpy (((TIL_Cmd_Insert_Args*) cmd->args)->text, text);
				g_free (text);
			}
		}
		break;
	case TIL_Cmd_Copy:
		break;
	case TIL_Cmd_Undo:
		break;
	case TIL_Cmd_Redo:
		break;
	case TIL_Cmd_Indent:
		break;
	case TIL_Cmd_Complete:
		break;
	case TIL_Cmd_Format:
		break;
	case TIL_Cmd_Open:
		break;
	case TIL_Cmd_Status:
		break;
	}
	if (cmd != NULL)
		cmd->id = id;

  out:
	g_strfreev (words);
	return cmd;
}

TestPair *
getNextTestPair ()
{
	if (testFile == NULL)
		return NULL;
	if (testPair != NULL)
	{
		g_free (testPair->keyevent);
		for (TIL_Cmd *work = *testPair->commands; work != NULL; work++)
		{
			g_free (work);
		}
		g_strfreev ((gchar **) testPair->commands);
		g_free (testPair);
		testPair = NULL;
	}

	while (g_io_channel_read_line_string (testFile, line, NULL, NULL) == G_IO_STATUS_NORMAL)
	{
		linecount++;

		/* strip of comments and whitespace */
		for (int i = 0; i < line->len; i++)
		{
			if (line->str[i] == '#')
			{
				g_string_truncate (line, i);
				break;
			}
		}
		g_strstrip (line->str);

		/* skip line if blank */
		if (line->len == 0)
			continue;

		TIL_Keyevent *keyevent = NULL, *keyevent2 = NULL;
		gboolean hit = FALSE;
		TIL_Keyevent_Type type;
		gboolean autorep = FALSE;
		gint modifiers = 0;

		/* extract the fields */
		gchar **fields = g_strsplit (line->str, "::", 0);
		if (fields[0] == NULL || fields[1] == NULL)
			goto parseError;

		/*
		 * FIELD 1 - EVENT TYPE
		 */
		g_strstrip (fields[0]);
		if (g_ascii_strcasecmp (fields[0], "press") == 0)
		{
			type = TIL_Event_Pressed;
		}
		else if (g_ascii_strcasecmp (fields[0], "pressrep") == 0)
		{
			type = TIL_Event_Pressed;
			autorep = TRUE;
		}
		else if (g_ascii_strcasecmp (fields[0], "release") == 0)
		{
			type = TIL_Event_Released;
		}
		else if (g_ascii_strcasecmp (fields[0], "hit") == 0)
		{
			type = TIL_Event_Pressed;
			hit = TRUE;
		}
		else
		{
			goto parseError;
		}

		/*
		 * FIELD 2 - KEY
		 */
		char *keystring = g_strstrip (fields[1]);
		TIL_Keycode code = TIL_Key_unknown;
		GString *text = g_string_new ("");
		gboolean hasNonPrintable = FALSE;	/* TRUE <=> the key sequence has non-printable chars */
		while (*keystring != '\0')
		{
			gunichar c = g_utf8_get_char_validated (keystring, -1);
			if (!g_unichar_validate (c))
				goto parseError;
			keystring = g_utf8_next_char (keystring);
			if (c == (gunichar) '<' && *keystring != '\0')
			{					/* an escaped key */
				gchar *esckey = keystring;
				/* go to the next '>' */
				gunichar ch;
				do
				{
					ch = g_utf8_get_char_validated (keystring, -1);
					if (!g_unichar_validate (ch))
						goto parseError;
					keystring = g_utf8_next_char (keystring);
				}
				while (*keystring != '\0' && ch != (gunichar) '>');

				if (*keystring == '\0' && ch != (gunichar) '>')
				{				/* string not finished with '>' */
					goto parseError;
				}
				/* overwrite the '>' with NULL character, so that esckey contains the string
				 * inside the brackets */
				*(keystring - 1) = '\0';
				TIL_Keycode tempcode = unescapeKey (esckey);
				if (tempcode == TIL_Key_unknown)
					goto parseError;
				c = keyToChar (tempcode);
				if (c == 0)
				{				/* non-printable character */
					if (code != TIL_Key_unknown)
						/* we already have a key - this is not valid */
						goto parseError;

					hasNonPrintable = TRUE;
					code = tempcode;
				}
			}

			if (c != 0)
			{
				if (hasNonPrintable)
					/* non-printable characters and text together is not valid */
					goto parseError;

				g_string_append_unichar (text, c);
			}
		}

		/*
		 * FIELD 3 - MODIFIERS
		 */

		gchar **modv = g_strsplit (fields[2], ",", 0);
		gint i = 0;
		while (modv[i] != NULL)
		{
			g_strstrip (modv[i]);
			switch (g_ascii_toupper (modv[i][0]))
			{
			case 'S':
				modifiers |= TIL_Mod_Shift;
				break;
			case 'C':
				modifiers |= TIL_Mod_Control;
				break;
			case 'A':
				modifiers |= TIL_Mod_Alt;
				break;
			case 'M':
				modifiers |= TIL_Mod_Meta;
				break;
			}
		}
		g_strfreev (modv);

		/*
		 * CONSTRUCT THE KEY EVENTS
		 */
		keyevent = g_malloc (sizeof (TIL_Keyevent) + text->len + 1);
		keyevent->type = type;
		keyevent->autorep = autorep;
		keyevent->modifiers = modifiers;
		keyevent->keycode = code;
		strcpy (keyevent->text, text->str);

		/*
		 * REMAINING FIELDS: COMMANDS
		 */
		GSList *cmds = NULL;
		for (int i = 3; fields[i] != NULL; i++)
		{
			TIL_Cmd *cmd = cmdFromString (fields[i]);
			if (cmd == NULL)
			{
				GSList *work = cmds;
				while (work != NULL)
				{
					g_free(work->data);
					work = g_slist_next (work);
				}
				goto parseError;
			}
			g_slist_append (cmds, cmd);
		}
		int numCmds = i - 3;

		/*
		 * BUILD THE TEST PAIR
		 */
		testPair = g_malloc (sizeof (TestPair));
		testPair->keyevent = keyevent;
		testPair->commands = g_malloc (sizeof (TIL_Cmd *) * (numCmds + 1));
		GSList *work = cmds;
		for (int j = 0; j < numCmds; j++)
		{
			testPair->commands[j] = work->data;
			work = g_slist_next (work);
		}
		testPair->commands[numCmds] = NULL;
		testPair->line = linecount;
		goto succeeded;

	  parseError:
		printf ("error on line %d\n", linecount);
		g_free (keyevent);
		continue;

	  succeeded:
		g_strfreev (fields);
		g_slist_free (cmds);
		return testPair;
	}							/* end while */

  eof:
	closeTestFile (testFile);
	return NULL;
}
