#ifndef _DICT_H_
#define _DICT_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/* 
 * Custom dictionary struct
 */

typedef struct dict_entry
{
	const char* key;
	const char* val;
} dict_entry;

typedef struct dict
{
	int len;
	dict_entry* entries;	
} Dict;


Dict* new_dict();
void dict_append(Dict* dict, const char* key, const char* val);
bool dict_exists(Dict* dict, const char* key);
const char* dict_get(Dict* dict, const char* key);
const char* dict_get_i(Dict* dict, int index);
void dict_get_keys(Dict* dict, const char** keys);
void dict_pop(Dict* dict, const char* key);
void dict_pop_i(Dict* dict, int index);
void dict_clear(Dict* dict);
void dict_free(Dict* dict);

#endif
