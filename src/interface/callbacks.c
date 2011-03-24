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

#include <config.h>		// created by configure, needed for version
#include <stdlib.h>		// free et all
#include <stdio.h>		// standard functions
#include <string.h>		// standard string functions
#include <gtk/gtk.h>		// gtk
#include "globals.h"		// some globals
#include "configmgm.h"		// plugin configuration
#include "callbacks.h"		// headers for this file
#include "interface.h"		// glade window definiton, generated
#include "support.h"		// glade help functions, generated



// The about dialog
GtkWidget *window_about = NULL;

// The config dialog
GtkWidget *window_config = NULL;
GList *searchpaths = NULL;
GList *extensions = NULL;
gint row_selected = -1;
gint row_count = -1;
gint skin_selected = -1;
gboolean old_preserve_aspectratio = FALSE;
gboolean dlgConfig_init = FALSE;

// Current filename playing, declared in mainwindow.c, used during config save
extern char *current_filename;

// Reload the skin if the a skin is selected
extern void mainwindow_loadskin (gchar *skinpath,_loadskin_types loadtype);



// --- handler functions
void interface_showabout() {
	if (window_about==NULL) {
		window_about = create_dlgAbout();
		gtk_widget_show_all (window_about);
	}
}

void interface_showconfig() {
	if (window_config==NULL) {
		window_config = create_dlgConfig();
		gtk_widget_show (window_config);
	}
}





// --- about dialog ---

// ***************************************************
// **** About window realize
// ***************************************************
void
on_label_about_realize                 (GtkWidget       *widget,
                                        gpointer         user_data)
{
	gchar *label_string;

	label_string = g_strconcat ("\nxmms/CD/cover\n\nxmms-Plugin, Version ",VERSION,"\n\nBy\nMagnus Schmidt, (c) 2003\n\neMail: xmms@27b-6.de\n      URL: http://ducts.27b-6.de/cdcover      \n",NULL);
	gtk_label_set_text ((GtkLabel*)widget,label_string);
	g_free (label_string);
}



// ***************************************************
// **** About window button close
// ***************************************************
void
on_button_aboutclosed_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy(window_about);
	window_about = NULL;
}



// ***************************************************
// **** About window delete
// ***************************************************
gboolean
on_dlgAbout_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	window_about = NULL;
	return FALSE;
}





// --- config dialog ----

void load_custom_skin (int row) {
	GtkLabel *label_skin_author;
	GtkCList *clist_skins;
	gchar *author_text,*skin_path;

	// Prepare some vars
	label_skin_author = (GtkLabel*) gtk_object_get_data (GTK_OBJECT(window_config),"label_skin_author");
	clist_skins       = (GtkCList*) gtk_object_get_data (GTK_OBJECT(window_config),"clist_skins");

	if (row!=-1) {
		// Load the skin selected in the list

		// Update the author label
		gtk_clist_get_text (clist_skins,row,1,&author_text);
		gtk_label_set_text (label_skin_author,author_text);

		// Load the new skin
		gtk_clist_get_text (clist_skins,row,2,&skin_path);
		mainwindow_loadskin (skin_path,LOADSKIN_TEST);
	} else {
		// Load the standard skin
		mainwindow_loadskin (NULL,LOADSKIN_NORMAL);
	}
}

// ***************************************************
// **** Check preserve aspect ratio
// ***************************************************
void
on_dlgConfig_check_aspectratio_toggled (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	GtkToggleButton *check_aspectratio;
	t_cdcover_config *plugin_config;

	// Signal is also emitted, when set during the realize phase, however
	// we don't want it that way.
	if (!dlgConfig_init) {
		// Prepare some vars we need
		check_aspectratio = (GtkToggleButton*) gtk_object_get_data (GTK_OBJECT(window_config),"check_aspectratio");
		plugin_config = get_xmms_config();

		// Save the new value
		plugin_config->preserve_aspectratio = gtk_toggle_button_get_active (check_aspectratio);

		// redisplay the skin
		load_custom_skin (skin_selected);
	}

}



// ***************************************************
// **** Button add
// ***************************************************
void
on_button_add_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkEntry *path_entry;
	GtkCList *clist_searchpaths;
	gchar *text;

	path_entry = (GtkEntry*) gtk_object_get_data (GTK_OBJECT(window_config),"entry_searchpath");
	clist_searchpaths = (GtkCList*)  gtk_object_get_data (GTK_OBJECT(window_config),"clist_searchpaths");
	text = gtk_editable_get_chars ((GtkEditable*)path_entry,0,-1);

	if (strlen(text)>0) {
		gchar *temp[1];
		temp[0] = text;
		row_count = gtk_clist_append (clist_searchpaths,temp);
		gtk_entry_set_text (path_entry,"");
	}

	g_free (text);
}



// ***************************************************
// **** Button up
// ***************************************************
void
on_button_up_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkCList *search_list;

	search_list = (GtkCList*) gtk_object_get_data (GTK_OBJECT(window_config),"clist_searchpaths");

	if (row_selected>0 && row_count>0) {
		gtk_clist_swap_rows (search_list,row_selected,row_selected-1);
		row_selected--;
	}
}



