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

#include <stdio.h>				// basic functions
#include <string.h>				// string operations
#include <stdlib.h>				// realloc
#include <gtk/gtk.h>				// gtk for window functions
#include <gdk/gdk.h>				// gdk, for graphic functions
#include <gdk-pixbuf/gdk-pixbuf.h>		// pixbuf-libs, needed for gdk fileloading
#include <gdk-pixbuf/gdk-pixbuf-loader.h>	// pixbuf-libs
#include <audacious/plugin.h>			// XMMS plugin-interface
#include <audacious/playlist.h>			// XMMS plugin-interface
#include "globals.h"				// configuration
#include "mainwindow.h"				// Defines for this window
#include "configmgm.h"				// configuration management
#include "debug.h"				// debug printing
#include "findcover.h"				// Searches for a valid coverfilename



// xmms-structure from main.c
extern VisPlugin xmms_vp;
// findcover filename from findcover.c
extern char *findcover_filename;

// Configuration
t_cdcover_config *plugin_config;

// Struct to organize global data we need for the gtk-window
struct {
	// Our mainwindow
	GtkWidget *window;
	// Timer to check cover
	guint window_timer;
	// Visible drawing surface
	GtkWidget *drawing_area;
	// Double-buffer pixmap
	GdkPixmap *dbuf_pixmap;
	// Simple Mutex for the timer
	gboolean loading_cover;
	// Window movement
	int window_moving;
	int movement_x,movement_y;
	// Semaphore for skin loading
	gboolean loading_skin;
} mainwindow_data = {NULL,0,NULL,NULL,FALSE,0,0,0,FALSE};

// Current filename playing
char *current_filename;

// Redraw parts of the window if requestet
void mainwindow_sigexpose (GtkWidget *widget,GdkEventExpose *event,gpointer data) {
	if (mainwindow_data.loading_skin) return;

	if (mainwindow_data.loading_cover==FALSE) {
		// Don't repaint entire window upon each exposure
  		gdk_window_set_back_pixmap (widget->window, NULL, FALSE);

  		// Refresh double buffer, then copy the "dirtied" area to
   		// the on-screen GdkWindow
  		gdk_window_copy_area(widget->window,
    			widget->style->fg_gc[GTK_STATE_NORMAL],
    			event->area.x, event->area.y,
    			mainwindow_data.dbuf_pixmap,
    			event->area.x, event->area.y,
    			event->area.width, event->area.height);
	}
}

// Signal processing for movement of the window
// Simple drag and move action within the whole window
void mainwindow_sigpress (GtkWidget *widget,GdkEventButton *event, gpointer callback_data) {
        DPRINT (__DEBUG_GENERAL__,"mainwindow_sigpress");

        mainwindow_data.movement_x = event->x;
	mainwindow_data.movement_y = event->y;
        if (event->button==1 && event->type==GDK_BUTTON_PRESS) {
                mainwindow_data.window_moving = TRUE;
                gdk_window_raise (mainwindow_data.window->window);
                gdk_pointer_grab (mainwindow_data.window->window, FALSE,
                        GDK_BUTTON_MOTION_MASK | GDK_BUTTON_RELEASE_MASK, GDK_NONE, GDK_NONE, GDK_CURRENT_TIME);
        }
}

void mainwindow_sigmotion (GtkWidget *widget,GdkEventMotion *event,gpointer callback_data) {
        if (mainwindow_data.window_moving) {
		gint mx,my,newx,newy;
		GdkModifierType modmask;
		// Get Position
        	gdk_window_get_pointer (NULL,&mx,&my,&modmask);
		// Moving window
		newx = mx-mainwindow_data.movement_x;
		newy = my-mainwindow_data.movement_y;
                gdk_window_move (mainwindow_data.window->window,newx,newy);
		plugin_config->winpos_x = newx;
		plugin_config->winpos_y = newy;
        }
}

static void mainwindow_sigrelease (GtkWidget *widget,GdkEventButton *event,gpointer callback_data) {
        DPRINT (__DEBUG_GTK__,"mainwindow_sigrelease");

        gdk_pointer_ungrab (GDK_CURRENT_TIME);
        if (mainwindow_data.window_moving && event->button == 1 && event->type == GDK_BUTTON_RELEASE) {
                mainwindow_data.window_moving = FALSE;
        }
}

