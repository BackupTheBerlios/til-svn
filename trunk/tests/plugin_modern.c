/*  plugin_modern.c
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

#include <CUnit/CUnit.h>
#include <til.h>
#include <glib.h>

int
plugin_modern_init_tests ()
{
	if (!til_init ())
		return 1;
	const gchar *pluginID = NULL;
	if (!til_loadPlugin ("plugins/modern/modern", &pluginID))
		return 1;
	til_setDefaultPlugin (pluginID);
	return 0;
}

int
plugin_modern_cleanup_tests ()
{
	return til_cleanup ()? 0 : 1;
}

void
view_test ()
{
	/* register and unregister some views */
	TIL_View view;
	CU_ASSERT (til_registerView (&view, NULL));
	CU_ASSERT (til_registerView (&view, NULL));
	CU_ASSERT (til_registerView (&view, NULL));
	CU_ASSERT (til_unregisterView (view));
	CU_ASSERT (til_registerView (&view, NULL));
	CU_ASSERT (til_unregisterView (view));
	CU_ASSERT (til_unregisterAllViews ());
}

gint
arrayLength (TIL_Cmd ** array)
{
	if (array == NULL)
		return -1;
	int len = 0;
	while (array[len] != NULL)
	{
		len++;
	}
	return len;
}

/* nice hack to save typing, see below */
#define IFEQUAL(actual,expected) CU_ASSERT_EQUAL (actual,expected); if (actual == expected)