// ***************************************************
// **** Button down
// ***************************************************
void
on_dglConfig_button_down_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkCList *search_list;

	search_list = (GtkCList*) gtk_object_get_data (GTK_OBJECT(window_config),"clist_searchpaths");

	if (row_selected<row_count && row_count>0 && row_selected>=0) {
		gtk_clist_swap_rows (search_list,row_selected,row_selected+1);
		row_selected++;
	}
}



// ***************************************************
// **** Button delete
// ***************************************************
void
on_dglConfig_button_delete_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkCList *search_list;

	search_list = (GtkCList*) gtk_object_get_data (GTK_OBJECT(window_config),"clist_searchpaths");

	if (row_selected<=row_count && row_count>=0 && row_selected>=0) {
		gtk_clist_remove (search_list,row_selected);
		row_selected=-1;
		row_count--;
	}
}



// ***************************************************
// **** Button help
// ***************************************************
void
on_dlgConfig_button_help_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *help;

	help = (GtkWidget*) gtk_object_get_data (GTK_OBJECT(window_config),"vbox_help");
	if (GTK_WIDGET_VISIBLE(help)) {
		gtk_widget_hide (help);
	} else {
		gtk_widget_show (help);
	}
}



// ***************************************************
// **** Row selected in search path list
// ***************************************************
void
on_clist_searchpaths_select_row        (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	row_selected = row;
}



// ***************************************************
// **** Row selected in skin list
// ***************************************************
void
on_dlgConfig_clist_skins_select_row    (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	// Save the selected skin-row, so we can save it later on
	skin_selected = row;

	// Load the new skin
	load_custom_skin (row);
}



// ***************************************************
// **** Button OK
// ***************************************************
void
on_dlgConfig_button_ok_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkCList *clist_searchpaths,*clist_extensions,*clist_skins;
	GtkToggleButton *check_remember,*check_aspectratio;
	t_cdcover_config *plugin_config;
	GSList *newlist = NULL;
	GSList *oldlist = NULL;
	GSList *position = NULL;
	GSList *positionext = NULL;
	GSList *newlistext = NULL;
	GSList *oldlistext = NULL;
	gchar *entry, *old, *skin_path;
	gboolean skin_changed = FALSE;
	gint row = 0;

	clist_searchpaths = (GtkCList*)  gtk_object_get_data (GTK_OBJECT(window_config),"clist_searchpaths");
	clist_extensions  = (GtkCList*)  gtk_object_get_data (GTK_OBJECT(window_config),"clist_extensions");
	clist_skins       = (GtkCList*)  gtk_object_get_data (GTK_OBJECT(window_config),"clist_skins");
	check_remember    = (GtkToggleButton*) gtk_object_get_data (GTK_OBJECT(window_config),"check_last_winpos");
	check_aspectratio = (GtkToggleButton*) gtk_object_get_data (GTK_OBJECT(window_config),"check_aspectratio");

	// Get pointer to config structure
	plugin_config = get_xmms_config();

	// Iterate through the search path list a construct a new list with it
	while (gtk_clist_get_text(clist_searchpaths,row,0,&entry)) {
		gchar *newpath;
		newpath = g_strconcat (entry,NULL);
		newlist = g_slist_append (newlist,newpath);
		row++;
	}
	oldlist = plugin_config->cover_searchpaths;
	plugin_config->cover_searchpaths = newlist;

	// Do the same for extensions
	while (gtk_clist_get_text(clist_extensions,row,0,&entry)) {
		gchar *newext;
		newext = g_strconcat (entry,NULL);
		newlistext = g_slist_append (newlistext,newext);
		row++;
	}
	oldlistext = plugin_config->cover_extensions;
	plugin_config->cover_extensions = newlistext;

	// Remember window position check box
	plugin_config->save_window_pos = gtk_toggle_button_get_active (check_remember);

	// Preserve aspect ratio checkbox
	plugin_config->preserve_aspectratio = gtk_toggle_button_get_active (check_aspectratio);

	// Save the selected skin
	if (skin_selected>=0) {
		// Free the old path
		free (plugin_config->skin_path);

		// The user has selected a skin, so lets save it
		if (skin_selected==0) {
			// The default inbuilt skin is selected, path must be NULL
			plugin_config->skin_path = NULL;
		} else {
			// A user suplied skin has been selected, retrieve the path now
			gtk_clist_get_text (clist_skins,skin_selected,2,&skin_path);
			plugin_config->skin_path = strdup (skin_path);
		}
		skin_changed = TRUE;
	}

	// Save the config
	write_xmms_config();

	// Configuration is done, tidy up the old config list
	position = oldlist;
	while (position!=NULL) {
		g_free (position->data);
		position = g_slist_next (position);
	}
	g_slist_free (oldlist);

	// Do the same for extensions
	positionext = oldlistext;
	while (positionext!=NULL) {
		g_free (positionext->data);
		positionext = g_slist_next (positionext);
	}
	g_slist_free (oldlistext);

	// Reload the necessary display parts
	if (skin_changed) {
		// Reload the entire skin
		mainwindow_loadskin (NULL,LOADSKIN_NORMAL);
	} else {
		// Only reload the cover
		// Empty the current filename saved in mainwindow.c, in order
		// to force a reload
		entry = g_strconcat ("",NULL);
		old = current_filename;
		current_filename = entry;
		g_free (old);
	}

	// Destroy the window
	gtk_widget_destroy (window_config);
	window_config = NULL;
}



