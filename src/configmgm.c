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

#include <stdio.h>		// standard functions
#include <stdlib.h>		// free, etc
#include <audacious/plugin.h>	// xmms config management
#include <audacious/configdb.h>	// xmms config management
#include <string.h>		// string functions
#include <libgen.h>		// dirname
#include <sys/types.h>		// needed for opendir/readdir
#include <dirent.h>		// needed for opendir/readdir
#include <fnmatch.h>		// needed for fnmatch()
#include "debug.h"		// debugging
#include "globals.h"		// system-wide config
#include "configmgm.h"		// config-structs
#include "skin/skin1_base.xpm"	// The background skin
#include "skin/skin1_nocover.xpm"	// The background skin
#include "skin/skin1.h"		// Some size constants

t_cdcover_config cdcover_config = {1,-1,-1,NULL,NULL,FALSE,NULL,-1,-1,-1,-1,-1,-1,NULL,NULL};

void init_skin (gchar *skinpath, _loadskin_types loadtype) {
	gboolean init_success = FALSE;
	/*mcs_handle_t *config;
	GError *error = NULL;
	gchar *file_bitmap,*file_bitmapwithpath,*file_bitmapdirname,*file_bitmaptemp;*/
	gchar *real_skinpath;

	// In case we are in configuration, skinpath contains the path to try
	if (loadtype==LOADSKIN_NORMAL) {
		real_skinpath = cdcover_config.skin_path;
	} else {
		if (strlen(skinpath)==0) {
			real_skinpath = NULL;
		} else {
			real_skinpath = skinpath;
		}
	}

	// Unreference old buffer
	if (cdcover_config.skin_base_custom_pixbuf!=NULL) {
		g_object_unref (cdcover_config.skin_base_custom_pixbuf);
	}
	if (cdcover_config.skin_nocover_custom_pixbuf!=NULL) {
		g_object_unref (cdcover_config.skin_nocover_custom_pixbuf);
	}
	// Reset some values
	cdcover_config.skin_coverx=cdcover_config.skin_covery=-1;
	cdcover_config.skin_cover_width=cdcover_config.skin_cover_width=-1;

    // hard-code using the default
	/*if (real_skinpath==NULL) {*/
		// No skin sepecified, so use the inbuild default skin
		cdcover_config.skin_width        = DEFAULT_SKIN_WIDTH;
		cdcover_config.skin_height       = DEFAULT_SKIN_HEIGHT;
		cdcover_config.skin_coverx       = DEFAULT_SKIN_COVER_STARTX;
		cdcover_config.skin_covery       = DEFAULT_SKIN_COVER_STARTY;
		cdcover_config.skin_cover_width  = DEFAULT_SKIN_COVER_WIDTH;
		cdcover_config.skin_cover_height = DEFAULT_SKIN_COVER_HEIGHT;
		// Load Skin from memory
		cdcover_config.skin_base_custom_pixbuf = gdk_pixbuf_new_from_xpm_data ((const char **)skin1_base_xpm);
		cdcover_config.skin_nocover_custom_pixbuf = gdk_pixbuf_new_from_xpm_data ((const char **)skin1_nocover_xpm);
		init_success = TRUE;
	/*} else {
		// Load configuration data for custom skin
		//config = xmms_cfg_open_file (real_skinpath);
		config = aud_cfg_db_open ();
		if (config) {
			DPRINT (__DEBUG_GENERAL__,"Loading skin '%s'",real_skinpath);
			// Read values
			aud_cfg_db_get_string (config,"SKIN","file",&file_bitmap);
			aud_cfg_db_get_int    (config,"SKIN","coverx",&cdcover_config.skin_coverx);
			aud_cfg_db_get_int    (config,"SKIN","covery",&cdcover_config.skin_covery);
			aud_cfg_db_get_int    (config,"SKIN","coverwidth",&cdcover_config.skin_cover_width);
			aud_cfg_db_get_int    (config,"SKIN","coverheight",&cdcover_config.skin_cover_height);
			aud_cfg_db_close (config);

			// Now load the skin-bitmap into a pixbuf
			if (file_bitmap!=NULL &&
			    cdcover_config.skin_coverx!=-1 && cdcover_config.skin_covery!=-1 &&
			    cdcover_config.skin_cover_width!=-1 && cdcover_config.skin_cover_width!=-1) {
				file_bitmaptemp = strdup (real_skinpath);
				file_bitmapdirname = dirname (file_bitmaptemp);
				file_bitmapwithpath = g_strconcat (file_bitmapdirname,DIR_SEPARATOR_STRING,
				                                   file_bitmap,NULL);
				cdcover_config.skin_custom_pixbuf = gdk_pixbuf_new_from_file (file_bitmapwithpath, &error);
				if (cdcover_config.skin_nocover_custom_pixbuf!=NULL) {
					cdcover_config.skin_width  = gdk_pixbuf_get_width (cdcover_config.skin_custom_pixbuf);
					cdcover_config.skin_height = gdk_pixbuf_get_height (cdcover_config.skin_custom_pixbuf);
					init_success = TRUE;
				} else {
					DPRINT (__DEBUG_GENERAL__,"gdk_pixbuf_new_from_file failed for '%s'",file_bitmapwithpath);
				}
				g_free (file_bitmapwithpath);
				free (file_bitmaptemp);
			} else {
				DPRINT (__DEBUG_GENERAL__,"No valid path in skin config found, or other params failed");
			}

		} else {
			DPRINT (__DEBUG_GENERAL__,"Failed to open skin config '%s'",real_skinpath);
		}
	}

	if (!init_success) {
		// Something went wrong, load the inbuild skin instead
		// calling this recursivly is dangerous, as the execution path
		// with skin_path=NULL is not allowed to fail, otherwise we in end
		// up with a stack overflow
		DPRINT (__DEBUG_GENERAL__,"WARNING: loadksin failed, trying to load default");
		if (skinpath==NULL) free (real_skinpath);
		cdcover_config.skin_path = NULL;
		init_skin (NULL,LOADSKIN_NORMAL);
	}
    */
}

