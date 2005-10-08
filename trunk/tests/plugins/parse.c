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
	default:
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
	default:
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
	int val = -1;
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


TIL_Keycode
unescapeKey (gchar * escaped)
{
	Mapping keyMap[] = {
		{"ESCAPE", TIL_Key_Escape},
		{"TAB", TIL_Key_Tab},
		{"BACKTAB", TIL_Key_BackTab},
		{"BACKSPACE", TIL_Key_BackSpace},
		{"RETURN", TIL_Key_Return},
		{"ENTER", TIL_Key_Enter},
		{"INSERT", TIL_Key_Insert},
		{"DELETE", TIL_Key_Delete},
		{"PAUSE", TIL_Key_Pause},
		{"PRINT", TIL_Key_Print},
		{"SYSREQ", TIL_Key_SysReq},
		{"CLEAR", TIL_Key_Clear},
		{"HOME", TIL_Key_Home},
		{"END", TIL_Key_End},
		{"LEFT", TIL_Key_Left},
		{"UP", TIL_Key_Up},
		{"RIGHT", TIL_Key_Right},
		{"DOWN", TIL_Key_Down},
		{"PRIOR", TIL_Key_Prior},
		{"PAGEUP", TIL_Key_PageUp},
		{"NEXT", TIL_Key_Next},
		{"PAGEDOWN", TIL_Key_PageDown},
		{"SHIFT", TIL_Key_Shift},
		{"CONTROL", TIL_Key_Control},
		{"META", TIL_Key_Meta},
		{"ALT", TIL_Key_Alt},
		{"CAPSLOCK", TIL_Key_CapsLock},
		{"NUMLOCK", TIL_Key_NumLock},
		{"SCROLLLOCK", TIL_Key_ScrollLock},
		{"F1", TIL_Key_F1},
		{"F2", TIL_Key_F2},
		{"F3", TIL_Key_F3},
		{"F4", TIL_Key_F4},
		{"F5", TIL_Key_F5},
		{"F6", TIL_Key_F6},
		{"F7", TIL_Key_F7},
		{"F8", TIL_Key_F8},
		{"F9", TIL_Key_F9},
		{"F10", TIL_Key_F10},
		{"F11", TIL_Key_F11},
		{"F12", TIL_Key_F12},
		{"F13", TIL_Key_F13},
		{"F14", TIL_Key_F14},
		{"F15", TIL_Key_F15},
		{"F16", TIL_Key_F16},
		{"F17", TIL_Key_F17},
		{"F18", TIL_Key_F18},
		{"F19", TIL_Key_F19},
		{"F20", TIL_Key_F20},
		{"F21", TIL_Key_F21},
		{"F22", TIL_Key_F22},
		{"F23", TIL_Key_F23},
		{"F24", TIL_Key_F24},
		{"F25", TIL_Key_F25},
		{"F26", TIL_Key_F26},
		{"F27", TIL_Key_F27},
		{"F28", TIL_Key_F28},
		{"F29", TIL_Key_F29},
		{"F30", TIL_Key_F30},
		{"F31", TIL_Key_F31},
		{"F32", TIL_Key_F32},
		{"F33", TIL_Key_F33},
		{"F34", TIL_Key_F34},
		{"F35", TIL_Key_F35},
		{"SUPER_L", TIL_Key_Super_L},
		{"SUPER_R", TIL_Key_Super_R},
		{"MENU", TIL_Key_Menu},
		{"HYPER_L", TIL_Key_Hyper_L},
		{"HYPER_R", TIL_Key_Hyper_R},
		{"HELP", TIL_Key_Help},
		{"DIRECTION_L", TIL_Key_Direction_L},
		{"DIRECTION_R", TIL_Key_Direction_R},
		{"SPACE", TIL_Key_Space},
		{"ANY", TIL_Key_Any},
		{"EXCLAM", TIL_Key_Exclam},
		{"QUOTEDBL", TIL_Key_QuoteDbl},
		{"NUMBERSIGN", TIL_Key_NumberSign},
		{"DOLLAR", TIL_Key_Dollar},
		{"PERCENT", TIL_Key_Percent},
		{"AMPERSAND", TIL_Key_Ampersand},
		{"APOSTROPHE", TIL_Key_Apostrophe},
		{"PARENLEFT", TIL_Key_ParenLeft},
		{"PARENRIGHT", TIL_Key_ParenRight},
		{"ASTERISK", TIL_Key_Asterisk},
		{"PLUS", TIL_Key_Plus},
		{"COMMA", TIL_Key_Comma},
		{"MINUS", TIL_Key_Minus},
		{"PERIOD", TIL_Key_Period},
		{"SLASH", TIL_Key_Slash},
		{"0", TIL_Key_0},
		{"1", TIL_Key_1},
		{"2", TIL_Key_2},
		{"3", TIL_Key_3},
		{"4", TIL_Key_4},
		{"5", TIL_Key_5},
		{"6", TIL_Key_6},
		{"7", TIL_Key_7},
		{"8", TIL_Key_8},
		{"9", TIL_Key_9},
		{"COLON", TIL_Key_Colon},
		{"SEMICOLON", TIL_Key_Semicolon},
		{"LESS", TIL_Key_Less},
		{"EQUAL", TIL_Key_Equal},
		{"GREATER", TIL_Key_Greater},
		{"QUESTION", TIL_Key_Question},
		{"AT", TIL_Key_At},
		{"A", TIL_Key_A},
		{"B", TIL_Key_B},
		{"C", TIL_Key_C},
		{"D", TIL_Key_D},
		{"E", TIL_Key_E},
		{"F", TIL_Key_F},
		{"G", TIL_Key_G},
		{"H", TIL_Key_H},
		{"I", TIL_Key_I},
		{"J", TIL_Key_J},
		{"K", TIL_Key_K},
		{"L", TIL_Key_L},
		{"M", TIL_Key_M},
		{"N", TIL_Key_N},
		{"O", TIL_Key_O},
		{"P", TIL_Key_P},
		{"Q", TIL_Key_Q},
		{"R", TIL_Key_R},
		{"S", TIL_Key_S},
		{"T", TIL_Key_T},
		{"U", TIL_Key_U},
		{"V", TIL_Key_V},
		{"W", TIL_Key_W},
		{"X", TIL_Key_X},
		{"Y", TIL_Key_Y},
		{"Z", TIL_Key_Z},
		{"BRACKETLEFT", TIL_Key_BracketLeft},
		{"BACKSLASH", TIL_Key_Backslash},
		{"BRACKETRIGHT", TIL_Key_BracketRight},
		{"ASCIICIRCUM", TIL_Key_AsciiCircum},
		{"UNDERSCORE", TIL_Key_Underscore},
		{"QUOTELEFT", TIL_Key_QuoteLeft},
		{"BRACELEFT", TIL_Key_BraceLeft},
		{"BAR", TIL_Key_Bar},
		{"BRACERIGHT", TIL_Key_BraceRight},
		{"ASCIITILDE", TIL_Key_AsciiTilde},
		{"NOBREAKSPACE", TIL_Key_nobreakspace},
		{"EXCLAMDOWN", TIL_Key_exclamdown},
		{"CENT", TIL_Key_cent},
		{"STERLING", TIL_Key_sterling},
		{"CURRENCY", TIL_Key_currency},
		{"YEN", TIL_Key_yen},
		{"BROKENBAR", TIL_Key_brokenbar},
		{"SECTION", TIL_Key_section},
		{"DIAERESIS", TIL_Key_diaeresis},
		{"COPYRIGHT", TIL_Key_copyright},
		{"ORDFEMININE", TIL_Key_ordfeminine},
		{"GUILLEMOTLEFT", TIL_Key_guillemotleft},
		{"NOTSIGN", TIL_Key_notsign},
		{"HYPHEN", TIL_Key_hyphen},
		{"REGISTERED", TIL_Key_registered},
		{"MACRON", TIL_Key_macron},
		{"DEGREE", TIL_Key_degree},
		{"PLUSMINUS", TIL_Key_plusminus},
		{"TWOSUPERIOR", TIL_Key_twosuperior},
		{"THREESUPERIOR", TIL_Key_threesuperior},
		{"ACUTE", TIL_Key_acute},
		{"MU", TIL_Key_mu},
		{"PARAGRAPH", TIL_Key_paragraph},
		{"PERIODCENTERED", TIL_Key_periodcentered},
		{"CEDILLA", TIL_Key_cedilla},
		{"ONESUPERIOR", TIL_Key_onesuperior},
		{"MASCULINE", TIL_Key_masculine},
		{"GUILLEMOTRIGHT", TIL_Key_guillemotright},
		{"ONEQUARTER", TIL_Key_onequarter},
		{"ONEHALF", TIL_Key_onehalf},
		{"THREEQUARTERS", TIL_Key_threequarters},
		{"QUESTIONDOWN", TIL_Key_questiondown},
		{"AGRAVE", TIL_Key_Agrave},
		{"AACUTE", TIL_Key_Aacute},
		{"ACIRCUMFLEX", TIL_Key_Acircumflex},
		{"ATILDE", TIL_Key_Atilde},
		{"ADIAERESIS", TIL_Key_Adiaeresis},
		{"ARING", TIL_Key_Aring},
		{"AE", TIL_Key_AE},
		{"CCEDILLA", TIL_Key_Ccedilla},
		{"EGRAVE", TIL_Key_Egrave},
		{"EACUTE", TIL_Key_Eacute},
		{"ECIRCUMFLEX", TIL_Key_Ecircumflex},
		{"EDIAERESIS", TIL_Key_Ediaeresis},
		{"IGRAVE", TIL_Key_Igrave},
		{"IACUTE", TIL_Key_Iacute},
		{"ICIRCUMFLEX", TIL_Key_Icircumflex},
		{"IDIAERESIS", TIL_Key_Idiaeresis},
		{"ETH", TIL_Key_ETH},
		{"NTILDE", TIL_Key_Ntilde},
		{"OGRAVE", TIL_Key_Ograve},
		{"OACUTE", TIL_Key_Oacute},
		{"OCIRCUMFLEX", TIL_Key_Ocircumflex},
		{"OTILDE", TIL_Key_Otilde},
		{"ODIAERESIS", TIL_Key_Odiaeresis},
		{"MULTIPLY", TIL_Key_multiply},
		{"OOBLIQUE", TIL_Key_Ooblique},
		{"UGRAVE", TIL_Key_Ugrave},
		{"UACUTE", TIL_Key_Uacute},
		{"UCIRCUMFLEX", TIL_Key_Ucircumflex},
		{"UDIAERESIS", TIL_Key_Udiaeresis},
		{"YACUTE", TIL_Key_Yacute},
		{"THORN", TIL_Key_THORN},
		{"SSHARP", TIL_Key_ssharp},
		{"AGRAVE", TIL_Key_agrave},
		{"AACUTE", TIL_Key_aacute},
		{"ACIRCUMFLEX", TIL_Key_acircumflex},
		{"ATILDE", TIL_Key_atilde},
		{"ADIAERESIS", TIL_Key_adiaeresis},
		{"ARING", TIL_Key_aring},
		{"AE", TIL_Key_ae},
		{"CCEDILLA", TIL_Key_ccedilla},
		{"EGRAVE", TIL_Key_egrave},
		{"EACUTE", TIL_Key_eacute},
		{"ECIRCUMFLEX", TIL_Key_ecircumflex},
		{"EDIAERESIS", TIL_Key_ediaeresis},
		{"IGRAVE", TIL_Key_igrave},
		{"IACUTE", TIL_Key_iacute},
		{"ICIRCUMFLEX", TIL_Key_icircumflex},
		{"IDIAERESIS", TIL_Key_idiaeresis},
		{"ETH", TIL_Key_eth},
		{"NTILDE", TIL_Key_ntilde},
		{"OGRAVE", TIL_Key_ograve},
		{"OACUTE", TIL_Key_oacute},
		{"OCIRCUMFLEX", TIL_Key_ocircumflex},
		{"OTILDE", TIL_Key_otilde},
		{"ODIAERESIS", TIL_Key_odiaeresis},
		{"DIVISION", TIL_Key_division},
		{"OSLASH", TIL_Key_oslash},
		{"UGRAVE", TIL_Key_ugrave},
		{"UACUTE", TIL_Key_uacute},
		{"UCIRCUMFLEX", TIL_Key_ucircumflex},
		{"UDIAERESIS", TIL_Key_udiaeresis},
		{"YACUTE", TIL_Key_yacute},
		{"THORN", TIL_Key_thorn},
		{"YDIAERESIS", TIL_Key_ydiaeresis},
		{"BACK", TIL_Key_Back},
		{"FORWARD", TIL_Key_Forward},
		{"STOP", TIL_Key_Stop},
		{"REFRESH", TIL_Key_Refresh},
		{"VOLUMEDOWN", TIL_Key_VolumeDown},
		{"VOLUMEMUTE", TIL_Key_VolumeMute},
		{"VOLUMEUP", TIL_Key_VolumeUp},
		{"BASSBOOST", TIL_Key_BassBoost},
		{"BASSUP", TIL_Key_BassUp},
		{"BASSDOWN", TIL_Key_BassDown},
		{"TREBLEUP", TIL_Key_TrebleUp},
		{"TREBLEDOWN", TIL_Key_TrebleDown},
		{"MEDIAPLAY", TIL_Key_MediaPlay},
		{"MEDIASTOP", TIL_Key_MediaStop},
		{"MEDIAPREV", TIL_Key_MediaPrev},
		{"MEDIANEXT", TIL_Key_MediaNext},
		{"MEDIARECORD", TIL_Key_MediaRecord},
		{"HOMEPAGE", TIL_Key_HomePage},
		{"FAVORITES", TIL_Key_Favorites},
		{"SEARCH", TIL_Key_Search},
		{"STANDBY", TIL_Key_Standby},
		{"OPENURL", TIL_Key_OpenUrl},
		{"LAUNCHMAIL", TIL_Key_LaunchMail},
		{"LAUNCHMEDIA", TIL_Key_LaunchMedia},
		{"LAUNCH0", TIL_Key_Launch0},
		{"LAUNCH1", TIL_Key_Launch1},
		{"LAUNCH2", TIL_Key_Launch2},
		{"LAUNCH3", TIL_Key_Launch3},
		{"LAUNCH4", TIL_Key_Launch4},
		{"LAUNCH5", TIL_Key_Launch5},
		{"LAUNCH6", TIL_Key_Launch6},
		{"LAUNCH7", TIL_Key_Launch7},
		{"LAUNCH8", TIL_Key_Launch8},
		{"LAUNCH9", TIL_Key_Launch9},
		{"LAUNCHA", TIL_Key_LaunchA},
		{"LAUNCHB", TIL_Key_LaunchB},
		{"LAUNCHC", TIL_Key_LaunchC},
		{"LAUNCHD", TIL_Key_LaunchD},
		{"LAUNCHE", TIL_Key_LaunchE},
		{"LAUNCHF", TIL_Key_LaunchF},
		{"MEDIALAST", TIL_Key_MediaLast},
		{NULL, TIL_Key_unknown},
	};

	return (TIL_Keycode) map (keyMap, escaped);
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
	TIL_CmdID id = TIL_Cmd_Unknown;
	if (words[0] != NULL)
		id = (TIL_CmdID) map (cmdMap, words[0]);

	switch (id)
	{
	case TIL_Cmd_Move:
		{
			Mapping entityMap[] = {
				{"row", TIL_Cmd_Move_Row},
				{"line", TIL_Cmd_Move_Line},
				{"col", TIL_Cmd_Move_Column},
				{"char", TIL_Cmd_Move_Character},
				{"word", TIL_Cmd_Move_Word},
				{"sent", TIL_Cmd_Move_Sentence},
				{"block", TIL_Cmd_Move_Block},
				{"par", TIL_Cmd_Move_Paragraph},
				{"page", TIL_Cmd_Move_Page},
				{"doc", TIL_Cmd_Move_Doc},
				{NULL, -1},
			};
			Mapping flagsMap[] = {
				{"back", TIL_Cmd_Move_Backwards},
				{"end", TIL_Cmd_Move_EndOf},
				{"abs", TIL_Cmd_Move_Absolute},
				{"nowrap", TIL_Cmd_Move_NoLineWrap},
				{"screen", TIL_Cmd_Move_Screen},
				{NULL, 0},
			};

			int i=1;
			TIL_Cmd_Move_Args args = { TIL_Cmd_Move_Row, 1, 0 };

			if (words[i] == NULL)
				goto out;

			/* try to read the entity count */
			if (sscanf (words[i], "%d", &args.count) > 0)
				i++;

			if (words[i] == NULL)
				goto out;
				
			args.entity = (TIL_Cmd_Move_Entity) map (entityMap, words[i]);
			i++;

			if (words[i] != NULL)
			{
				/* the flags */
				gchar **flags = g_strsplit (words[i], "|", -1);
				gboolean error = FALSE;
				for (int j = 0; flags[j] != NULL; j++)
				{
					int flag = map (flagsMap, flags[j]);
					if (flag == 0)
						error = TRUE;
					else
						args.flags |= flag;
				}
				g_strfreev (flags);
				if (error)
					goto out;
				/* if there are additional parameters, go out */
				if (words[i+1] != NULL)
					goto out;
			}

			/* allocate the command and init it with the args */
			cmd = til_createCmd (id, &args, sizeof (args));
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
			cmd = til_createCmd (id, &args, sizeof (args));
		}
		break;

	case TIL_Cmd_Search:
		break;
	case TIL_Cmd_Delete:

		/* delete takes no args */
		if (words[1] != NULL)
			goto out;

		cmd = til_createCmd (id, NULL, 0);
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
			cmd->id = id;
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
	case TIL_Cmd_Unknown:
		goto out;
	}

  out:
	g_strfreev (words);
	return cmd;
}

