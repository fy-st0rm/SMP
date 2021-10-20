#include "listbox.h"
#include "config.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <vlc/vlc.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

// Definations
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/*
 * GLOBAL VARIABLES
 */

// UI
static ListBox* track_list;
static ListBox* queue_list;

// Media player global variables
static libvlc_instance_t* vlc_inst;
static libvlc_media_player_t* vlc_mp;
static libvlc_media_t* vlc_m;

// Flags
static bool player_running;
static pthread_t player_thread;

static bool play;
static bool stopped;
static bool queue_loop;

/*
 * FUNCTIONS
 */

// Init and quit functions
void init();
void init_colors();
void quit();

// UI
void render();

// core
void extract_url(const char* key);
void create_daemon();
void close_daemon();
void* player_daemon();
void launch_music();


/*
 * Init and quir function declarations
 */

void init()
{
	initscr();

	noecho();
	curs_set(0);
	refresh();

	if (has_colors())
		init_colors();
	
	init_songs();
}

void init_colors()
{
	start_color();
	use_default_colors();

	init_pair(SELECTED, selected_color[0], selected_color[1]);
	init_pair(BORDER, border_color[0], border_color[1]);
	init_pair(TITLE, title_color[0], title_color[1]);	
}

void quit()
{
	dict_free(song_url);
	curs_set(1);
	endwin();
}


/*
 * Core function declaration
 */

void extract_url(const char* key)
{
	char* url = dict_get(song_url, key);
	char* cmd = "python /usr/bin/smpd.py -l ";
	
	// Creating the full cmd
	int cmd_size = strlen(cmd) + strlen(url) + 1;
	char full_cmd[cmd_size]; //(char*)malloc(sizeof(url) + sizeof(cmd));
	for (int j = 0; j < cmd_size; j++) full_cmd[j] = '\0';
	
	strcat(full_cmd, cmd);
	strcat(full_cmd, url);
	
	// Reading from stdout	
	FILE *fp;
	int status;
	char output[1048];

	// Open the command for reading. 
	fp = popen(full_cmd, "r");
	if (fp == NULL) {
		printf("Failed to run command\n" );
		exit;
	}

	// Read the output a line at a time
	int i = 1;
	while (fgets(output, sizeof(output), fp) != NULL)
	{
		sleep(0.1);	// To sync with python program	
		if (i % 2 != 0)
		{
			//printf("%d)- %s\n", i, output);
			char* title = strtok(output, "]");
			char* url = strtok(NULL, "]");
			url = strtok(url, "\n");
				
			char* new_title = calloc(strlen(title)+1, sizeof(char*));
			char* new_url = calloc(strlen(url)+1, sizeof(char*));

			strcpy(new_title, title);
			strcpy(new_url, url);

			dict_append(queue, new_title, new_url);
		}
		i++;
	}
	pclose(fp);
}


/*
 * Music player
 */

void launch_music()
{
	char* url = dict_get_i(queue, 0); 
	vlc_m = libvlc_media_new_location(vlc_inst, url);
	vlc_mp = libvlc_media_player_new_from_media(vlc_m);
		
	libvlc_media_release(vlc_m);

	libvlc_media_player_play(vlc_mp);

	sleep(5);
}

void* player_daemon()
{
	while(player_running)
	{
		time_t start = time(NULL);

		if (play)
		{
			if ((vlc_mp) && (stopped))
			{
				libvlc_media_player_pause(vlc_mp);
				stopped = false;
			}
			else
			{
				// Poping the music thats been finished playing
				if ((vlc_mp) && (queue->len > 0))
					if (!libvlc_media_player_is_playing(vlc_mp))
					{
						const char* keys[queue->len];
						dict_get_keys(queue, keys);

						if (queue_loop)
						{
							char* val = (char*) calloc(1, sizeof(dict_get(queue, keys[0])));
							strcpy(val, dict_get(queue, keys[0]));
							dict_append(queue, keys[0], val);
							dict_pop(queue, keys[0]);
						}
						else
							dict_pop(queue, keys[0]);
						
						render();
					}
				
				if (queue->len != 0)
				{
					// Launching the music
					if (!vlc_mp)
						launch_music();
					else
					{
						if (!libvlc_media_player_is_playing(vlc_mp))
						{
							launch_music();
						}
					}
				}
				else
				{
					// If the queue is empty. Stop
					play = false;

					// Emptying the media player
					libvlc_media_player_release(vlc_mp);
					vlc_mp = NULL;

					render();
				}
			}
		}

		sleep(MAX(1/.25 - (time(NULL) - start), 0));
	}
}

void create_daemon()
{
	// Creating a new instance of vlc
	vlc_inst = libvlc_new(0, NULL);
	
	// Declaring all the media player vars	
	player_running = true;
	play = false;
	stopped = false;
	queue_loop = false;
	
	int err = pthread_create(&player_thread, NULL, player_daemon, NULL);
	if (err)
	{
		printf("ERROR: Failed to start player daemon!");
		exit(1);
	}
}

void close_daemon()
{
	player_running = false;
	pthread_join(player_thread, NULL);
}


/*
 * Rendering functions
 */

