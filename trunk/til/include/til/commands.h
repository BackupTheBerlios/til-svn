/*  commands.h
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

#ifndef _COMMANDS_H_
#define _COMMANDS_H_

typedef enum _TIL_CmdID
{
	TIL_Cmd_Move,
	TIL_Cmd_ChangeCursor,
	TIL_Cmd_SetSelection,
	TIL_Cmd_Search,
	TIL_Cmd_Delete,
	TIL_Cmd_Insert,
	TIL_Cmd_Replace,
	TIL_Cmd_Copy,
	TIL_Cmd_Undo,
	TIL_Cmd_Redo,
	TIL_Cmd_Indent,
	TIL_Cmd_Complete,
	TIL_Cmd_Format,
	TIL_Cmd_Open,
	TIL_Cmd_SetStatusText,
} TIL_CmdID;


typedef struct _TIL_Cmd
{
	TIL_CmdID id;
	gpointer *args;
} TIL_Cmd;


#endif