static void mainwindow_sigsizerequest (GtkWidget *widget,GtkRequisition *requisition,gpointer user_data) {
	DPRINT (__DEBUG_GTK__,"mainwindow_sigresizerequest");
	// gtk_widget_set_usize (mainwindow_data.window,SKIN_WIDTH,SKIN_HEIGHT);
}

static void mainwindow_sigstatechanged (GtkWidget *widget,GtkStateType state,gpointer user_data) {
	DPRINT (__DEBUG_GTK__,"mainwindow_sigstatechange");
}

void repaint_cover () {
	if (mainwindow_data.loading_skin) return;

 	gdk_window_copy_area (mainwindow_data.drawing_area->window,
    			      mainwindow_data.drawing_area->style->fg_gc[GTK_STATE_NORMAL],
    			      0,0,
    			      mainwindow_data.dbuf_pixmap,
    			      0,0,
			      plugin_config->skin_width,
			      plugin_config->skin_height);
}

void loadimage (char *filename) {
	GdkPixbuf *pixbuf, *pixbuf_scaled, *pixbuf_skin;
	GError *error = NULL;
	pixbuf=pixbuf_scaled=pixbuf_skin=NULL;
	int new_cover_width,new_cover_height,
	    org_cover_width,org_cover_height,
	    cover_offset_x,cover_offset_y;

	if (mainwindow_data.loading_skin) return;


	// Load image
	if (filename) {

        // Load in our "base" skin
        pixbuf_skin = gdk_pixbuf_copy (plugin_config->skin_base_custom_pixbuf);

		pixbuf = gdk_pixbuf_new_from_file (filename, &error);
		org_cover_width  = gdk_pixbuf_get_width (pixbuf);
		org_cover_height = gdk_pixbuf_get_height (pixbuf);

        // Figure out if we should be rescaling at all
        if (org_cover_width > plugin_config->skin_cover_width ||
                org_cover_height > plugin_config->skin_cover_height)
        {
            // Calculate the rescale
            if (plugin_config->preserve_aspectratio) {
                // Resize and preserve the apsect ratio
                new_cover_width  = plugin_config->skin_cover_width;
                new_cover_height = (org_cover_height * new_cover_width) / org_cover_width;
                if (new_cover_height>plugin_config->skin_cover_height) {
                    new_cover_height = plugin_config->skin_cover_height;
                    new_cover_width  = (org_cover_width * new_cover_height) / org_cover_height;
                }
                if (new_cover_height > plugin_config->skin_cover_height ||
                    new_cover_width  > plugin_config->skin_cover_width) {
                        // mmh ... somehting went wrong
                    DPRINT (__DEBUG_GENERAL__,"Resize did not work for %dx%d -> %dx%d",
                                              org_cover_width,org_cover_height,
                                  plugin_config->skin_cover_width,
                                  plugin_config->skin_cover_height)
                        new_cover_width  = plugin_config->skin_cover_width;
                    new_cover_height = plugin_config->skin_cover_height;
                }
            } else {
                // Just a brutal rescale to fit in the target area
                new_cover_width  = plugin_config->skin_cover_width;
                new_cover_height = plugin_config->skin_cover_height;
            }
        }
        else
        {
            new_cover_width = org_cover_width;
            new_cover_height = org_cover_height;
        }
		cover_offset_x = (plugin_config->skin_cover_width  - new_cover_width)  / 2;
		cover_offset_y = (plugin_config->skin_cover_height - new_cover_height) / 2;

		// Now scale the cover
		pixbuf_scaled = gdk_pixbuf_scale_simple (pixbuf,                // source pixbuf
						         new_cover_width,	// Target width
							 new_cover_height,	// Target height
							 GDK_INTERP_BILINEAR);  // Flags

		// Copy loaded cover onto the skin
		gdk_pixbuf_copy_area (pixbuf_scaled,				// Source pixbuf
			      	0,0,						// Source x,y
    				new_cover_width,                		// Width to copy
				new_cover_height,               		// Width to copy
			      	pixbuf_skin,					// Destination pixbuf
				plugin_config->skin_coverx + cover_offset_x,	// Desination x,y
				plugin_config->skin_covery + cover_offset_y);	// Desination x,y
		// free the original pictures
		g_object_unref (pixbuf);
		g_object_unref (pixbuf_scaled);
	}
    else
    {
        // Load in the skin that has "no cover" information
        pixbuf_skin = gdk_pixbuf_copy (plugin_config->skin_nocover_custom_pixbuf);
    }

	// Copy image-data to the double-buffer
  	gdk_pixbuf_render_to_drawable (pixbuf_skin,  					// A Pixbuf
				       mainwindow_data.dbuf_pixmap, 			// Destination drawable
				       mainwindow_data.window->style->fg_gc[GTK_STATE_NORMAL], // GC
    				       0,0, 						// Source X/Y in pixbuf
				       0,0,						// Dest X/Y
				       plugin_config->skin_width,			// Dest. height
				       plugin_config->skin_height,			// Dest. height
    				       GDK_RGB_DITHER_NORMAL,0,0); 			// Dithermode and Offset for dither

	g_object_unref (pixbuf_skin);

	// Repaint the new cover
	repaint_cover();
}

