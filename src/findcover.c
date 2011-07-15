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

#include <stdio.h>		// Some standard libraries
#include <stdlib.h>		// mem-functions
#include <string.h>		// String-Functions
#include <glib.h>		// GSList, string functions
#include <unistd.h>		// open/close file
#include <sys/types.h>		// needed for file access
#include <sys/stat.h>		// needed for file access
#include <dirent.h>		// neeede for file access (readdir)
#include <fcntl.h>		// needed for file access
#include <libgen.h>		// dirname/basename
#include <fnmatch.h>		// fnmatch for filename wildcards
#include "configmgm.h"		// Configuration management
#include "findcover.h"		// Headers for this file
#include "globals.h"		// some global options
#include "debug.h"		// Debuging



char *findcover_filename = NULL;

char *strReplace (char *text, char *token, char *replace) {
	char *target, *newtarget;

	// Check if we have a valid text string
	if (text==NULL || strlen(text)==0) { return NULL; }

	// Copy the new target string as work string
	target = strdup (text);

	// Check if we have valid tokens and replace strings
	if (replace==NULL || strlen(replace)==0) { return target; }
	if (token==NULL   || strlen(token)==0)   { return target; }
	// Check if the token we replace is not in the replacement
	// Otherwise we well end up in an endless loop with a memory leak
	if (strstr(replace,token)) { return target; }

	// Not nice this way
	free (text);

	while (strstr(target,token)) {
		// Allocate space for the new replaced string
		newtarget = (char *) malloc (strlen(target)-strlen(token)+strlen(replace)+1);
		memset (newtarget,0,strlen(target)-strlen(token)+strlen(replace)+1);
		// Copy first half
		strncat (newtarget,target,strstr(target,token)-target);
		// Now the replacement-token
		strcat (newtarget,replace);
		// The rest after the replacement
		strcat (newtarget,strstr(target,token)+strlen(token));
		// Free old target and make the new target the default
		free (target);
		target = newtarget;
	}

	// Return the new target
	return target;
}

char *get_coverfilename (char *filename, char *mask) {
	int x,tmp_length;
	int slash_last,slash_secondlast;
	char *path_filename, *path_filename_woext, *path_directory, *path_base;
	char *posSeparator;
	char *result;
	path_filename=path_filename_woext=path_directory=path_base = NULL;
	slash_last=slash_secondlast = -1;

	// Find the last and the before last slash, if possible
	for (x=0;x<strlen(filename);x++) {
		if (filename[x]==DIR_SEPARATOR) {
			slash_secondlast = slash_last;
			slash_last = x;
		}
	}

	if (slash_last==-1) { slash_last = 0; }

	// Directory
	if (slash_last>0) {
		tmp_length = slash_last;
		path_directory = (char *) malloc (tmp_length+1);
		memcpy (path_directory,filename,tmp_length);
		path_directory[tmp_length] = 0;
	}

	// Basename
	if (path_directory && slash_secondlast!=-1) {
		tmp_length = slash_last - slash_secondlast;
		path_base = (char *) malloc (tmp_length+1);
		strcpy (path_base,&path_directory[slash_secondlast+1]);
	}

	// Filenname
	tmp_length = strlen(filename)-slash_last-1;
	path_filename = (char *) malloc (tmp_length+1);
	strcpy (path_filename,&filename[slash_last+1]);

	// Filename without extension
	if (rindex(path_filename,SEPARATOR_FILEEXTENSION)) {
		// We have found a separator
		posSeparator = rindex(path_filename,SEPARATOR_FILEEXTENSION);
	} else {
		// No extension found, so filename and filename
		// without extension are the same
		posSeparator = path_filename+strlen(path_filename);
	}
	tmp_length = posSeparator-path_filename;
	path_filename_woext = (char *) malloc (tmp_length+1);
	memcpy (path_filename_woext,path_filename,tmp_length);
	path_filename_woext[tmp_length] = 0;

	result = (char *) malloc (strlen(mask)+1);
	strcpy (result,mask);
	result = strReplace (result,SEARCH_TOKEN("PATH"),path_directory);
	result = strReplace (result,SEARCH_TOKEN("BASE"),path_base);
	result = strReplace (result,SEARCH_TOKEN("FILENAME"),path_filename_woext);
	result = strReplace (result,SEARCH_TOKEN("FILENAME-EXT"),path_filename);

	free (path_directory);
	free (path_base);
	free (path_filename);
	free (path_filename_woext);

	return result;
}

