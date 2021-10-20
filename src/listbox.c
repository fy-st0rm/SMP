#include "listbox.h"


ListBox* new_list_box(const char* title, int x, int y, int w, int h)
{
	ListBox* list_box = malloc(sizeof(ListBox));
	
	// Setting the variables
	list_box->title = title;
	list_box->x = x;
	list_box->y = y;
	list_box->w = w;
	list_box->h = h;

	list_box->selected = 0;
	list_box->focus = false;

	list_box->surface = newwin(list_box->h, list_box->w, list_box->y, list_box->x);

	return list_box;
}

void draw_list_box(ListBox* list_box)
{
	wclear(list_box->surface);

	wattrset(list_box->surface, COLOR_PAIR(BORDER));
	
	box(list_box->surface, '|', '-');
	touchwin(list_box->surface);
	wrefresh(list_box->surface);
	
	wattroff(list_box->surface, COLOR_PAIR(BORDER));
	
	// Adding title
	int len = strlen(list_box->title);
	int x = list_box->w / 2 - (len + 2) / 2;
	
	wattrset(list_box->surface, COLOR_PAIR(BORDER));
	mvwaddstr(list_box->surface, 0, x, "|");
	wrefresh(list_box->surface);
	wattroff(list_box->surface, COLOR_PAIR(BORDER));
	x++;

	wattrset(list_box->surface, COLOR_PAIR(TITLE));
	mvwaddstr(list_box->surface, 0, x, list_box->title);
	wrefresh(list_box->surface);
	wattroff(list_box->surface, COLOR_PAIR(TITLE));
	x += len;

	wattrset(list_box->surface, COLOR_PAIR(BORDER));
	mvwaddstr(list_box->surface, 0, x, "|");
	wrefresh(list_box->surface);
	wattroff(list_box->surface, COLOR_PAIR(BORDER));
	x++;
}

void relocate_list_box(ListBox* list_box, int x, int y, int w, int h)
{
	list_box->x = x;
	list_box->y = y;
	list_box->w = w;
	list_box->h = h;
	
	delwin(list_box->surface);
	list_box->surface = newwin(list_box->h, list_box->w, list_box->y, list_box->x);
}

void fill_list_box(ListBox* list_box, char** items, int size)
{
	for (int i = 0; i < size; i++)
	{
		char* item = items[i];
		int len = strlen(item);
		
			
		if (len < list_box->w)
		{
			// Adding spaces to the item names
			int diff = list_box->w - (len) - 2;
			char* temp = malloc(100);	// Hardcoded 100 to resolve the dynamic allocation crashes
			for (int i = 0; i < sizeof(temp); i++) temp[i] = '\0';
			
			strcat(temp, item);	
		
			for (int i = 0; i < diff; i++)
			{
				strcat(temp, " ");
			}
			
			item = temp;
		}
			
		else if (len >= list_box->w)
		{
			// Reducing the sizeof the string when the listbox is small
			if (list_box->w > 1)
			{
				char* temp = strndup(item, list_box->w - 2);
				item = temp;
			}
		}
		
		// Rendering the text inside the listbox
		if (i == list_box->selected)	
			wattrset(list_box->surface, COLOR_PAIR(SELECTED));

		mvwaddstr(list_box->surface, i+1, 1, item);
		wrefresh(list_box->surface);
	
		if (i == list_box->selected)	
			wattroff(list_box->surface, COLOR_PAIR(SELECTED));
	}
	wrefresh(list_box->surface);
}

