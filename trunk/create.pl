#!/usr/bin/perl

my $filename = $ARGV[0];
open FILE,'>',$filename;
$,="\n";
print FILE ("/*  $filename",
" *",
" *  This file is part of the Text Input Layer (TIL).",
" *  Copyright (C) 2005 Pascal Maillard <pascalmaillard\@web.de>",
" *",
" *  The Text Input Layer is free software; you can redistribute it and/or",
" *  modify it under the terms of the GNU Library General Public",
" *  License as published by the Free Software Foundation; either",
" *  version 2 of the License, or (at your option) any later version.",
" *",
" *  The Text Input Layer is distributed in the hope that it will be useful,",
" *  but WITHOUT ANY WARRANTY; without even the implied warranty of",
" *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU",
" *  Library General Public License for more details.",
" *",
" *  You should have received a copy of the GNU Library General Public License",
" *  along with this library; see the file COPYING.LIB.  If not, write to",
" *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,",
" *  Boston, MA 02111-1307, USA.",
" **/\n\n");
if($filename =~ /\.h$/) {
	my $tag = '_'.uc($filename).'_';
	$tag =~ s/\./_/g;
	print FILE "#ifndef $tag",
	"#define $tag",
	"",
	"#endif";
}
close FILE;
system("gvim $filename");
