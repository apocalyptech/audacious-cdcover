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

#ifndef __CONFIGMGM_H__
#define __CONFIGMGM_H__

#include <gdk-pixbuf/gdk-pixbuf.h>

typedef enum {LOADSKIN_NORMAL,LOADSKIN_TEST} _loadskin_types;

typedef struct {
	gboolean save_window_pos;
	gint winpos_x,winpos_y;
	GSList *cover_searchpaths;
	GSList *cover_extensions;
	gboolean preserve_aspectratio;
	char *skin_path;
	int skin_width,skin_height,
	    skin_coverx,skin_covery,
	    skin_cover_width,skin_cover_height;
	GdkPixbuf *skin_base_custom_pixbuf;
	GdkPixbuf *skin_nocover_custom_pixbuf;
} t_cdcover_config;

typedef struct {
	gchar *skin_title,*skin_path,*skin_author;
} _skinsearch_entry;

void read_xmms_config();
void write_xmms_config();
void free_xmms_config();
t_cdcover_config *get_xmms_config();
void init_skin (gchar *skinpath,_loadskin_types loadtype);
GSList *search_skins ();

#endif