gboolean checkcover (gint tag) {
	gint  playlist_position;
    gchar *playlist_uri = NULL;
    gchar *playlist_filename = NULL;
    gint playlist = aud_playlist_get_active();
    gboolean processed = 0;
    gboolean fast_call = 0;

	gdk_threads_enter ();
	playlist_position = aud_playlist_get_position(playlist);
    //playlist_uri = aud_playlist_get_filename (playlist);
    const Tuple *tuple = aud_playlist_entry_get_tuple(playlist, playlist_position, fast_call);
    if (tuple)
    {
        playlist_uri = g_strdup((gchar *) tuple_get_string(tuple, FIELD_FILE_PATH, NULL));
        if (playlist_uri)
        {
            playlist_filename = g_filename_from_uri(playlist_uri, NULL, NULL);
            if (playlist_filename)
            {
                if (strcmp (playlist_filename,current_filename) && !mainwindow_data.loading_cover)
                {
                    mainwindow_data.loading_cover = TRUE;
                    // Filename has changed
                    current_filename = realloc (current_filename,strlen(playlist_filename)+1);
                    strcpy (current_filename,playlist_filename);
                    DPRINT (__DEBUG_GENERAL__,"New playfile detected: %s",current_filename);
                    // Find new cover if possible
                    findcover_search (current_filename);
                    DPRINT (__DEBUG_GENERAL__,"Cover to display: %s",findcover_filename);
                    loadimage (findcover_filename);
                    mainwindow_data.loading_cover = FALSE;
                }
                free (playlist_filename);
                free (playlist_uri);
                processed = 1;
            }
        }
    }

    if (!processed)
    {
        mainwindow_data.loading_cover = TRUE;
        loadimage (NULL);
        mainwindow_data.loading_cover = FALSE;
    }
	gdk_threads_leave ();

	return TRUE;
}

void mainwindow_loadskin (gchar *skinpath,_loadskin_types loadtype) {
	// Have we got a window at all ?
	if (mainwindow_data.window==NULL) return;

	DPRINT (__DEBUG_GTK__,"Loading skin");
	// Prevent any display operations while loading
	mainwindow_data.loading_skin = TRUE;
	// Load skin configuration
	init_skin (skinpath,loadtype);
	// Create double-buffered pixmap
	// unref pixmap, in case we call this function again
	if (mainwindow_data.dbuf_pixmap!=NULL) gdk_pixmap_unref (mainwindow_data.dbuf_pixmap);
	mainwindow_data.dbuf_pixmap = gdk_pixmap_new (mainwindow_data.window->window,
						      plugin_config->skin_width,plugin_config->skin_height,
						      -1);
	// Set new window size
	gtk_widget_set_usize (mainwindow_data.window,plugin_config->skin_width,plugin_config->skin_height);
	// Allow window operations again
	mainwindow_data.loading_skin = FALSE;

	// Repaint the new window
	loadimage (findcover_filename);
}

void mainwindow_done () {
	DPRINT (__DEBUG_GTK__,"shutting down window");

	// Destroy the timer
	if (mainwindow_data.window_timer!=0) {
		g_source_remove (mainwindow_data.window_timer);
		mainwindow_data.window_timer = 0;
	}
	free (current_filename);
	// Only hide window, we cannot quit as this would destroy xmms itself
	gtk_widget_hide (mainwindow_data.window);
	// Deinit findcover
	findcover_done();
}