GSList *search_skins_indirectory (char *dirname,GSList *skinlist) {
	DIR *dirhandle;
	struct dirent *direntry;
	mcs_handle_t *config;
	gchar *skin_path;

	dirhandle = opendir (dirname);
	if (dirhandle>0) {
		while ((direntry=readdir(dirhandle))) {
			if (fnmatch("*.ini",direntry->d_name,0)==0) {
				// Construct the skin path
				skin_path = g_strconcat (dirname,DIR_SEPARATOR_STRING,direntry->d_name,NULL);
				// We found a inifile, read the data
				//config = xmms_cfg_open_file (skin_path);
				config = aud_cfg_db_open ();
				if (config) {
					gchar *skin_type,*skin_author,*skin_path_copy,*skin_title;
					_skinsearch_entry *skinentry = malloc (sizeof(_skinsearch_entry));
					skin_type=skin_author=skin_title=NULL;

					// Test the skin type
					aud_cfg_db_get_string (config,"SKIN","skintype",&skin_type);
					if (skin_type==NULL) break;
					if (strcmp(skin_type,SKIN_TYPE_IDENTIFIER)) break;
					// Read in the author
					aud_cfg_db_get_string (config,"SKIN","author",&skin_author);
					skinentry->skin_author = skin_author;
					// Read in the title
					aud_cfg_db_get_string (config,"SKIN","title",&skin_title);
					skinentry->skin_title = skin_title;
					// Save the skin path
					skin_path_copy = strdup (skin_path);
					skinentry->skin_path = skin_path_copy;
					DPRINT (__DEBUG_GENERAL__,"Found skin: '%s','%s','%s'",
						skinentry->skin_title,skinentry->skin_author,skinentry->skin_path);
					// Save the new entry to our skinlist
					skinlist = g_slist_append (skinlist,skinentry);
					// Free the unused skin_type and free config
					g_free (skin_type);
					aud_cfg_db_close (config);
				}
				g_free (skin_path);

			}
		}
		closedir (dirhandle);
	}

	return skinlist;
}

GSList *search_skins () {
	gchar *searchpath;
	GSList *skinlist;

	skinlist = NULL;
	// Search in home dir
	searchpath = g_strconcat (g_get_home_dir(),DIR_SEPARATOR_STRING,".xmms/CDcover",NULL);
	skinlist = search_skins_indirectory (searchpath,skinlist);
	g_free (searchpath);

	return skinlist;
}

