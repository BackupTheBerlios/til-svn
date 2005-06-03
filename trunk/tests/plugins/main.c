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

#define ERROR(msg) { error = msg; goto errorout; }

int
main (int argc, char ** argv)
{
	gchar *error = NULL;

	if (argc != 2)
		ERROR ("Syntax: plugintest PLUGIN");

	char *pluginpath = argv[1];
	/* initialize TIL */
	if (!til_init())
		ERROR ("Could not initialize TIL!");

	/* load the plugin */
	gchar *pluginID = NULL;
	if (!til_loadPlugin (pluginpath, &pluginID))
		ERROR ("Could not load plugin!");

	TIL_View view;
	if (!til_registerView (&view, pluginID))
		ERROR ("Could not register any view!");

	/* open the test file */
	if (!openTestFile (pluginpath))
		ERROR ("Could not open test file!");
	
	TestPair *testPair;
	while ((testPair = getNextTestPair()) != NULL)
	{
		TIL_Cmd **cmds1 = testPair->commands, **cmds2 = NULL;
		if (til_processEvent (view, testPair->keyevent, &cmds2))
		{
			/* compare commands */
			int i=0;
			for (; cmds1[i] != NULL && cmds2[i] != NULL; i++)
			{
				size_t size = cmds1[i]->size;
			}
		}
		else
			printf ("line %d: Could not process event\n", testPair->line);
	}

	return 0;

errorout:
	closeTestFile ();
	til_cleanup ();
	printf ("ERROR: %s\n", error);
	return 1;
	
}
