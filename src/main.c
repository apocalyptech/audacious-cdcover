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

#include <stdio.h>			// standard functions
#include <string.h>			// string functions
#include <audacious/plugin.h>		// XMMS Plugin-Interface
#include <gtk/gtk.h>			// needed for dialogs
#include "mainwindow.h"			// the main gtk-window
#include "debug.h"			// Debug functions
#include "globals.h"			// config
#include "configmgm.h"			// xmms config
#include "interface/interface.h"	// dialogs, created by glade
#include "interface/callbacks_export.h"	// the config and about dialog


// Forward declarations for xmms_vp
static void xmmsplugin_init (void);
static void xmmsplugin_cleanup (void);
static void xmmsplugin_about (void);
static void xmmsplugin_configure (void);
static void xmmsplugin_playback_start (void);
static void xmmsplugin_playback_stop (void);
static void xmmsplugin_disable_plugin (struct _VisPlugin *);

// XMMS-Plugin structure
/*
VisPlugin xmms_vp = {
	0, 			 	// Handle, filled in by xmms
	0, 			 	// Filename, filled in by xmms
	0,                  		// Session ID
	PLUGIN_NAME,       	 	// description
	0, 			 	// # of PCM channels for render_pcm()
	1, 			 	// # of freq channels wanted for render_freq()
	xmmsplugin_init,       		// Called when plugin is enabled
	xmmsplugin_cleanup,    		// Called when plugin is disabled
	xmmsplugin_about,     		// Show the about box
	xmmsplugin_configure,  		// Show the configure box
	0,                      	// Called to disable plugin, filled in by xmms
	xmmsplugin_playback_start, 	// Called when playback starts
	xmmsplugin_playback_stop,  	// Called when playback stops
	0,                     		// Render the PCM data, must return quickly
	0     		 		// Render the freq data, must return quickly
};
*/
/*
VisPlugin xmms_vp = {
	0, 			 	// Handle, filled in by xmms
	0, 			 	// Filename, filled in by xmms
	PLUGIN_NAME,       	 	// description
	xmmsplugin_init,       		// Called when plugin is enabled
	xmmsplugin_cleanup,    		// Called when plugin is disabled
	xmmsplugin_about,     		// Show the about box
	xmmsplugin_configure,  		// Show the configure box
	0,                      	// Called to disable plugin, filled in by xmms
	0, 			 	// # of PCM channels for render_pcm()
	1, 			 	// # of freq channels wanted for render_freq()
    xmmsplugin_disable_plugin,      // disable_plugin thing
	xmmsplugin_playback_start, 	// Called when playback starts
	xmmsplugin_playback_stop,  	// Called when playback stops
	0,                     		// Render the PCM data, must return quickly
	0     		 		// Render the freq data, must return quickly
};
*/
static VisPlugin xmms_vp =
{
    .description = PLUGIN_NAME,
    .init = xmmsplugin_init,
    .about = xmmsplugin_about,
    .cleanup = xmmsplugin_cleanup,
    .configure = xmmsplugin_configure,
    .num_pcm_chs_wanted = 0,
    .num_freq_chs_wanted = 1,
    .disable_plugin = xmmsplugin_disable_plugin,
    .playback_start = xmmsplugin_playback_start,
    .playback_stop = xmmsplugin_playback_stop,
    .render_pcm = 0,
    .render_freq = 0
};

// XMMS entry point
VisPlugin *get_vplugin_info (void) {
	return &xmms_vp;
}

int done_cleanup = 0;

static void xmmsplugin_disable_plugin (struct _VisPlugin *plugin)
{
}

// XMMS exit point
static void xmmsplugin_cleanup (void) {
	DPRINT (__DEBUG_XMMS__,"xmms: cleanup called");
	if (done_cleanup == 0)
	{
		printf("Doing Cleanup activities\n");
		write_xmms_config();
		mainwindow_done();
		free_xmms_config();
		done_cleanup = 1;
	}
	else
	{
		printf("Cleanup has already been called, skipping\n");
	}
}

// XMMS Plugin initialization
static void xmmsplugin_init (void) {
	DPRINT (__DEBUG_XMMS__,"xmms: plugin init");
	read_xmms_config();
	mainwindow_init();
	mainwindow_show();
	done_cleanup = 0;
}

// XMMS About request

static void xmmsplugin_about (void) {
	DPRINT (__DEBUG_XMMS__,"xmms: About dialog requested");
	interface_showabout();
}

// XMMS Configure request
static void xmmsplugin_configure (void) {
	DPRINT (__DEBUG_XMMS__,"xmms: Configure dialog request");
	interface_showconfig();
}

// XMMS signals that its starting playback
static void xmmsplugin_playback_start (void) {
	DPRINT (__DEBUG_XMMS__,"xmms: Playback is starting");
}

// XMMS signals that playback has stoped
static void xmmsplugin_playback_stop (void) {
	DPRINT (__DEBUG_XMMS__,"xmms: Playback has stopped");
}

VisPlugin *cdcover_gplist[] = { &xmms_vp, NULL };

DECLARE_PLUGIN(cdcover, NULL, NULL, NULL, NULL, NULL, NULL, cdcover_gplist, NULL);

