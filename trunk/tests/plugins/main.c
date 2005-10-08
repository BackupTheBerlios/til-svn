/*  main.c
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
#include <stdio.h>
#include <string.h>

#define ERROR(msg) { error = g_strdup (msg); goto errorout; }

int
main (int argc, char ** argv)
{
	gchar *error = NULL;

	if (argc != 3)
		ERROR ("Syntax: plugintest PATH-TO-PLUGIN PATH-TO-TEST-FILE");

	char *pluginpath = argv[1];
	char *testfile = argv[2];
	/* initialize TIL */
	if (!til_init())
		ERROR ("Could not initialize TIL!");

	/* load the plugin */
	const gchar *pluginID = NULL;
	if (!til_loadPlugin (pluginpath, &pluginID))
		ERROR ("Could not load plugin!");

	TIL_View view;
	if (!til_registerView (&view, pluginID))
		ERROR ("Could not register any view!");

	/* parse the test file */
	GSList *testPairs = parseTestFile (testfile);
	if (testPairs == NULL)
		ERROR ("Could not open test file or test file empty!");
	
	/* check if there have been parse errors */
	GSList *work = testPairs;
	gboolean parseErrors = FALSE;
	while (work != NULL)
	{
		TestPair *tp = (TestPair *) work->data;
		if (tp->keyevent == NULL)
		{
			parseErrors = TRUE;
			break;
		}
		work = g_slist_next (work);
	}

	if (parseErrors)
	{
		/* print the lines that contain errors */
		while (work != NULL)
		{
			TestPair *tp = (TestPair *) work->data;
			if (tp->keyevent == NULL)
				printf ("%d:\t%s\n", tp->linenr, tp->line);
			work = g_slist_next (work);
		}
		ERROR ("There are errors in the input file (see above)!");
	}

	/* perform tests only if there are no errors in the input file */
	puts ("Performing tests, printing failures...");
	int failCount = 0;
	for (work = testPairs; work != NULL; work = g_slist_next (work))
	{
		TestPair *tp = work->data;
		TIL_Cmd **cmds1 = tp->commands, **cmds2 = NULL;
		if (til_processEvent (view, tp->keyevent, &cmds2))
		{
			/* compare commands */
			int i=0;
			gboolean equal = TRUE;
			for (; cmds1[i] != NULL && cmds2[i] != NULL; i++)
			{
				if (cmds1[i]->size != cmds2[i]->size ||
						memcmp (cmds1[i], cmds2[i], cmds1[i]->size) != 0)
				{
					equal = FALSE;
					break;
				}
			}
			if (cmds1[i] != NULL || cmds2[i] != NULL)
				equal = FALSE;

			if (!equal)
			{
				printf ("%d:\t%s\n", tp->linenr, tp->line);
				failCount++;
			}
		}
		else
		{
			error = g_strdup_printf ("Failure to process event\n"
					"%d:\t%s\n", tp->linenr, tp->line);
			deleteTestPairs (testPairs);
			goto errorout;
		}
	}

	if (failCount == 0)
		puts ("*** All tests have been successful!");
	else
		printf ("*** %d tests have failed\n", failCount);

	til_cleanup ();
	return 0;

errorout:
	til_cleanup ();
	printf ("FATAL ERROR: %s\nQuitting...", error);
	free (error);
	return 1;
}
