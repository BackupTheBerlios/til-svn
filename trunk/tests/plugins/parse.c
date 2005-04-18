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

static GIOChannel *testFile = NULL;
static GString *line = NULL;

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

TestPair *
getNextTestPair ()
{
	if (testFile == NULL)
		return NULL;

	while (g_io_channel_read_line_string (testFile, line, NULL, NULL) == G_IO_STATUS_NORMAL)
	{
		/* strip of the comments */
		for (int i = 0; i < line->len; i++)
		{
			if (line->str[i] == '#')
			{
				g_string_truncate (line, i);
				break;
			}
		}
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
			type = TIL_Event_Pressed hit = TRUE;
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
		GString text = g_string_new ("");
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
				do
				{
					gunichar ch = g_utf8_get_char_validated (keystring, -1);
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

		gchar **modv = g_strsplit (fields[2], ',', 0);
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


	  parseError:
		g_strfreev (fields);
	}							/* end while */

  eof:
	closeTestFile (testFile);
	return NULL;
}
