/*  error.c
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

#include <glib.h>
#include <glib/gprintf.h>
#include <stdarg.h>

#include "error.h"

static gchar *_lastErrorMessage = NULL;
static gint _lastErrorCode = 0;

gboolean
error_init ()
{
	return TRUE;
}

gboolean
error_cleanup ()
{
	if (_lastErrorMessage != NULL)
	{
		g_free (_lastErrorMessage);
		_lastErrorMessage = NULL;
	}
	return TRUE;
}

void
setError (gint code, const gchar * message, ...)
{
	/* copy code */
	_lastErrorCode = code;
	/* free last message */
	if (_lastErrorMessage != NULL)
	{
		g_free (_lastErrorMessage);
		_lastErrorMessage = NULL;
	}
	/* create message */
	if (message != NULL)
	{
		va_list args;
		va_start (args, message);
		g_vasprintf (&_lastErrorMessage, message, args);
		va_end (args);
	}
}

gint
til_lastErrorCode ()
{
	return _lastErrorCode;
}

const gchar *
til_lastErrorMessage ()
{
	return _lastErrorMessage;
}