GSList *
parseTestFile (gchar * filename)
{
	/* open test file */
	GIOChannel *testFile = g_io_channel_new_file (filename, "r", NULL);
	if (testFile == NULL)
		return NULL;

	GSList *testPairs = NULL;
	GString *line = g_string_new(NULL);
	int linecount = 0;

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
		if (strlen (line->str) == 0)
			continue;

		/* initialize test pair */
		TestPair *testPair = g_malloc (sizeof (TestPair));
		testPair->linenr = linecount;
		testPair->line = g_strdup (line->str);
		testPair->keyevent = NULL;
		testPair->commands = NULL;

		TIL_Keyevent *keyevent = NULL;
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
			default:
				break;
			}
			i++;
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
		int numCmds = 0;
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
			cmds = g_slist_append (cmds, cmd);
			numCmds++;
		}

		/*
		 * BUILD THE TEST PAIR
		 */
		testPair->keyevent = keyevent;
		testPair->commands = g_malloc (sizeof (TIL_Cmd *) * (numCmds + 1));
		GSList *work = cmds;
		for (int j = 0; j < numCmds; j++)
		{
			testPair->commands[j] = work->data;
			work = g_slist_next (work);
		}
		testPair->commands[numCmds] = NULL;

		g_strfreev (fields);
		g_slist_free (cmds);
		testPairs = g_slist_append (testPairs, testPair);
		continue;

	  parseError:
		g_free (keyevent);
		testPairs = g_slist_append (testPairs, testPair);
	}							/* end while */

	/* cleanup */
	g_io_channel_shutdown (testFile, FALSE, NULL);
	g_io_channel_unref (testFile);
	g_string_free (line, FALSE);

	return testPairs;
}

void
deleteTestPairs (GSList *testPairs)
{
	for (; testPairs != NULL; testPairs = g_slist_next (testPairs))
	{
		TestPair *tp = (TestPair *) testPairs->data;
		g_free (tp->line);
		g_free (tp->keyevent);
		g_strfreev ((gchar **) tp->commands);
		g_free (tp);
		testPairs = g_slist_delete_link (testPairs, testPairs);
	}
}