// ***************************************************
// **** Button Cancel
// ***************************************************
void
on_dlgConfig_button_cancel_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
	t_cdcover_config *plugin_config;

	gtk_widget_destroy(window_config);
	window_config = NULL;

	// Get pointer to config structure
	plugin_config = get_xmms_config();

	// We might have changed the skin, so restore it
	plugin_config->preserve_aspectratio = old_preserve_aspectratio;
	mainwindow_loadskin (NULL,LOADSKIN_NORMAL);
}



// ***************************************************
// **** Window realize
// ***************************************************
void
on_dlgConfig_realize                   (GtkWidget       *widget,
                                        gpointer         user_data)
{
	GtkCList *clist_searchpaths,*clist_extensions,*clist_skins;
	GtkWidget *check_remember,*check_aspectratio;
	t_cdcover_config *plugin_config;
	GSList *position = NULL;
	GSList *positionext = NULL;
	GSList *skinlist = NULL;
	gchar *default_skin[3] = {"<default>","Bultin skin, derived from the standard xmms skin",""};

	dlgConfig_init = TRUE;

	// Read configuration
	read_xmms_config();

	// get some start values
	clist_searchpaths = (GtkCList*)  gtk_object_get_data (GTK_OBJECT(window_config),"clist_searchpaths");
	clist_extensions  = (GtkCList*)  gtk_object_get_data (GTK_OBJECT(window_config),"clist_extensions");
	clist_skins       = (GtkCList*)  gtk_object_get_data (GTK_OBJECT(window_config),"clist_skins");
	check_remember    = (GtkWidget*) gtk_object_get_data (GTK_OBJECT(window_config),"check_last_winpos");
	check_aspectratio = (GtkWidget*) gtk_object_get_data (GTK_OBJECT(window_config),"check_aspectratio");

	plugin_config = get_xmms_config();
	position = plugin_config->cover_searchpaths;
	positionext = plugin_config->cover_extensions;

	// First clear the list, just to be on the secure side
	gtk_clist_clear (clist_searchpaths);
	gtk_clist_clear (clist_extensions);

	// Now iterate through the list and add the pathnames
	while (position!=NULL) {
		gchar *temp[1];
		temp[0] = position->data;
		row_count = gtk_clist_append (clist_searchpaths,temp);
		position = g_slist_next (position);
	}
	while (positionext!=NULL) {
		gchar *temp[1];
		temp[0] = positionext->data;
		row_count = gtk_clist_append (clist_extensions,temp);
		positionext = g_slist_next (positionext);
	}

	// Remember window pos
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(check_remember),plugin_config->save_window_pos);

	// Preserve aspect ratio
	old_preserve_aspectratio = plugin_config->preserve_aspectratio;
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(check_aspectratio),old_preserve_aspectratio);

	// Load the skinlist
	skinlist = search_skins ();
	// Add the entries to our clist select box
	// The first column contains the visible skin name, the second the author and the third the path
	// Column 2 and 3 are hidden, row 1 contains the default built in skin
	gtk_clist_set_column_visibility (clist_skins,1,FALSE);
	gtk_clist_set_column_visibility (clist_skins,2,FALSE);
	gtk_clist_append (clist_skins,default_skin);
	position = skinlist;
	while (position!=NULL) {
		_skinsearch_entry *entry = position->data;
		gchar *temp_entry[3];

		// Prepare the array for clist_append
		temp_entry[0] = entry->skin_title;
		temp_entry[1] = entry->skin_author;
		temp_entry[2] = entry->skin_path;
		// Add it to the box
		gtk_clist_append (clist_skins,temp_entry);
		// Free the old data, we don't need it any longer
		free (entry->skin_title);
		free (temp_entry[1]);
		free (temp_entry[2]);
		free (position->data);
		// Next entry
		position = g_slist_next (position);
	}
	g_slist_free (skinlist);

	dlgConfig_init = FALSE;
}



// ***************************************************
// **** Window delete
// ***************************************************
gboolean
on_dlgConfig_delete_event              (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	t_cdcover_config *plugin_config;

	window_config = NULL;
	// Get pointer to config structure
	plugin_config = get_xmms_config();

	// We might have changed the skin, so restore it
	plugin_config->preserve_aspectratio = old_preserve_aspectratio;
	if (skin_selected!=-1) mainwindow_loadskin (NULL,LOADSKIN_NORMAL);

	return FALSE;
}




void
on_button_e_add_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_clist_extensions_select_row         (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

}


void
on_button_e_up_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_e_down_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_e_delete_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{

}