void read_xmms_config() {
	int  count;
	/*char tempname[100];*/
	char *tempname = (char *)malloc(sizeof(char)*100);
	gchar *path;
	gchar *ext;
	mcs_handle_t *config;

	DPRINT (__DEBUG_GENERAL__,"Starting to read xmms config");

	// empty cover search list, the plugin is probably initialized twice by the user
	g_slist_free (cdcover_config.cover_searchpaths);
	cdcover_config.cover_searchpaths = NULL;

	// empty extension search list, the plugin is probably initialized twice by the user
	g_slist_free (cdcover_config.cover_extensions);
	cdcover_config.cover_extensions = NULL;

	config = aud_cfg_db_open ();
	if (config) {
		// Window position
		aud_cfg_db_get_bool (config,PLUGIN_NAME,"savewindowpos",&cdcover_config.save_window_pos);
		aud_cfg_db_get_int  (config,PLUGIN_NAME,"windowposx",&cdcover_config.winpos_x);
		aud_cfg_db_get_int  (config,PLUGIN_NAME,"windowposy",&cdcover_config.winpos_y);

		// Aspect ratio
		aud_cfg_db_get_bool (config,PLUGIN_NAME,"aspectratio",&cdcover_config.preserve_aspectratio);

		// Skin
		if (!aud_cfg_db_get_string  (config,PLUGIN_NAME,"skinpath",&cdcover_config.skin_path)) {
			cdcover_config.skin_path = NULL;
		}

		// Read in the paths
		sprintf (tempname,"path1");
		count=1;
		while (aud_cfg_db_get_string (config,PLUGIN_NAME,tempname,&path)) {
			// save the pointer to this resource string in the list
			cdcover_config.cover_searchpaths = g_slist_append (cdcover_config.cover_searchpaths,path);
			//printf("Got path '%s' with var %s\n", path, tempname);
			// construct next path variable
			count++;
			sprintf (tempname,"path%d",count);
		}
		// Where we able to read any search paths ?
		if (g_slist_length(cdcover_config.cover_searchpaths)==0) {
			// Some default paths to start with
			// Allocate memory, so we can free it on shutdown
			//printf("No paths found, populating with defaults\n");
			gchar *path1 = g_strconcat ("?PATH?/cover.jpg",NULL);
			gchar *path2 = g_strconcat ("?PATH?/?BASE?.jpg",NULL);
			gchar *path3 = g_strconcat ("?PATH?/media/?FILENAME?.jpg",NULL);
			gchar *path4 = g_strconcat ("?PATH?/*.jpg",NULL);
			cdcover_config.cover_searchpaths = g_slist_append (cdcover_config.cover_searchpaths,path1);
			cdcover_config.cover_searchpaths = g_slist_append (cdcover_config.cover_searchpaths,path2);
			cdcover_config.cover_searchpaths = g_slist_append (cdcover_config.cover_searchpaths,path3);
			cdcover_config.cover_searchpaths = g_slist_append (cdcover_config.cover_searchpaths,path4);
		}

		// Read in the extensions
		sprintf (tempname,"ext1");
		count=1;
		while (aud_cfg_db_get_string (config,PLUGIN_NAME,tempname,&ext)) {
			// save the pointer to this resource string in the list
			cdcover_config.cover_extensions = g_slist_append (cdcover_config.cover_extensions,ext);
			//printf("Got ext '%s' with var %s\n", ext, tempname);
			// construct next path variable
			count++;
			sprintf (tempname,"ext%d",count);
		}
		// Were we able to read any extensions?
		if (g_slist_length(cdcover_config.cover_extensions)==0) {
			// Some default extensions to start with
			// Allocate memory, so we can free it on shutdown
			//printf("No extensions found, populating with defaults\n");
			gchar *ext1 = g_strconcat ("png",NULL);
			gchar *ext2 = g_strconcat ("jpg",NULL);
			gchar *ext3 = g_strconcat ("gif",NULL);
			cdcover_config.cover_extensions = g_slist_append(cdcover_config.cover_extensions,ext1);
			cdcover_config.cover_extensions = g_slist_append(cdcover_config.cover_extensions,ext2);
			cdcover_config.cover_extensions = g_slist_append(cdcover_config.cover_extensions,ext3);
		}

		// Free config handle
		aud_cfg_db_close (config);
	} else {
		DPRINT (__DEBUG_GENERAL__,"Cannot open config file");
	}
}

