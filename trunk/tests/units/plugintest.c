/*  plugintest.c
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
#include "../plugins/parse.h"

void
parse_test ()
{
	GSList *testPairs = parseTestFile ("tests/units/parsetest.txt");
	CU_ASSERT_FATAL (g_slist_length (testPairs) == 2);
	TestPair *tp = NULL;
	GSList *work = testPairs;

	/* press	:: A	::		:: replace text A */
	tp = work->data;
	CU_ASSERT (tp != NULL);
	if (tp != NULL)
	{
		CU_ASSERT (tp->linenr == 1);
		CU_ASSERT (g_strcasecmp (tp->line,
					"press\t:: A\t::\t:: replace text A") == 0);
		CU_ASSERT (tp->keyevent != NULL);
		CU_ASSERT (tp->keyevent->type == TIL_Event_Pressed);
		CU_ASSERT (tp->keyevent->autorep == 0);
		CU_ASSERT (tp->keyevent->modifiers == 0);
		CU_ASSERT (g_strcasecmp (tp->keyevent->text, "A") == 0);

		CU_ASSERT (tp->commands != NULL);
		CU_ASSERT (tp->commands[0] != NULL);
		CU_ASSERT (tp->commands[1] == NULL);
		if (tp->commands[0] != NULL)
		{
			TIL_Cmd * cmd = tp->commands[0];
			CU_ASSERT (cmd->id == TIL_Cmd_Replace);
			TIL_Cmd_Replace_Args *args = (TIL_Cmd_Replace_Args*) cmd->args;
			CU_ASSERT (args->clipboard == FALSE);
			CU_ASSERT (g_strcasecmp (args->text, "A") == 0);
		}
	}

	/* aolerch */
	work = g_slist_next (work);
	tp = work->data;
	CU_ASSERT (tp != NULL);
	if (tp != NULL)
	{
		CU_ASSERT (tp->linenr == 2);
		CU_ASSERT (g_strcasecmp (tp->line, "aolerch") == 0);
		CU_ASSERT (tp->keyevent == NULL);
		CU_ASSERT (tp->commands == NULL);
	}

	deleteTestPairs (testPairs);
}

static CU_TestInfo tests[] = {
	{"parsing test file", parse_test},
	CU_TEST_INFO_NULL,
};

CU_SuiteInfo tests_plugintest_suiteInfo = { "Plugintest test suite", NULL, NULL, tests };