int findcover_fileexists (char *filename) {
	int hFile,result;
	struct stat filestat;
	result = 0;

	// Check if filename exists
	hFile = open (filename,O_RDONLY);
	if (hFile>0) {
		// File exists, lets see if it has a filesize>0
		if (fstat(hFile,&filestat)!=-1) {
			if (filestat.st_size > 0) {
				result = 1;
			}
		}
		close (hFile);
	}

	return result;
}

char *findcover_search (char *filename) {
	GSList *position;
	GSList *extposition;
	char *testFile;
	char *totalFile;
	int path_ok;
	t_cdcover_config *plugin_config;

	// Get a reference to the config
	plugin_config = get_xmms_config();
	position = plugin_config->cover_searchpaths;
	extposition = plugin_config->cover_extensions;


	while (position!=NULL)
	{
		char *cover_dirname,*cover_dirname_tmp,*cover_basename,*cover_basename_tmp;
		DIR *dirhandle;
		struct dirent *direntry;

		while (extposition != NULL)
		{
			totalFile = (char *)malloc(sizeof(char)*(strlen((char *)position->data)+strlen((char *)extposition->data)+1)+1);
			sprintf(totalFile, "%s.%s", (char *)position->data, (char *)extposition->data);
			/*printf("Checking for '%s'\n", totalFile);*/

			path_ok = 0;
			// Expand the token parameters
			testFile = get_coverfilename (filename,totalFile);
			// If we have a wildcard in our filename, we have to scan the whole
			// directory, so check if we have one
			cover_dirname_tmp  = strdup(testFile);
			cover_basename_tmp = strdup(testFile);
			cover_dirname  = dirname (cover_dirname_tmp);
			cover_basename = basename (cover_basename_tmp);
			if (strcmp(cover_basename,"*")) {
				// We have a wildcard, so lets search the directory for it
				dirhandle = opendir (cover_dirname);
				if (dirhandle>0) {
					while ((direntry=readdir(dirhandle))) {
						if (fnmatch(cover_basename,direntry->d_name,0)==0) {
							char *newFile, *oldFile;
							// We found a real matching file, so allocate new
							// space for it
							newFile = g_strconcat (cover_dirname,"/",&direntry->d_name,NULL);
							oldFile = testFile;
							testFile = newFile;
							free (oldFile);
							path_ok = 1;
							break;
						}
					}
					closedir (dirhandle);
				} else {
					DPRINT (__DEBUG_FILE__,"Error opening dir '%s'",cover_dirname);
				}
			} else {
				path_ok = findcover_fileexists (testFile);
			}
			free (cover_dirname_tmp);
			free (cover_basename_tmp);

			if (path_ok) {
				// We have a valid path
				free (findcover_filename);
				findcover_filename = testFile;
				/*printf("Found cover %s\n", findcover_filename);*/
				return findcover_filename;
			} else {
				// This path failed, free the path
				free (testFile);
			}

			free(totalFile);

			// Move to next extension
			extposition = g_slist_next (extposition);
		}

		// Move to next searchpaths
		position = g_slist_next (position);
		extposition = plugin_config->cover_extensions;
	}

	free (findcover_filename);
	findcover_filename = NULL;
	// Noting found
	return NULL;
}

void findcover_init() {
}

void findcover_done() {
	free (findcover_filename);
	findcover_filename = NULL;
}


