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
	TIL_Cmd_Cursor,
	TIL_Cmd_Select,
	TIL_Cmd_Search,
	TIL_Cmd_Delete,
	TIL_Cmd_Insert,
	TIL_Cmd_Replace,
	TIL_Cmd_Copy,
	TIL_Cmd_Undo,
	TIL_Cmd_Redo,
	TIL_Cmd_Override,
	TIL_Cmd_Indent,
	TIL_Cmd_Complete,
	TIL_Cmd_Format,
	TIL_Cmd_Open,
	TIL_Cmd_Status,
	TIL_Cmd_Unknown = 0xffff,
} TIL_CmdID;


typedef struct
{
	size_t size;
	TIL_CmdID id;
	gchar args[];
} TIL_Cmd;

TIL_Cmd *til_createCmd (TIL_CmdID id, void * args, int size_of_args);

/*
 * "move" command
 */

typedef enum
{
	TIL_Cmd_Move_Row,
	TIL_Cmd_Move_Line,
	TIL_Cmd_Move_Column,
	TIL_Cmd_Move_Character,
	TIL_Cmd_Move_Word,
	TIL_Cmd_Move_Sentence,
	TIL_Cmd_Move_Block,
	TIL_Cmd_Move_Paragraph,
	TIL_Cmd_Move_Page,
	TIL_Cmd_Move_Doc,
} TIL_Cmd_Move_Entity;

#define TIL_Cmd_Move_Backwards    0x01
#define TIL_Cmd_Move_EndOf        0x02
#define TIL_Cmd_Move_Absolute     0x04
#define TIL_Cmd_Move_NoLineWrap   0x08
#define TIL_Cmd_Move_Screen       0x10

typedef struct 
{
	TIL_Cmd_Move_Entity entity;
	gint count;
	guint flags;
} TIL_Cmd_Move_Args;


/*
 * "change cursor" command
 */

typedef enum
{
	TIL_Cmd_Cursor_Line,
	TIL_Cmd_Cursor_Block,
} TIL_Cmd_Cursor_Shape;

typedef struct
{
	TIL_Cmd_Cursor_Shape shape;
} TIL_Cmd_Cursor_Args;

/*
 * "set selection" command
 */

typedef enum
{
	TIL_Cmd_Select_None,
	TIL_Cmd_Select_Default,
	TIL_Cmd_Select_Normal,
	TIL_Cmd_Select_Line,
	TIL_Cmd_Select_Block,
} TIL_Cmd_Select_Mode;

typedef struct
{
	TIL_Cmd_Select_Mode mode;
} TIL_Cmd_Select_Args;

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

/* same args as insert */
typedef TIL_Cmd_Insert_Args TIL_Cmd_Replace_Args;

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
 * "override" command
 */

/* passes a "gboolean" meaning override mode on or off */

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
