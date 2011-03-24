//
// xmms Plugin CDcover
// Copyright (C) 2003, Magnus Schmidt <xmms@27b-6.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef __UTIL_H__
#define __UTIL_H__

//#define __DEBUG__ 	  1
#define __DEBUG_GTK__	  1
#define __DEBUG_GENERAL__ 2
#define __DEBUG_XMMS__	  4
#define __DEBUG_FILE__	  8
#define __DEBUG_LEVEL__   15

#ifdef __DEBUG__
#define DPRINT(level,format,args...) if (level & __DEBUG_LEVEL__) { printf ("dbg CDcover ("__FILE__ ":%d): " format "\n", __LINE__ , ## args); }
#else
#define DPRINT(level,format,args...) ;
#endif

#endif
