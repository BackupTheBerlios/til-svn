/*  keyevent.h
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

#ifndef _KEYEVENT_H_
#define _KEYEVENT_H_

#include <glib.h>

typedef enum _TIL_KeyeventType
{
	TIL_Event_Pressed,
	TIL_Event_Released,
} TIL_KeyeventType;

typedef enum _TIL_Keycode
{
	TIL_Key_Escape = 0x1000,
	TIL_Key_Tab = 0x1001,
	TIL_Key_Backtab = 0x1002,
	TIL_Key_BackTab = TIL_Key_Backtab,
	TIL_Key_Backspace = 0x1003,
	TIL_Key_BackSpace = TIL_Key_Backspace,
	TIL_Key_Return = 0x1004,
	TIL_Key_Enter = 0x1005,
	TIL_Key_Insert = 0x1006,
	TIL_Key_Delete = 0x1007,
	TIL_Key_Pause = 0x1008,
	TIL_Key_Print = 0x1009,
	TIL_Key_SysReq = 0x100a,
	TIL_Key_Clear = 0x100b,
	TIL_Key_Home = 0x1010,
	TIL_Key_End = 0x1011,
	TIL_Key_Left = 0x1012,
	TIL_Key_Up = 0x1013,
	TIL_Key_Right = 0x1014,
	TIL_Key_Down = 0x1015,
	TIL_Key_Prior = 0x1016,
	TIL_Key_PageUp = TIL_Key_Prior,
	TIL_Key_Next = 0x1017,
	TIL_Key_PageDown = TIL_Key_Next,
	TIL_Key_Shift = 0x1020,
	TIL_Key_Control = 0x1021,
	TIL_Key_Meta = 0x1022,
	TIL_Key_Alt = 0x1023,
	TIL_Key_CapsLock = 0x1024,
	TIL_Key_NumLock = 0x1025,
	TIL_Key_ScrollLock = 0x1026,
	TIL_Key_F1 = 0x1030,
	TIL_Key_F2 = 0x1031,
	TIL_Key_F3 = 0x1032,
	TIL_Key_F4 = 0x1033,
	TIL_Key_F5 = 0x1034,
	TIL_Key_F6 = 0x1035,
	TIL_Key_F7 = 0x1036,
	TIL_Key_F8 = 0x1037,
	TIL_Key_F9 = 0x1038,
	TIL_Key_F10 = 0x1039,
	TIL_Key_F11 = 0x103a,
	TIL_Key_F12 = 0x103b,
	TIL_Key_F13 = 0x103c,
	TIL_Key_F14 = 0x103d,
	TIL_Key_F15 = 0x103e,
	TIL_Key_F16 = 0x103f,
	TIL_Key_F17 = 0x1040,
	TIL_Key_F18 = 0x1041,
	TIL_Key_F19 = 0x1042,
	TIL_Key_F20 = 0x1043,
	TIL_Key_F21 = 0x1044,
	TIL_Key_F22 = 0x1045,
	TIL_Key_F23 = 0x1046,
	TIL_Key_F24 = 0x1047,
	TIL_Key_F25 = 0x1048,
	TIL_Key_F26 = 0x1049,
	TIL_Key_F27 = 0x104a,
	TIL_Key_F28 = 0x104b,
	TIL_Key_F29 = 0x104c,
	TIL_Key_F30 = 0x104d,
	TIL_Key_F31 = 0x104e,
	TIL_Key_F32 = 0x104f,
	TIL_Key_F33 = 0x1050,
	TIL_Key_F34 = 0x1051,
	TIL_Key_F35 = 0x1052,
	TIL_Key_Super_L = 0x1053,
	TIL_Key_Super_R = 0x1054,
	TIL_Key_Menu = 0x1055,
	TIL_Key_Hyper_L = 0x1056,
	TIL_Key_Hyper_R = 0x1057,
	TIL_Key_Help = 0x1058,
	TIL_Key_Direction_L = 0x1059,
	TIL_Key_Direction_R = 0x1060,
	TIL_Key_Space = 0x20,
	TIL_Key_Any = TIL_Key_Space,
	TIL_Key_Exclam = 0x21,
	TIL_Key_QuoteDbl = 0x22,
	TIL_Key_NumberSign = 0x23,
	TIL_Key_Dollar = 0x24,
	TIL_Key_Percent = 0x25,
	TIL_Key_Ampersand = 0x26,
	TIL_Key_Apostrophe = 0x27,
	TIL_Key_ParenLeft = 0x28,
	TIL_Key_ParenRight = 0x29,
	TIL_Key_Asterisk = 0x2a,
	TIL_Key_Plus = 0x2b,
	TIL_Key_Comma = 0x2c,
	TIL_Key_Minus = 0x2d,
	TIL_Key_Period = 0x2e,
	TIL_Key_Slash = 0x2f,
	TIL_Key_0 = 0x30,
	TIL_Key_1 = 0x31,
	TIL_Key_2 = 0x32,
	TIL_Key_3 = 0x33,
	TIL_Key_4 = 0x34,
	TIL_Key_5 = 0x35,
	TIL_Key_6 = 0x36,
	TIL_Key_7 = 0x37,
	TIL_Key_8 = 0x38,
	TIL_Key_9 = 0x39,
	TIL_Key_Colon = 0x3a,
	TIL_Key_Semicolon = 0x3b,
	TIL_Key_Less = 0x3c,
	TIL_Key_Equal = 0x3d,
	TIL_Key_Greater = 0x3e,
	TIL_Key_Question = 0x3f,
	TIL_Key_At = 0x40,
	TIL_Key_A = 0x41,
	TIL_Key_B = 0x42,
	TIL_Key_C = 0x43,
	TIL_Key_D = 0x44,
	TIL_Key_E = 0x45,
	TIL_Key_F = 0x46,
	TIL_Key_G = 0x47,
	TIL_Key_H = 0x48,
	TIL_Key_I = 0x49,
	TIL_Key_J = 0x4a,
	TIL_Key_K = 0x4b,
	TIL_Key_L = 0x4c,
	TIL_Key_M = 0x4d,
	TIL_Key_N = 0x4e,
	TIL_Key_O = 0x4f,
	TIL_Key_P = 0x50,
	TIL_Key_Q = 0x51,
	TIL_Key_R = 0x52,
	TIL_Key_S = 0x53,
	TIL_Key_T = 0x54,
	TIL_Key_U = 0x55,
	TIL_Key_V = 0x56,
	TIL_Key_W = 0x57,
	TIL_Key_X = 0x58,
	TIL_Key_Y = 0x59,
	TIL_Key_Z = 0x5a,
	TIL_Key_BracketLeft = 0x5b,
	TIL_Key_Backslash = 0x5c,
	TIL_Key_BracketRight = 0x5d,
	TIL_Key_AsciiCircum = 0x5e,
	TIL_Key_Underscore = 0x5f,
	TIL_Key_QuoteLeft = 0x60,
	TIL_Key_BraceLeft = 0x7b,
	TIL_Key_Bar = 0x7c,
	TIL_Key_BraceRight = 0x7d,
	TIL_Key_AsciiTilde = 0x7e,
	TIL_Key_nobreakspace = 0x0a0,
	TIL_Key_exclamdown = 0x0a1,
	TIL_Key_cent = 0x0a2,
	TIL_Key_sterling = 0x0a3,
	TIL_Key_currency = 0x0a4,
	TIL_Key_yen = 0x0a5,
	TIL_Key_brokenbar = 0x0a6,
	TIL_Key_section = 0x0a7,
	TIL_Key_diaeresis = 0x0a8,
	TIL_Key_copyright = 0x0a9,
	TIL_Key_ordfeminine = 0x0aa,
	TIL_Key_guillemotleft = 0x0ab,
	TIL_Key_notsign = 0x0ac,
	TIL_Key_hyphen = 0x0ad,
	TIL_Key_registered = 0x0ae,
	TIL_Key_macron = 0x0af,
	TIL_Key_degree = 0x0b0,
	TIL_Key_plusminus = 0x0b1,
	TIL_Key_twosuperior = 0x0b2,
	TIL_Key_threesuperior = 0x0b3,
	TIL_Key_acute = 0x0b4,
	TIL_Key_mu = 0x0b5,
	TIL_Key_paragraph = 0x0b6,
	TIL_Key_periodcentered = 0x0b7,
	TIL_Key_cedilla = 0x0b8,
	TIL_Key_onesuperior = 0x0b9,
	TIL_Key_masculine = 0x0ba,
	TIL_Key_guillemotright = 0x0bb,
	TIL_Key_onequarter = 0x0bc,
	TIL_Key_onehalf = 0x0bd,
	TIL_Key_threequarters = 0x0be,
	TIL_Key_questiondown = 0x0bf,
	TIL_Key_Agrave = 0x0c0,
	TIL_Key_Aacute = 0x0c1,
	TIL_Key_Acircumflex = 0x0c2,
	TIL_Key_Atilde = 0x0c3,
	TIL_Key_Adiaeresis = 0x0c4,
	TIL_Key_Aring = 0x0c5,
	TIL_Key_AE = 0x0c6,
	TIL_Key_Ccedilla = 0x0c7,
	TIL_Key_Egrave = 0x0c8,
	TIL_Key_Eacute = 0x0c9,
	TIL_Key_Ecircumflex = 0x0ca,
	TIL_Key_Ediaeresis = 0x0cb,
	TIL_Key_Igrave = 0x0cc,
	TIL_Key_Iacute = 0x0cd,
	TIL_Key_Icircumflex = 0x0ce,
	TIL_Key_Idiaeresis = 0x0cf,
	TIL_Key_ETH = 0x0d0,
	TIL_Key_Ntilde = 0x0d1,
	TIL_Key_Ograve = 0x0d2,
	TIL_Key_Oacute = 0x0d3,
	TIL_Key_Ocircumflex = 0x0d4,
	TIL_Key_Otilde = 0x0d5,
	TIL_Key_Odiaeresis = 0x0d6,
	TIL_Key_multiply = 0x0d7,
	TIL_Key_Ooblique = 0x0d8,
	TIL_Key_Ugrave = 0x0d9,
	TIL_Key_Uacute = 0x0da,
	TIL_Key_Ucircumflex = 0x0db,
	TIL_Key_Udiaeresis = 0x0dc,
	TIL_Key_Yacute = 0x0dd,
	TIL_Key_THORN = 0x0de,
	TIL_Key_ssharp = 0x0df,
	TIL_Key_agrave = 0x0e0,
	TIL_Key_aacute = 0x0e1,
	TIL_Key_acircumflex = 0x0e2,
	TIL_Key_atilde = 0x0e3,
	TIL_Key_adiaeresis = 0x0e4,
	TIL_Key_aring = 0x0e5,
	TIL_Key_ae = 0x0e6,
	TIL_Key_ccedilla = 0x0e7,
	TIL_Key_egrave = 0x0e8,
	TIL_Key_eacute = 0x0e9,
	TIL_Key_ecircumflex = 0x0ea,
	TIL_Key_ediaeresis = 0x0eb,
	TIL_Key_igrave = 0x0ec,
	TIL_Key_iacute = 0x0ed,
	TIL_Key_icircumflex = 0x0ee,
	TIL_Key_idiaeresis = 0x0ef,
	TIL_Key_eth = 0x0f0,
	TIL_Key_ntilde = 0x0f1,
	TIL_Key_ograve = 0x0f2,
	TIL_Key_oacute = 0x0f3,
	TIL_Key_ocircumflex = 0x0f4,
	TIL_Key_otilde = 0x0f5,
	TIL_Key_odiaeresis = 0x0f6,
	TIL_Key_division = 0x0f7,
	TIL_Key_oslash = 0x0f8,
	TIL_Key_ugrave = 0x0f9,
	TIL_Key_uacute = 0x0fa,
	TIL_Key_ucircumflex = 0x0fb,
	TIL_Key_udiaeresis = 0x0fc,
	TIL_Key_yacute = 0x0fd,
	TIL_Key_thorn = 0x0fe,
	TIL_Key_ydiaeresis = 0x0ff,
	TIL_Key_Back = 0x1061,
	TIL_Key_Forward = 0x1062,
	TIL_Key_Stop = 0x1063,
	TIL_Key_Refresh = 0x1064,
	TIL_Key_VolumeDown = 0x1070,
	TIL_Key_VolumeMute = 0x1071,
	TIL_Key_VolumeUp = 0x1072,
	TIL_Key_BassBoost = 0x1073,
	TIL_Key_BassUp = 0x1074,
	TIL_Key_BassDown = 0x1075,
	TIL_Key_TrebleUp = 0x1076,
	TIL_Key_TrebleDown = 0x1077,
	TIL_Key_MediaPlay = 0x1080,
	TIL_Key_MediaStop = 0x1081,
	TIL_Key_MediaPrev = 0x1082,
	TIL_Key_MediaNext = 0x1083,
	TIL_Key_MediaRecord = 0x1084,
	TIL_Key_HomePage = 0x1090,
	TIL_Key_Favorites = 0x1091,
	TIL_Key_Search = 0x1092,
	TIL_Key_Standby = 0x1093,
	TIL_Key_OpenUrl = 0x1094,
	TIL_Key_LaunchMail = 0x10a0,
	TIL_Key_LaunchMedia = 0x10a1,
	TIL_Key_Launch0 = 0x10a2,
	TIL_Key_Launch1 = 0x10a3,
	TIL_Key_Launch2 = 0x10a4,
	TIL_Key_Launch3 = 0x10a5,
	TIL_Key_Launch4 = 0x10a6,
	TIL_Key_Launch5 = 0x10a7,
	TIL_Key_Launch6 = 0x10a8,
	TIL_Key_Launch7 = 0x10a9,
	TIL_Key_Launch8 = 0x10aa,
	TIL_Key_Launch9 = 0x10ab,
	TIL_Key_LaunchA = 0x10ac,
	TIL_Key_LaunchB = 0x10ad,
	TIL_Key_LaunchC = 0x10ae,
	TIL_Key_LaunchD = 0x10af,
	TIL_Key_LaunchE = 0x10b0,
	TIL_Key_LaunchF = 0x10b1,
	TIL_Key_MediaLast = 0x1fff,
	TIL_Key_unknown = 0xffff,
} TIL_Keycode;

// we don't want the til plugins to use the modifiers - these are reserved for the application
/*enum TIL_Modifier {
	TIL_Mod_Shift = 0x0100,
	TIL_Mod_Control = 0x0200,
	TIL_Mod_Alt = 0x0400,
	TIL_Mod_Meta = 0x0800,
};*/

typedef struct
{
	TIL_KeyeventType type;
	gboolean autorep;
	//gint modifiers;
	TIL_Keycode keycode;
	gchar *text;
} TIL_Keyevent;

#endif