void
processEvent_test ()
{
	/* register two views */
	TIL_View view1, view2;
	CU_ASSERT (til_registerView (&view1, NULL));
	CU_ASSERT (til_registerView (&view2, NULL));

	/* send some printable characters */
	TIL_Keyevent *pEvent = g_malloc (sizeof (TIL_Keyevent) + 3);
	pEvent->type = TIL_Event_Pressed;
	pEvent->autorep = FALSE;
	pEvent->modifiers = 0;
	pEvent->keycode = TIL_Key_A;
	pEvent->text[0] = 'A';
	pEvent->text[1] = '\0';
	TIL_Cmd **cmds = NULL;
	CU_ASSERT (til_processEvent (view1, pEvent, &cmds));
	IFEQUAL (arrayLength (cmds), 1)
	{
		CU_ASSERT (cmds[0]->id == TIL_Cmd_Replace);
		CU_ASSERT (cmds[0]->args[0] == 'A' && cmds[0]->args[1] == '\0');
	}
	til_freeCmdArray (cmds);
	cmds = NULL;

	pEvent->autorep = TRUE;
	CU_ASSERT (til_processEvent (view1, pEvent, &cmds));
	IFEQUAL (arrayLength (cmds), 1)
	{
		CU_ASSERT (cmds[0]->id == TIL_Cmd_Replace);
		CU_ASSERT (cmds[0]->args[0] == 'A' && cmds[0]->args[1] == '\0');
	}
	til_freeCmdArray (cmds);
	cmds = NULL;
	
	/* character ä (a umlaut) */
	pEvent->autorep = FALSE;
	pEvent->text[0] = (gchar)0xc3;
	pEvent->text[1] = (gchar)0xa4;
	pEvent->text[2] = '\0';
	CU_ASSERT (til_processEvent (view1, pEvent, &cmds));
	IFEQUAL (arrayLength (cmds), 1)
	{
		CU_ASSERT (cmds[0]->id == TIL_Cmd_Replace);
		CU_ASSERT (cmds[0]->args[0] == (gchar)0xc3 && cmds[0]->args[1] == (gchar)0xa4
				&& cmds[0]->args[2] == '\0');
	}
	til_freeCmdArray (cmds);
	cmds = NULL;

	/* move cursor */
	TIL_Cmd_Move_Args *args = NULL;
	pEvent->text[0] = '\0';
	pEvent->keycode = TIL_Key_Left;
	CU_ASSERT (til_processEvent (view1, pEvent, &cmds));
	IFEQUAL (arrayLength (cmds), 1)
	{
		CU_ASSERT (cmds[0]->id == TIL_Cmd_Move);
		args = (TIL_Cmd_Move_Args *) cmds[0]->args;
		CU_ASSERT_EQUAL (args->entity, TIL_Cmd_Move_Character);
		CU_ASSERT_EQUAL (args->count, 1);
		CU_ASSERT_EQUAL (args->flags, TIL_Cmd_Move_Previous | TIL_Cmd_Move_Relative
				| TIL_Cmd_Move_Linewrap);
	}
	til_freeCmdArray (cmds);
	cmds = NULL;

	pEvent->modifiers = TIL_Mod_Control;
	CU_ASSERT (til_processEvent (view1, pEvent, &cmds));
	IFEQUAL (arrayLength (cmds), 1)
	{
		CU_ASSERT (cmds[0]->id == TIL_Cmd_Move);
		args = (TIL_Cmd_Move_Args *) cmds[0]->args;
		CU_ASSERT_EQUAL (args->entity, TIL_Cmd_Move_Word);
		CU_ASSERT_EQUAL (args->count, 1);
		CU_ASSERT_EQUAL (args->flags, TIL_Cmd_Move_Previous | TIL_Cmd_Move_Relative
				| TIL_Cmd_Move_Linewrap);
	}
	til_freeCmdArray (cmds);
	cmds = NULL;

	pEvent->keycode = TIL_Key_Right;
	pEvent->modifiers = 0;
	CU_ASSERT (til_processEvent (view1, pEvent, &cmds));
	IFEQUAL (arrayLength (cmds), 1)
	{
		CU_ASSERT (cmds[0]->id == TIL_Cmd_Move);
		args = (TIL_Cmd_Move_Args *) cmds[0]->args;
		CU_ASSERT_EQUAL (args->entity, TIL_Cmd_Move_Character);
		CU_ASSERT_EQUAL (args->count, 1);
		CU_ASSERT_EQUAL (args->flags, TIL_Cmd_Move_Relative | TIL_Cmd_Move_Linewrap);
	}
	til_freeCmdArray (cmds);
	cmds = NULL;

	pEvent->modifiers = TIL_Mod_Control;
	CU_ASSERT (til_processEvent (view1, pEvent, &cmds));
	IFEQUAL (arrayLength (cmds), 1)
	{
		CU_ASSERT (cmds[0]->id == TIL_Cmd_Move);
		args = (TIL_Cmd_Move_Args *) cmds[0]->args;
		CU_ASSERT_EQUAL (args->entity, TIL_Cmd_Move_Word);
		CU_ASSERT_EQUAL (args->count, 1);
		CU_ASSERT_EQUAL (args->flags, TIL_Cmd_Move_Relative | TIL_Cmd_Move_Linewrap);
	}
	til_freeCmdArray (cmds);
	cmds = NULL;

	pEvent->keycode = TIL_Key_Up;
	pEvent->modifiers = 0;
	CU_ASSERT (til_processEvent (view1, pEvent, &cmds));
	IFEQUAL (arrayLength (cmds), 1)
	{
		CU_ASSERT (cmds[0]->id == TIL_Cmd_Move);
		args = (TIL_Cmd_Move_Args *) cmds[0]->args;
		CU_ASSERT_EQUAL (args->entity, TIL_Cmd_Move_Row);
		CU_ASSERT_EQUAL (args->count, 1);
		CU_ASSERT_EQUAL (args->flags, TIL_Cmd_Move_Previous | TIL_Cmd_Move_Relative
				| TIL_Cmd_Move_Virtual);
	}
	til_freeCmdArray (cmds);
	cmds = NULL;

	pEvent->keycode = TIL_Key_Down;
	CU_ASSERT (til_processEvent (view1, pEvent, &cmds));
	IFEQUAL (arrayLength (cmds), 1)
	{
		CU_ASSERT_EQUAL (cmds[0]->id, TIL_Cmd_Move);
		args = (TIL_Cmd_Move_Args *) cmds[0]->args;
		CU_ASSERT_EQUAL (args->entity, TIL_Cmd_Move_Row);
		CU_ASSERT_EQUAL (args->count, 1);
		CU_ASSERT_EQUAL (args->flags, TIL_Cmd_Move_Relative | TIL_Cmd_Move_Virtual);
	}
	til_freeCmdArray (cmds);
	cmds = NULL;

	/* delete and backspace */
	pEvent->keycode = TIL_Key_Delete;
	CU_ASSERT (til_processEvent (view1, pEvent, &cmds));
	IFEQUAL (arrayLength (cmds), 3)
	{
		CU_ASSERT_EQUAL (cmds[0]->id, TIL_Cmd_Select);
		CU_ASSERT_EQUAL (* (TIL_Cmd_Select_Modes *) cmds[0]->args, TIL_Cmd_Select_Normal)

		CU_ASSERT_EQUAL (cmds[1]->id, TIL_Cmd_Move);
		args = (TIL_Cmd_Move_Args *) cmds[1]->args;
		CU_ASSERT_EQUAL (args->entity, TIL_Cmd_Move_Character);
		CU_ASSERT_EQUAL (args->count, 1);
		CU_ASSERT_EQUAL (args->flags, TIL_Cmd_Move_Relative | TIL_Cmd_Move_Linewrap);

		CU_ASSERT_EQUAL (cmds[2]->id, TIL_Cmd_Delete);
	}
	til_freeCmdArray (cmds);
	cmds = NULL;

	pEvent->modifiers = TIL_Mod_Control;
	CU_ASSERT (til_processEvent (view1, pEvent, &cmds));
	IFEQUAL (arrayLength (cmds), 3)
	{
		CU_ASSERT_EQUAL (cmds[0]->id, TIL_Cmd_Select);
		CU_ASSERT_EQUAL (* (TIL_Cmd_Select_Modes *) cmds[0]->args, TIL_Cmd_Select_Normal)

		CU_ASSERT_EQUAL (cmds[1]->id, TIL_Cmd_Move);
		args = (TIL_Cmd_Move_Args *) cmds[1]->args;
		CU_ASSERT_EQUAL (args->entity, TIL_Cmd_Move_Word);
		CU_ASSERT_EQUAL (args->count, 1);
		CU_ASSERT_EQUAL (args->flags, TIL_Cmd_Move_Relative | TIL_Cmd_Move_Linewrap);

		CU_ASSERT_EQUAL (cmds[2]->id, TIL_Cmd_Delete);
	}
	til_freeCmdArray (cmds);
	cmds = NULL;

	pEvent->keycode = TIL_Key_Backspace;
	pEvent->modifiers = 0;
	CU_ASSERT (til_processEvent (view1, pEvent, &cmds));
	IFEQUAL (arrayLength (cmds), 3)
	{
		CU_ASSERT_EQUAL (cmds[0]->id, TIL_Cmd_Select);
		CU_ASSERT_EQUAL (* (TIL_Cmd_Select_Modes *) cmds[0]->args, TIL_Cmd_Select_Normal)

		CU_ASSERT_EQUAL (cmds[1]->id, TIL_Cmd_Move);
		args = (TIL_Cmd_Move_Args *) cmds[1]->args;
		CU_ASSERT_EQUAL (args->entity, TIL_Cmd_Move_Character);
		CU_ASSERT_EQUAL (args->count, 1);
		CU_ASSERT_EQUAL (args->flags, TIL_Cmd_Move_Previous | TIL_Cmd_Move_Relative
				| TIL_Cmd_Move_Linewrap);

		CU_ASSERT_EQUAL (cmds[2]->id, TIL_Cmd_Delete);
	}
	til_freeCmdArray (cmds);
	cmds = NULL;

	pEvent->modifiers = TIL_Mod_Control;
	CU_ASSERT (til_processEvent (view1, pEvent, &cmds));
	IFEQUAL (arrayLength (cmds), 3)
	{
		CU_ASSERT_EQUAL (cmds[0]->id, TIL_Cmd_Select);
		CU_ASSERT_EQUAL (* (TIL_Cmd_Select_Modes *) cmds[0]->args, TIL_Cmd_Select_Normal)

		CU_ASSERT_EQUAL (cmds[1]->id, TIL_Cmd_Move);
		args = (TIL_Cmd_Move_Args *) cmds[1]->args;
		CU_ASSERT_EQUAL (args->entity, TIL_Cmd_Move_Word);
		CU_ASSERT_EQUAL (args->count, 1);
		CU_ASSERT_EQUAL (args->flags, TIL_Cmd_Move_Previous | TIL_Cmd_Move_Relative
				| TIL_Cmd_Move_Linewrap);

		CU_ASSERT_EQUAL (cmds[2]->id, TIL_Cmd_Delete);
	}
	til_freeCmdArray (cmds);
	cmds = NULL;

	til_unregisterAllViews ();
}

static CU_TestInfo tests[] = {
	{"manage views", view_test},
	{"process events", processEvent_test},
	CU_TEST_INFO_NULL,
};

CU_SuiteInfo tests_plugin_modern_suiteInfo = { "Modern plugin test suite", plugin_modern_init_tests, plugin_modern_cleanup_tests, tests };
