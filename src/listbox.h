#ifndef _LISTBOX_H_
#define _LISTBOX_H_


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <ncurses.h>

#include "config.h"

// Listbox object
struct __list_box
{
	int x, y, w, h, selected;
	bool focus;
	WINDOW* surface;
	const char* title;
};
typedef struct __list_box ListBox;

// Sortof constructor for listbox
ListBox* new_list_box(const char* title, int x, int y, int w, int h);


// Listbox util functions
void draw_list_box(ListBox* list_box);
void relocate_list_box(ListBox* list_box, int x, int y, int w, int h);
void fill_list_box(ListBox* list_box, char** items, int size);

#endif
