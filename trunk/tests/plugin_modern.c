/*  til.c
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

void
processEvent_test ()
{
	/* register two views */
	TIL_View view1, view2;
	CU_ASSERT (til_registerView (&view1, NULL));
	CU_ASSERT (til_registerView (&view2, NULL));

	/* send some printable characters */
	TIL_Keyevent *pEvent = g_malloc (sizeof (TIL_Keyevent) + 2);
	pEvent->type = TIL_Event_Pressed;
	pEvent->autorep = FALSE;
	pEvent->modifiers = 0;
	pEvent->keycode = TIL_Key_A;
	pEvent->text[0] = 'A';
	pEvent->text[1] = '\0';
	TIL_Cmd **cmds = NULL;
	CU_ASSERT (til_processEvent (view1, pEvent, &cmds));
	CU_ASSERT (cmds != NULL);
	if (cmds != NULL)
	{
		CU_ASSERT (cmds[0] != NULL)
		CU_ASSERT (cmds[1] == NULL);
		if (cmds[0] != NULL)
		{
			CU_ASSERT (cmds[0]->id == TIL_Cmd_Replace);
			CU_ASSERT (cmds[0]->args[0] == 'A' && cmds[0]->args[1] == '\0');
		}
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
