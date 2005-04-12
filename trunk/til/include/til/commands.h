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

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
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


typedef struct
{
	TIL_CmdID id;
	gchar args[];
} TIL_Cmd;


/*
 * "move" command
 */

typedef enum
{
	TIL_Cmd_Move_Row,
	TIL_Cmd_Move_Column,
	TIL_Cmd_Move_Character,
	TIL_Cmd_Move_Word,
	TIL_Cmd_Move_Sentence,
	TIL_Cmd_Move_Block,
	TIL_Cmd_Move_Paragraph,
} TIL_Cmd_Move_Entity;

#define TIL_Cmd_Move_Relative 1
#define TIL_Cmd_Move_Linewrap 2
#define TIL_Cmd_Move_Virtual  4

typedef struct 
{
	TIL_Cmd_Move_Entity entity;
	guint flags;
} TIL_Cmd_Move_Args;


/*
 * "change cursor" command
 */

/* TODO */

/*
 * "set selection" command
 */

typedef enum
{
	TIL_Cmd_Select_None,
	TIL_Cmd_Select_Normal,
	TIL_Cmd_Select_Line,
	TIL_Cmd_Select_Block,
} TIL_Cmd_Select_Modes;

/*
 * "delete" command
 */

/* no arguments, operates on the selected text */

/*
 * "insert" command
 */

typedef struct
{
	gboolean clipboard;
	gchar text[];
} TIL_Cmd_Insert_Args;

/*
 * "replace" command
 */

typedef struct
{
	gboolean clipboard;
	gchar text[];
} TIL_Cmd_Replace_Args;

/*
 * "copy" command
 */

/* no arguments, operates on the selected text */

/*
 * "undo" command
 */

/* passes a "gint" telling how many levels to undo */

/*
 * "redo" command
 */

/* passes a "gint" telling how many levels to redo */

/*
 * "indent" command
 */

/* TODO */

/*
 * "complete" command
 */

/* TODO */

/*
 * "format" command
 */

/* TODO */

/*
 * "open" command
 */

/* passes an UTF-8 string with the URL to open */

/*
 * "set status text" command
 */

/* TODO */

#ifdef __cplusplus
}
#endif

#endif
