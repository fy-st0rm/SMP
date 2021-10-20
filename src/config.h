#ifndef _CONFIG_H_
#define _CONFIG_H_

//WARNING: Dont change the includes
#include <ncurses.h>
#include "dict.h"

// Color definations
#define SELECTED 1
#define BORDER   2
#define TITLE	 3

// Colors
static int selected_color[2] = {COLOR_BLACK, 5};
static int border_color[2] 	 = {4, -1};
static int title_color[2]	 = {3, -1};

// Default volume
static int volume = 60;

// Songs path
static Dict* song_url;
static Dict* queue;

static void init_songs()
{
	song_url = new_dict();
	queue = new_dict();
	dict_append(song_url, "anson", "https://www.youtube.com/playlist?list=PLukGxamtDUYEAAo3zuYzHjmismnDhSTe2");
	dict_append(song_url, "hayd", "https://www.youtube.com/playlist?list=PLukGxamtDUYGndSU06x1dQClgPk8kIErT");
	dict_append(song_url, "yaeow", "https://www.youtube.com/playlist?list=PLukGxamtDUYEsS7xxW9rYqmi02IeRZuGH");
	dict_append(song_url, "zevia", "https://www.youtube.com/playlist?list=PLukGxamtDUYGLCRSKCm-rl-h9X4DLYTxF");
	dict_append(song_url, "mix", "https://www.youtube.com/playlist?list=PLukGxamtDUYG3iH3f5Mwnx4ZoxqYJhKI-");
}


#endif
