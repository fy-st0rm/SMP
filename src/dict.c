#include "dict.h"

Dict* new_dict()
{
	Dict* new = malloc(sizeof(Dict));
	new->len = 0;
	return new;
}

void dict_append(Dict* dict, const char* key, const char* val)
{
	dict->entries = realloc(dict->entries, sizeof(dict_entry) * (dict->len + 1));	// Extending the allocation
	//dict->entries[dict->len].key = (char*) malloc(sizeof(char) * strlen(key));
	//dict->entries[dict->len].val = (char*) malloc(sizeof(char) * strlen(val));

	dict->entries[dict->len].key = key;
	dict->entries[dict->len].val = val;
	dict->len++;
}

bool dict_exists(Dict* dict, const char* key)
{
	for (int i = 0; i < dict->len; i++)
	{
		if (dict->entries[i].key == key)
			return true;
	}
	return false;
}

const char* dict_get(Dict* dict, const char* key)
{
	for (int i = 0; i < dict->len; i++)
	{
		if (dict->entries[i].key == key)
		{
			return dict->entries[i].val;
		}
	}
	return NULL;
}

const char* dict_get_i(Dict* dict, int index)
{
	return dict->entries[index].val;
}

void dict_get_keys(Dict* dict, const char** keys)
{
	for (int i = 0; i < dict->len; i++)
	{
		keys[i] = dict->entries[i].key;
	}
}

void dict_pop(Dict* dict, const char* key)
{
	dict_entry* new_entries = malloc(sizeof(dict_entry) * (dict->len - 1));

	int count = 0;
	for (int i = 0; i < dict->len; i++)
	{
		if (key != dict->entries[i].key)
		{
			new_entries[count].key = dict->entries[i].key;
			new_entries[count].val = dict->entries[i].val;
			count++;
		}

	}

	dict->len--;
	dict->entries = new_entries;
}

void dict_pop_i(Dict* dict, int index)
{
	dict_entry* new_entries = malloc(sizeof(dict_entry) * (dict->len - 1));

	int count = 0;
	for (int i = 0; i < dict->len; i++)
	{
		if (i != index)
		{
			new_entries[count].key = dict->entries[i].key;
			new_entries[count].val = dict->entries[i].val;
			count++;
		}
	}

	dict->len--;
	dict->entries = new_entries;
}

void dict_clear(Dict* dict)
{
	dict->len = 0;
	free(dict->entries);
	dict->entries = NULL;
}

void dict_free(Dict* dict)
{
	if (dict->entries != NULL)
		free(dict->entries);
	free(dict);
}