void write_xmms_config() {
	/*char tempname[100];*/
	char *tempname = (char *)malloc(sizeof(char)*100);
	int count;
	GSList *position = cdcover_config.cover_searchpaths;
	GSList *positionext = cdcover_config.cover_extensions;
	mcs_handle_t *config;

	DPRINT (__DEBUG_GENERAL__,"Writing config");

	config = aud_cfg_db_open ();
	if (config) {
		// Window position
		aud_cfg_db_set_bool (config,PLUGIN_NAME,"savewindowpos",cdcover_config.save_window_pos);
		aud_cfg_db_set_int  (config,PLUGIN_NAME,"windowposx",cdcover_config.winpos_x);
		aud_cfg_db_set_int  (config,PLUGIN_NAME,"windowposy",cdcover_config.winpos_y);

		// Aspect ratio
		aud_cfg_db_set_bool (config,PLUGIN_NAME,"aspectratio",cdcover_config.preserve_aspectratio);

		// Iterate through the search list and save the paths
		count=0;
		while (position!=NULL) {
			count++;
			sprintf (tempname,"path%d",count);
			aud_cfg_db_set_string (config,PLUGIN_NAME,tempname,position->data);
			//printf("Wrote path %s (%d, %s)\n", (char *)position->data, count, tempname);
			position = g_slist_next (position);
		}
		//printf("Done writing paths\n");

		// Delete the next key, so there's a hole and read_xmms_config can stop here
		// Not too nice, as we probably leave garbage in the xmms config file
		sprintf (tempname,"path%d",count+1);
		aud_cfg_db_unset_key (config,PLUGIN_NAME,tempname);

		// Iterate through the search list and save the extensions
		count=0;
		//printf("About to write extensions\n");
		while (positionext!=NULL) {
			count++;
			sprintf (tempname,"ext%d",count);
			aud_cfg_db_set_string (config,PLUGIN_NAME,tempname,positionext->data);
			//printf("Wrote extension %s (%d, %s)\n", (char *)positionext->data, count, tempname);
			positionext = g_slist_next (positionext);
		}
		//printf("Done with the extensions too\n");

		// Delete the next key, so there's a hole and read_xmms_config can stop here
		// Not too nice, as we probably leave garbage in the xmms config file
		sprintf (tempname,"ext%d",count+1);
		aud_cfg_db_unset_key (config,PLUGIN_NAME,tempname);

		// Save the skin
		if (cdcover_config.skin_path!=NULL) {
			// Save the user selected skin
			aud_cfg_db_set_string (config,PLUGIN_NAME,"skinpath",cdcover_config.skin_path);
		} else {
			// Built in default skin, delete the key
			aud_cfg_db_unset_key (config,PLUGIN_NAME,"skinpath");
		}

		// Write and then free the config
		//printf("About to close config file\n");
		aud_cfg_db_close (config);
		//printf("Closed config file\n");
	} else {
		DPRINT (__DEBUG_GENERAL__,"cannot open config file for writing");
	}
}

t_cdcover_config *get_xmms_config() {
	return &cdcover_config;
}

void free_xmms_config() {
	GSList *position = cdcover_config.cover_searchpaths;
	GSList *positionext = cdcover_config.cover_extensions;

	// Free the strings in the list
	while (position!=NULL) {
		g_free (position->data);
		position = g_slist_next (position);
	}
	g_slist_free (cdcover_config.cover_searchpaths);
	while (positionext!=NULL) {
		g_free (positionext->data);
		positionext = g_slist_next (positionext);
	}
	g_slist_free (cdcover_config.cover_extensions);
	// Free skin path
	free (cdcover_config.skin_path);
	cdcover_config.cover_searchpaths = NULL;
	cdcover_config.cover_extensions = NULL;
	// Free skin pixbuf
	g_object_unref (cdcover_config.skin_base_custom_pixbuf);
	g_object_unref (cdcover_config.skin_nocover_custom_pixbuf);
	cdcover_config.skin_base_custom_pixbuf = NULL;
	cdcover_config.skin_nocover_custom_pixbuf = NULL;
}