void render()
{
	clear();
	refresh();

	// Getting the window size
	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	
	// Rendering listboxes
	draw_list_box(track_list);
	draw_list_box(queue_list);

	// Adding the track list
	const char* items[song_url->len];
	dict_get_keys(song_url, items);
	fill_list_box(track_list, items, song_url->len);
		
	// Adding the queue list
	if (queue->len > 0)
	{
		const char* names[queue->len];
		dict_get_keys(queue, names);
		fill_list_box(queue_list, names, queue->len);
	}

	// Drawing currently playing music
	mvaddstr(track_list->h + 2, track_list->x + 2, "> ");
	refresh();

	if (play)
	{
		const char* names[queue->len];
		dict_get_keys(queue, names);
		
		mvaddstr(track_list->h + 2, track_list->x + 4, names[0]);
		refresh();
	}
	
	// Drawing values
	
	// Status
	char status[17];
	snprintf(status, sizeof(status), "[State: %s]", play ? "Playing" : "Paused");
	mvaddstr(track_list->h + 4, size.ws_col / 5 - 18 / 2, status);
	refresh();	
	
	// Queue loop
	char loop[12];
	snprintf(loop, sizeof(loop), "[Loop: %s]", queue_loop ? "Yes" : "No");
	mvaddstr(track_list->h + 4, size.ws_col / 2.5 - 12 / 2, loop);
	refresh();

	// Volume
	char volume_percent[14];
	snprintf(volume_percent, sizeof(volume_percent), "[Volume: %d]", volume);
	mvaddstr(track_list->h + 4, size.ws_col / 1.7 - 14 / 2, volume_percent);
	refresh();
	
	// Queue total
	char queue_total[14];
	snprintf(queue_total, sizeof(queue_total), "[Queue: %d]", queue->len);	
	mvaddstr(track_list->h + 4, size.ws_col / 1.2 - 14 / 2, queue_total);
	refresh();
}


/*
 * Main function
 */
/*
int main()
{
	init_songs();

	extract_url("anson2");
	extract_url("anson2");
	
	printf("%d", queue->len);

	return 0;
}*/


int main()
{
	// Initializations
	init();
	
	// Starting the player daemon
	create_daemon();

	// Getting the window size
	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

	// Generating listbox
	track_list = new_list_box("Tracks", 2, 1, size.ws_col / 2 - 4, size.ws_row / 1.2 );
	queue_list = new_list_box("Queue", size.ws_col / 2, 1, size.ws_col / 2 - 2, size.ws_row / 1.2);

	// Enabling keyinput in track list
	keypad(track_list->surface, TRUE);

	// Track list	
	const char* items[song_url->len];
	dict_get_keys(song_url, items);

	// Main loop
	bool main_loop = true;
	while (main_loop)
	{
		// Rendering all the ui
		render();

		int key = wgetch(track_list->surface);
		switch(key)
		{
			case KEY_RESIZE: 
			{	
				// Handling resize event
				clear();
				refresh();
				
				ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
				relocate_list_box(track_list, 2, 1, size.ws_col / 2 - 4, size.ws_row / 1.2);
				relocate_list_box(queue_list, size.ws_col / 2, 1, size.ws_col / 2 - 2, size.ws_row / 1.2);

				draw_list_box(track_list);
				draw_list_box(queue_list);
				
				keypad(track_list->surface, TRUE);

				break;
			}

			case KEY_DOWN:
			{
				if (track_list->selected < song_url->len - 1)
					track_list->selected++;
				break;
			}

			case KEY_UP:
			{
				if (track_list->selected)	
					track_list->selected--;
				break;
			}
			
			// 10 == enter
			case 10:
			{
				extract_url(items[track_list->selected]);
				break;
			}
			
			// 112 == "p"	[To play or pause]	
			case 112:
			{
				if (queue->len > 0)
				{
					if (play)
					{
						play = false;

						if (vlc_mp)
						{
							libvlc_media_player_pause(vlc_mp);
							stopped = true;
						}
					}
					else
						play = true;
				}
				break;
			}
						
			// 113 == 'q'	[To exit out of the player'
			case 113:
			{
				main_loop = false;
				break;
			}
		
			// 114 == 'r'	[To reset the queue]
			case 114:
			{
				if (vlc_mp)	libvlc_media_player_pause(vlc_mp);
				dict_clear(queue);
				break;
			}
			
			// 115 == 's'	[To skip a song]
			case 115:
			{
				if (play)
				{
					libvlc_media_player_stop(vlc_mp);
					libvlc_media_player_release(vlc_mp);
					vlc_mp = NULL;
					
					const char* keys[queue->len];
					dict_get_keys(queue, keys);

					dict_pop(queue, keys[0]);
				}
				break;
			}

			// 108 == 'l' 	[To enable or disable loop]
			case 108:
			{
				if (queue->len > 0)
				{
					if (queue_loop)
						queue_loop = false;
					else
						queue_loop = true;
				}
				break;
			}

			// 61 == '='	[To increase the volume]
			case 61:
			{
				if (volume < 100) volume++;
				if (vlc_mp) libvlc_audio_set_volume(vlc_mp, volume);
				break;
			}

			// 45 == '-'	[To decrease the volume]
			case 45:
			{
				if (volume > 0) volume--;
				if (vlc_mp) libvlc_audio_set_volume(vlc_mp, volume);
				break;
			}
		}
	}

	// Releasing vlc's
	libvlc_release(vlc_inst);
	
	close_daemon();
	quit();
	return 0;
}