// Close application
void mainwindow_sigquit (GtkWidget *widget) {
  mainwindow_done();
}

void mainwindow_init () {
	DPRINT (__DEBUG_GENERAL__,"mainwindow_init: plugin initialization");

	// Get a reference to the config
	plugin_config = get_xmms_config();

	// Only init once
	if (mainwindow_data.window == NULL) {
		gtk_init (NULL,NULL);
	 	gdk_rgb_init();

		DPRINT (__DEBUG_GTK__,"mainwindow_init: creating gtk window");

		// Create Window
		mainwindow_data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title (mainwindow_data.window,PLUGIN_NAME);

		// Create drawing-area widget, must exist before expose signal connect (?!)
	  	mainwindow_data.drawing_area = gtk_drawing_area_new ();

		// Register events we want to know
		gtk_widget_set_events (mainwindow_data.window,GDK_BUTTON_MOTION_MASK|GDK_BUTTON_PRESS_MASK|GDK_BUTTON_RELEASE_MASK);
		// Handle singla window manager closing
	  	gtk_signal_connect (GTK_OBJECT(mainwindow_data.window),"delete_event",GTK_SIGNAL_FUNC(mainwindow_sigquit),NULL);
		// Handle signal for window movement
		gtk_signal_connect (GTK_OBJECT(mainwindow_data.window),"button_press_event",  GTK_SIGNAL_FUNC(mainwindow_sigpress),NULL);
		gtk_signal_connect (GTK_OBJECT(mainwindow_data.window),"button_release_event",GTK_SIGNAL_FUNC(mainwindow_sigrelease),NULL);
		gtk_signal_connect (GTK_OBJECT(mainwindow_data.window),"motion_notify_event", GTK_SIGNAL_FUNC(mainwindow_sigmotion),NULL);
		// Window signal for repaint signal
		gtk_signal_connect (GTK_OBJECT(mainwindow_data.drawing_area),"expose_event", GTK_SIGNAL_FUNC(mainwindow_sigexpose), NULL);
		gtk_signal_connect (GTK_OBJECT(mainwindow_data.drawing_area),"size-request", GTK_SIGNAL_FUNC(mainwindow_sigsizerequest), NULL);
		gtk_signal_connect (GTK_OBJECT(mainwindow_data.drawing_area),"state-changed", GTK_SIGNAL_FUNC(mainwindow_sigstatechanged), NULL);

		// Hide all window manager decorations, we have a skin
		gtk_widget_realize (mainwindow_data.window);
		gdk_window_set_decorations (mainwindow_data.window->window,0);
		gdk_window_set_functions   (mainwindow_data.window->window,0);
		gdk_window_set_back_pixmap (mainwindow_data.window->window, NULL, FALSE);
		gtk_window_set_policy	   (GTK_WINDOW(mainwindow_data.window),FALSE,FALSE,FALSE);

		// Container with our drawing area widget
		gtk_container_add (GTK_CONTAINER (mainwindow_data.window), mainwindow_data.drawing_area);
		gtk_widget_show (mainwindow_data.drawing_area);
		gtk_widget_show_all (mainwindow_data.window);

	}
	// Load skin
	mainwindow_loadskin (NULL,LOADSKIN_NORMAL);

	// Alocate memory for current filename
	current_filename = (char *) malloc (1);
	current_filename[0] = 0;

	// Create a first window, without a cover right now
	loadimage (NULL);

	// Init search functions
	findcover_init();

	// Add a timer
	if (mainwindow_data.window_timer==0) {
		mainwindow_data.window_timer = g_timeout_add (COVER_RECHECK_MS,(GSourceFunc)checkcover,NULL);
	}
}

void mainwindow_show() {
	// Show window
	gtk_widget_show  (mainwindow_data.window);

	// Move window to old position if requested
	if (plugin_config->save_window_pos && plugin_config->winpos_x!=-1 && plugin_config->winpos_y!=-1) {
		gdk_window_move (mainwindow_data.window->window,plugin_config->winpos_x,plugin_config->winpos_y);
	}
}
