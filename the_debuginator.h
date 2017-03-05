#ifndef INCLUDE_THE_DEBUGINATOR_H
#define INCLUDE_THE_DEBUGINATOR_H

/*
# THE DEBUGINATOR

## Usage
	// Create
	static void on_debuginator_save(DebuginatorItemDefinition* item, void* value, const char* value_title) {
		game_save_debug_setting(item->path, value_title);
	}

	DebuginatorItemDefinition item_buffer[256];
	TheDebuginator debuginator = debuginator_create(item_buffer, 256, on_debuginator_save);

	// Load settings at startup
	void parse_command_line(int argc, char** argv) {
		if (argc > 2 && strcmp(argv[1], "--do_the_bool") == 0) {
			debuginator_pre_load_setting(debuginator, "Test/Bool with function callback", "True");
		}
	}

	// Add debug menu item for reals
	// (can happen later, and multiple times if you want to override something)
	static void on_item_changed(DebuginatorItemDefinition* item, void* value, const char* value_title) {
		bool new_value = *(bool*)value;
		printf("Item %s changed value to (%d) %s", item->title, value, value_title);
	}

	DEBUGINATOR_create_bool_item(debuginator, "Test/Bool with function callback",
	"Calls on_item_changed when user changes the value in the menu.",
		on_item_changed, NULL);

	// Validate configuration
	// debuginator_validate(&debuginator);

	// ASCII banners generated using
	// http://patorjk.com/software/taag/#p=display&f=ANSI%20Shadow&t=update
*/

#ifndef DEBUGINATOR_assert
#include <assert.h>
#define DEBUGINATOR_assert assert;
#endif

#ifndef DEBUGINATOR_memcpy
#include <string.h>
#define DEBUGINATOR_memcpy memcpy
#endif

#ifndef DEBUGINATOR_fabs
#include <math.h>
#define DEBUGINATOR_fabs fabs
#endif

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifndef DEBUGINATOR_FREE_LIST_CAPACITY
#define DEBUGINATOR_FREE_LIST_CAPACITY 256
#endif

#ifndef DEBUGINATOR_max_title_length
#define DEBUGINATOR_max_title_length 20
#endif

#ifndef DEBUGINATOR_max_themes
#define DEBUGINATOR_max_themes 16
#endif

#ifndef DEBUGINATOR_debug_print
#define DEBUGINATOR_debug_print 
#endif

// TODO C99-ify these
typedef struct DebuginatorVector2 {
	float x;
	float y;
} DebuginatorVector2;

typedef struct DebuginatorColor {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} DebuginatorColor;

typedef struct DebuginatorFont {
	void* userdata;
	int size;
	bool bold;
	bool italic;
} DebuginatorFont;

DebuginatorVector2 debuginator__vector2(float x, float y) {
	DebuginatorVector2 v; v.x = x; v.y = y;
	return v;
}

DebuginatorColor debuginator__color(int r, int g, int b, int a) {
	DEBUGINATOR_assert(0 <= r && r <= 255);
	DEBUGINATOR_assert(0 <= g && g <= 255);
	DEBUGINATOR_assert(0 <= b && b <= 255);
	DEBUGINATOR_assert(0 <= a && a <= 255);
	DebuginatorColor c; 
	c.r = (unsigned char)r; c.g = (unsigned char)g; 
	c.b = (unsigned char)b; c.a = (unsigned char)a;
	return c;
}


typedef enum DebuginatorDrawTypes {
	DEBUGINATOR_Background,
	DEBUGINATOR_FolderTitle,
	DEBUGINATOR_ItemTitle,
	DEBUGINATOR_ItemTitleOverridden,
	DEBUGINATOR_ItemTitleHot,
	DEBUGINATOR_ItemTitleActive,
	DEBUGINATOR_ItemDescription,
	DEBUGINATOR_LineHighlight,
	DEBUGINATOR_ItemValueDefault,
	DEBUGINATOR_ItemValueOverridden,
	DEBUGINATOR_ItemValueHot,
	DEBUGINATOR_NumDrawTypes
} DebuginatorDrawTypes;

typedef struct DebuginatorTheme {
	DebuginatorColor colors[DEBUGINATOR_NumDrawTypes];
	DebuginatorFont fonts[DEBUGINATOR_NumDrawTypes];
} DebuginatorTheme;

typedef struct DebuginatorFolderData {
	struct DebuginatorItem* first_child;
	struct DebuginatorItem* hot_child;
} DebuginatorFolderData;

typedef struct DebuginatorItem DebuginatorItem;

typedef void (*DebuginatorDrawTextCallback)
	(const char* text, DebuginatorVector2* position, DebuginatorColor* color, DebuginatorFont* font, void* userdata);
typedef void (*DebuginatorDrawRectCallback)
	(DebuginatorVector2 position, DebuginatorVector2 size, DebuginatorColor color, void* userdata);
typedef const char* (*DebuginatorWordWrapCallback)
	(const char* text, DebuginatorFont font, float max_width, char* buffer, int buffer_size, void* userdata);

typedef void (*DebuginatorItemQuickDrawCallback)(DebuginatorItem* item, void* data);
typedef void (*DebuginatorOnItemChangedCallback)(DebuginatorItem* item, void* value, const char* value_title);

typedef struct DebuginatorLeafData {
	const char* description;

	bool is_active;
	char* hot_value;
	int hot_index;
	int active_index;

	const char** value_titles;
	const char** value_descriptions;
	void* values;

	int num_values;
	size_t array_element_size;

	DebuginatorItemQuickDrawCallback quick_draw_callback;
	DebuginatorOnItemChangedCallback on_item_changed_callback;
} DebuginatorLeafData;

typedef struct DebuginatorItem {
	char title[DEBUGINATOR_max_title_length];
	bool is_folder;
	void* user_data;

	DebuginatorItem* prev_sibling;
	DebuginatorItem* next_sibling;
	DebuginatorItem* parent;

	union {
		DebuginatorLeafData leaf;
		DebuginatorFolderData folder;
#pragma warning(suppress: 4201) // Unnamed union
	};
} DebuginatorItem;

typedef struct TheDebuginatorConfig {
	bool create_default_debuginator_items;

	size_t item_buffer_capacity;
	DebuginatorItem* item_buffer;

	DebuginatorTheme themes[DEBUGINATOR_max_themes];
	
	void* draw_user_data;
	DebuginatorDrawTextCallback draw_text;
	DebuginatorDrawRectCallback draw_rect;
	DebuginatorWordWrapCallback word_wrap;

	DebuginatorVector2 size;
	DebuginatorVector2 root_position;
	int open_direction;
	float focus_height;
} TheDebuginatorConfig;

typedef struct TheDebuginator {
	DebuginatorItem* root;
	DebuginatorItem* hot_item;

	size_t item_buffer_capacity;
	size_t item_buffer_size;
	DebuginatorItem* item_buffer;

	size_t free_list_size;
	size_t free_list[DEBUGINATOR_FREE_LIST_CAPACITY];

	bool is_open;
	float openness_timer; // range [0,1]
	float openness; // range [0,1]
	
	DebuginatorTheme themes[DEBUGINATOR_max_themes];
	DebuginatorTheme theme; // current theme
	int theme_index;

	void* draw_user_data;
	DebuginatorDrawTextCallback draw_text;
	DebuginatorDrawRectCallback draw_rect;
	DebuginatorWordWrapCallback word_wrap;

	DebuginatorVector2 size;
	DebuginatorVector2 root_position;
	int open_direction;
	float focus_height;
	float current_height_offset;
} TheDebuginator;

DebuginatorItem* debuginator_get_free_item(TheDebuginator* debuginator) {
	DebuginatorItem* item;
	if (debuginator->free_list_size > 0) {
		size_t free_index = debuginator->free_list[--debuginator->free_list_size];
		DEBUGINATOR_assert(0 <= free_index && free_index < debuginator->item_buffer_capacity);
		item = &debuginator->item_buffer[free_index];
	}
	else {
		DEBUGINATOR_assert(debuginator->item_buffer_size < debuginator->item_buffer_capacity);
		item = &debuginator->item_buffer[debuginator->item_buffer_size++];
	}

	memset(item, 0, sizeof(*item));
	return item;
}

static void on_change_theme(DebuginatorItem* item, void* value, const char* value_title) {
	(void)value_title;
	TheDebuginator* debuginator = (TheDebuginator*)item->user_data;
	debuginator->theme_index = *(int*)value;
	debuginator->theme = debuginator->themes[debuginator->theme_index];
}

void debuginator_set_title(DebuginatorItem* item, const char* title, size_t title_length) {
	if (title_length == 0) {
		title_length = strlen(title);
	}

	if (title_length >= DEBUGINATOR_max_title_length) {
#pragma warning(suppress: 4996)
		strncpy(item->title, title, DEBUGINATOR_max_title_length - 3);
		item->title[DEBUGINATOR_max_title_length - 3] = '.';
		item->title[DEBUGINATOR_max_title_length - 2] = '.';
		item->title[DEBUGINATOR_max_title_length - 1] = '\0';
	}
	else {
#pragma warning(suppress: 4996)
		strncpy(item->title, title, title_length);
	}
}

void debuginator_set_parent(DebuginatorItem* item, DebuginatorItem* parent) {
	if (parent == NULL)
		return;

	DEBUGINATOR_assert(item->parent == NULL || item->parent == parent);
	item->parent = parent;
	if (parent->folder.first_child == NULL) {
		parent->folder.first_child = item;
	}
	else {
		DebuginatorItem* last_sibling = parent->folder.first_child;
		while (last_sibling != NULL)
		{
			if (last_sibling == item) {
				// Item was already in parent
				return;
			}

			if (last_sibling->next_sibling == NULL) {
				// Found the last child, set item as the new last one
				last_sibling->next_sibling = item;
				item->prev_sibling = last_sibling;
				return;
			}

			last_sibling = last_sibling->next_sibling;
		}
	}
}

DebuginatorItem* debuginator_new_folder_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* title, size_t title_length) {
	DEBUGINATOR_assert(debuginator->item_buffer_size < debuginator->item_buffer_capacity);
	DebuginatorItem* folder_item;
	if (debuginator->free_list_size > 0) {
		folder_item = &debuginator->item_buffer[--debuginator->free_list_size];
	}
	else {
		folder_item = &debuginator->item_buffer[debuginator->item_buffer_size++];
	}

	folder_item->is_folder = true;
	debuginator_set_title(folder_item, title, title_length);
	debuginator_set_parent(folder_item, parent);
	return folder_item;
}

DebuginatorItem* debuginator_get_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* path, bool create_if_not_exist) {
	parent = parent == NULL ? debuginator->root : parent;
	const char* temp_path = path;
	while (true) {
		const char* next_slash = strchr(temp_path, '/');
		size_t path_part_length = next_slash ? next_slash - temp_path : strlen(temp_path);

		DebuginatorItem* current_item = NULL;
		DebuginatorItem* parent_child = parent->folder.first_child;
		while (parent_child) {
			const char* item_title = parent_child->title;
			size_t title_length = strlen(item_title); // strlen :(
			if (path_part_length >= DEBUGINATOR_max_title_length 
				&& title_length == DEBUGINATOR_max_title_length - 1 
				&& item_title[DEBUGINATOR_max_title_length - 2] == '.'
				&& item_title[DEBUGINATOR_max_title_length - 3] == '.') {
				path_part_length = title_length = DEBUGINATOR_max_title_length - 3;
			}

			if (title_length == path_part_length && memcmp(parent_child->title, temp_path, title_length * sizeof(char)) == 0) {
				current_item = parent_child;
				break;
			}

			parent_child = parent_child->next_sibling;
		}

		if (current_item == NULL && !create_if_not_exist) {
			return NULL;
		}

		// If current_item is set, it means the item already existed and we're just going to reuse it
		if (next_slash == NULL) {
			// Found the last part of the path
			if (current_item == NULL) {
				current_item = debuginator_get_free_item(debuginator);
				debuginator_set_title(current_item, temp_path, 0);
				debuginator_set_parent(current_item, parent);
			}
			
			return current_item;
		}
		else {
			// Found a folder
			if (current_item == NULL) {
				// Parent item doesn't exist yet
				parent = debuginator_new_folder_item(debuginator, parent, temp_path, next_slash - temp_path);
			}
			else {
				parent = current_item;
			}
			temp_path = next_slash + 1;
		}
	}

	DEBUGINATOR_assert(false);
	return NULL;
}

DebuginatorItem* debuginator_create_array_item(TheDebuginator* debuginator,
	DebuginatorItem* parent, const char* path, const char* description,
	DebuginatorOnItemChangedCallback on_item_changed_callback, void* user_data,
	const char** value_titles, void* values, int num_values, size_t value_size) {

	DebuginatorItem* item = debuginator_get_item(debuginator, parent, path, true);
	item->is_folder = false;
	item->leaf.num_values = num_values;
	item->leaf.values = values;
	item->leaf.array_element_size = value_size;
	item->leaf.value_titles = value_titles;
	item->leaf.on_item_changed_callback = on_item_changed_callback;
	item->user_data = user_data;
	item->leaf.description = description;

	if (item->leaf.hot_index >= num_values) {
		item->leaf.hot_index = num_values - 1;
	}

	//TODO preserve hot item
	return item;
}

void debuginator_set_hot_item(TheDebuginator* debuginator, const char* path) {
	DebuginatorItem* item = debuginator_get_item(debuginator, NULL, path, false);
	if (item == NULL) {
		return;
	}

	debuginator->hot_item = item;
	item->parent->folder.hot_child = item;
}

void debuginator_remove_item(TheDebuginator* debuginator, const char* path) {
	DebuginatorItem* item = debuginator_get_item(debuginator, NULL, path, false);
	if (item == NULL) {
		return;
	}

	if (item->prev_sibling) {
		item->prev_sibling->next_sibling = item->next_sibling;
	}
	if (item->next_sibling) {
		item->next_sibling->prev_sibling = item->prev_sibling;
	}
	
	DebuginatorItem* parent = item->parent;
	if (parent->folder.hot_child == item) {
		if (item->next_sibling != NULL) {
			parent->folder.hot_child = item->next_sibling;
		}
		else if (item->prev_sibling != NULL) {
			parent->folder.hot_child = item->prev_sibling;
		}
		else {
			parent->folder.hot_child = NULL;
		}

		if (parent->folder.first_child == item) {
			parent->folder.first_child = item->next_sibling;
		}
	}

	if (debuginator->hot_item == item) {
		if (parent->folder.hot_child == NULL) {
			debuginator->hot_item = parent;
		}
		else {
			debuginator->hot_item = parent->folder.hot_child;
		}
	}
}

//██╗███╗   ██╗██╗████████╗
//██║████╗  ██║██║╚══██╔══╝
//██║██╔██╗ ██║██║   ██║
//██║██║╚██╗██║██║   ██║
//██║██║ ╚████║██║   ██║
//╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝

void debuginator_get_default_config(TheDebuginatorConfig* config) {
	memset(config, 0, sizeof(*config));

	config->create_default_debuginator_items = true;
	config->open_direction = 1;
	config->focus_height = 0.65f;
	
	// Initialize default themes
	DebuginatorTheme* themes = config->themes;

	// Classic theme
	themes[0].colors[DEBUGINATOR_Background] = debuginator__color(25, 50, 25, 220);
	themes[0].colors[DEBUGINATOR_FolderTitle] = debuginator__color(255, 255, 255, 255);
	themes[0].colors[DEBUGINATOR_ItemTitle] = debuginator__color(120, 120, 0, 250);
	themes[0].colors[DEBUGINATOR_ItemTitleOverridden] = debuginator__color(200, 200, 0, 255);
	themes[0].colors[DEBUGINATOR_ItemTitleHot] = debuginator__color(230, 230, 200, 255);
	themes[0].colors[DEBUGINATOR_ItemTitleActive] = debuginator__color(100, 255, 100, 255);
	themes[0].colors[DEBUGINATOR_ItemDescription] = debuginator__color(150, 150, 150, 255);
	themes[0].colors[DEBUGINATOR_ItemValueDefault] = debuginator__color(50, 150, 50, 200);
	themes[0].colors[DEBUGINATOR_ItemValueOverridden] = debuginator__color(100, 255, 100, 200);
	themes[0].colors[DEBUGINATOR_ItemValueHot] = debuginator__color(100, 255, 100, 200);
	themes[0].colors[DEBUGINATOR_LineHighlight] = debuginator__color(100, 100, 50, 150);
	themes[0].fonts[DEBUGINATOR_ItemDescription].italic = true;

	// Neon theme
	themes[1].colors[DEBUGINATOR_Background] = debuginator__color(15, 15, 30, 220);
	themes[1].colors[DEBUGINATOR_FolderTitle] = debuginator__color(255, 255, 255, 255);
	themes[1].colors[DEBUGINATOR_ItemTitle] = debuginator__color(120, 120, 180, 250);
	themes[1].colors[DEBUGINATOR_ItemTitleOverridden] = debuginator__color(150, 150, 200, 255);
	themes[1].colors[DEBUGINATOR_ItemTitleHot] = debuginator__color(220, 220, 250, 255);
	themes[1].colors[DEBUGINATOR_ItemTitleActive] = debuginator__color(100, 100, 255, 255);
	themes[1].colors[DEBUGINATOR_ItemDescription] = debuginator__color(150, 150, 150, 255);
	themes[1].colors[DEBUGINATOR_ItemValueDefault] = debuginator__color(50, 50, 150, 200);
	themes[1].colors[DEBUGINATOR_ItemValueOverridden] = debuginator__color(100, 100, 255, 200);
	themes[1].colors[DEBUGINATOR_ItemValueHot] = debuginator__color(100, 100, 255, 200);
	themes[1].colors[DEBUGINATOR_LineHighlight] = debuginator__color(70, 70, 130, 150);

	// Black & White theme
	themes[2].colors[DEBUGINATOR_Background] = debuginator__color(25, 25, 25, 220);
	themes[2].colors[DEBUGINATOR_FolderTitle] = debuginator__color(255, 255, 255, 255);
	themes[2].colors[DEBUGINATOR_ItemTitle] = debuginator__color(120, 120, 120, 250);
	themes[2].colors[DEBUGINATOR_ItemTitleOverridden] = debuginator__color(200, 200, 200, 255);
	themes[2].colors[DEBUGINATOR_ItemTitleHot] = debuginator__color(230, 230, 230, 255);
	themes[2].colors[DEBUGINATOR_ItemTitleActive] = debuginator__color(100, 100, 100, 255);
	themes[2].colors[DEBUGINATOR_ItemDescription] = debuginator__color(150, 150, 150, 255);
	themes[2].colors[DEBUGINATOR_ItemValueDefault] = debuginator__color(100, 100, 100, 200);
	themes[2].colors[DEBUGINATOR_ItemValueOverridden] = debuginator__color(200, 200, 200, 200);
	themes[2].colors[DEBUGINATOR_ItemValueHot] = debuginator__color(255, 255, 255, 200);
	themes[2].colors[DEBUGINATOR_LineHighlight] = debuginator__color(100, 100, 100, 150);

	// Beige
	themes[3].colors[DEBUGINATOR_Background] = debuginator__color(255, 240, 220, 220);
	themes[3].colors[DEBUGINATOR_FolderTitle] = debuginator__color(0, 0, 0, 255);
	themes[3].colors[DEBUGINATOR_ItemTitle] = debuginator__color(120, 120, 120, 250);
	themes[3].colors[DEBUGINATOR_ItemTitleOverridden] = debuginator__color(200, 200, 200, 255);
	themes[3].colors[DEBUGINATOR_ItemTitleHot] = debuginator__color(230, 230, 230, 255);
	themes[3].colors[DEBUGINATOR_ItemTitleActive] = debuginator__color(100, 100, 100, 255);
	themes[3].colors[DEBUGINATOR_ItemDescription] = debuginator__color(150, 150, 150, 255);
	themes[3].colors[DEBUGINATOR_ItemValueDefault] = debuginator__color(100, 100, 100, 200);
	themes[3].colors[DEBUGINATOR_ItemValueOverridden] = debuginator__color(200, 200, 200, 200);
	themes[3].colors[DEBUGINATOR_ItemValueHot] = debuginator__color(255, 255, 255, 200);
	themes[3].colors[DEBUGINATOR_LineHighlight] = debuginator__color(100, 100, 100, 150);
}

void debuginator_create(TheDebuginatorConfig* config, TheDebuginator* debuginator) {
	DEBUGINATOR_assert(config->draw_rect != NULL);
	DEBUGINATOR_assert(config->draw_text != NULL);
	//DEBUGINATOR_assert(config->draw_user_data);
	DEBUGINATOR_assert(config->word_wrap != NULL);
	DEBUGINATOR_assert(config->item_buffer != NULL);
	DEBUGINATOR_assert(config->item_buffer_capacity > 0);
	DEBUGINATOR_assert(config->open_direction == -1 || config->open_direction == 1);
	DEBUGINATOR_assert(config->size.x > 0 && config->size.y > 0);

	memset(debuginator, 0, sizeof(*debuginator));

	debuginator->item_buffer_capacity = config->item_buffer_capacity;
	debuginator->item_buffer = config->item_buffer;
	memset(debuginator->item_buffer, 0, sizeof(DebuginatorItem) * debuginator->item_buffer_capacity);
	
	debuginator->draw_rect = config->draw_rect;
	debuginator->draw_text = config->draw_text;
	debuginator->word_wrap = config->word_wrap;
	debuginator->draw_user_data = config->draw_user_data;

	debuginator->size = config->size;
	debuginator->open_direction = config->open_direction;
	debuginator->focus_height = config->focus_height;

	memcpy(debuginator->themes, config->themes, sizeof(debuginator->themes));
	debuginator->theme_index = 0;
	debuginator->theme = debuginator->themes[0];

	// Create root
	DebuginatorItem* item = debuginator_new_folder_item(debuginator, NULL, "Menu Root", 0);
	debuginator->root = item;

	if (config->create_default_debuginator_items) {
		{
			debuginator_create_array_item(debuginator, NULL, "Debuginator/Help",
				"The Debuginator is a debug menu. With a keyboard, you open it with Right Arrow and close it with Left Arrow. You use those keys, plus Up/Down arrows to navigate. Right Arrow is also used to change value on a menu item.", NULL, NULL,
				NULL, NULL, 0, 0);
		}
		{
			static int theme_indices[4] = { 0, 1, 2, 3 };
			static const char* string_titles[4] = { "Classic", "Blue", "Black & White", "Beige" };
			debuginator_create_array_item(debuginator, NULL, "Debuginator/Theme",
				"Change color theme of The Debuginator.", on_change_theme, debuginator,
				string_titles, (void*)theme_indices, 4, sizeof(theme_indices[0]));
		}
	}
}

void debuginator_print(DebuginatorItem* item, int indentation) {
	DEBUGINATOR_debug_print("%*s%s\n", indentation, "", item->title);
	if (item->is_folder) {
		item = item->folder.first_child;
		while (item) {
			debuginator_print(item, indentation + 4);
			item = item->next_sibling;
		}
	}
	else {
		for (size_t i = 0; i < item->leaf.num_values; i++) {
			DEBUGINATOR_debug_print("%*s%s\n", indentation + 4, "", item->leaf.value_titles[i]);
		}
	}
}

DebuginatorItem* debuginator__find_first_leaf(DebuginatorItem* item) {
	if (!item->is_folder) {
		return item;
	}

	if (item->is_folder && item->folder.first_child != NULL) {
		DebuginatorItem* child = item->folder.first_child;
		while (child) {
			if (child->is_folder) {
				DebuginatorItem* leaf_item = debuginator__find_first_leaf(child);
				if (leaf_item != NULL) {
					return leaf_item;
				}

				child = child->next_sibling;
			}
			else {
				return child;
			}
		}
	}

	return NULL;
}

DebuginatorItem* debuginator__find_last_leaf(DebuginatorItem* item) {
	if (!item->is_folder) {
		return item;
	}

	if (item->is_folder && item->folder.first_child != NULL) {
		DebuginatorItem* child = item->folder.first_child;
		while (child->next_sibling) {
			child = child->next_sibling;
		}

		while (child) {
			if (child->is_folder) {
				DebuginatorItem* leaf_item = debuginator__find_last_leaf(child);
				if (leaf_item != NULL) {
					return leaf_item;
				}

				child = child->prev_sibling;
			}
			else {
				return child;
			}
		}
	}

	return NULL;
}


void debuginator_initialize(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator__find_first_leaf(debuginator->root);
	debuginator->hot_item = hot_item;

	debuginator->root->folder.hot_child = hot_item;
	//debuginator_print(debuginator->root, 0);
}

//██╗   ██╗██████╗ ██████╗  █████╗ ████████╗███████╗
//██║   ██║██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝██╔════╝
//██║   ██║██████╔╝██║  ██║███████║   ██║   █████╗
//██║   ██║██╔═══╝ ██║  ██║██╔══██║   ██║   ██╔══╝
//╚██████╔╝██║     ██████╔╝██║  ██║   ██║   ███████╗
//╚═════╝ ╚═╝     ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚══════╝

float debuginator__ease_out(float t, float start_value, float change, float duration) {
	t /= duration;
	return -change * t * (t - 2) + start_value;
}

float debuginator__lerp(float a, float b, float t) {
	return a * (1 - t) + b * t;
}

bool debuginator__distance_to_hot_item(DebuginatorItem* item, DebuginatorItem* hot_item, float* distance) {
	if (item == hot_item) {
		return true;
	}

	*distance += 30;
	if (item->is_folder) {
		DebuginatorItem* child = item->folder.first_child;
		while (child) {
			bool found = debuginator__distance_to_hot_item(child, hot_item, distance);
			if (found) {
				return true;
			}

			child = child->next_sibling;
		}
	}

	return false;
}

void debuginator_update(TheDebuginator* debuginator, float dt) {
	if (debuginator->is_open && debuginator->openness < 1) {
		debuginator->openness_timer += dt;
		if (debuginator->openness_timer > 1) {
			debuginator->openness_timer = 1;
		}

		debuginator->openness = debuginator__ease_out(debuginator->openness_timer, 0, 1, 1);
	}

	else if (!debuginator->is_open && debuginator->openness > 0) {
		debuginator->openness_timer -= dt;
		if (debuginator->openness_timer < 0) {
			debuginator->openness_timer = 0;
		}

		debuginator->openness = debuginator__ease_out(debuginator->openness_timer, 0, 1, 1);
	}


	// Ensure hot item is smoothly placed at a nice position
	float distance_from_root_to_hot_item = 0;
	debuginator__distance_to_hot_item(debuginator->root, debuginator->hot_item, &distance_from_root_to_hot_item);
	float wanted_y = debuginator->size.y * debuginator->focus_height;
	float distance_to_wanted_y = wanted_y - distance_from_root_to_hot_item;
	debuginator->current_height_offset = debuginator__lerp(debuginator->current_height_offset, distance_to_wanted_y, dt);
	if (DEBUGINATOR_fabs(debuginator->current_height_offset - distance_to_wanted_y) < 0.1f) {
		debuginator->current_height_offset = distance_to_wanted_y;
	}
}


//██████╗ ██████╗  █████╗ ██╗    ██╗
//██╔══██╗██╔══██╗██╔══██╗██║    ██║
//██║  ██║██████╔╝███████║██║ █╗ ██║
//██║  ██║██╔══██╗██╔══██║██║███╗██║
//██████╔╝██║  ██║██║  ██║╚███╔███╔╝
//╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝ ╚══╝╚══╝

float debuginator_draw_item(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2 offset, bool hot);

void debuginator_draw(TheDebuginator* debuginator) {
	// TODO return if fully closed

	// update theme opacity
	DebuginatorTheme* source_theme = &debuginator->themes[debuginator->theme_index];
	for (size_t i = 0; i < DEBUGINATOR_NumDrawTypes; i++) {
		debuginator->theme.colors[i].a = (unsigned char)(source_theme->colors[i].a * debuginator->openness);
	}

	// Background
	DebuginatorVector2 offset = debuginator->root_position;
	offset.x += debuginator__lerp(-debuginator->open_direction *debuginator->size.x, 0, debuginator->openness);
	debuginator->draw_rect(offset, debuginator->size, debuginator->theme.colors[DEBUGINATOR_Background], debuginator->draw_user_data);

	offset.y = debuginator->current_height_offset;
	// Draw all items
	debuginator_draw_item(debuginator, debuginator->root, offset, true);
}

float debuginator_draw_item(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2 offset, bool hot) {
	//draw_rect_filled(gui, offset, debuginator__vector2(100, 30), debuginator__color(200, 100, 50, 200));
	/*
	if (!debuginator->hot_item->is_folder && debuginator->hot_item->leaf.is_active) {
	if (debuginator->hot_item == item) {
	for (size_t i = 0; i < item->leaf.num_values; i++) {
	draw_rect_filled(gui, debuginator__vector2(0, offset.y + (i+1) * 30), debuginator__vector2(3, 20), debuginator__color(0, 255, 0, 255));
	}
	}
	}
	else if (debuginator->hot_item->parent == item->parent) {
	draw_rect_filled(gui, debuginator__vector2(0, offset.y), debuginator__vector2(3, 20), debuginator__color(0, 255, 0, 255));
	}*/

	if (item->is_folder) {
		if (debuginator->hot_item == item) {
			debuginator->draw_rect(debuginator__vector2(0, offset.y - 5), debuginator__vector2(500, 30), debuginator->theme.colors[DEBUGINATOR_LineHighlight], debuginator->draw_user_data);
		}

		unsigned color_index = item == debuginator->hot_item ? DEBUGINATOR_ItemTitleActive : (hot ? DEBUGINATOR_ItemTitleHot : DEBUGINATOR_FolderTitle);
		debuginator->draw_text(item->title, &offset, &debuginator->theme.colors[color_index], &debuginator->theme.fonts[DEBUGINATOR_ItemTitle], debuginator->draw_user_data);
		offset.x += 20;
		DebuginatorItem* child = item->folder.first_child;
		while (child) {
			offset.y += 30;
			offset.y = debuginator_draw_item(debuginator, child, offset, debuginator->hot_item == child);
			child = child->next_sibling;
		}
	}
	else {
		if (debuginator->hot_item == item && (!item->leaf.is_active || item->leaf.num_values == 0)) {
			debuginator->draw_rect(debuginator__vector2(debuginator->openness * 500 - 500, offset.y - 5), debuginator__vector2(500, 30), debuginator->theme.colors[DEBUGINATOR_LineHighlight], debuginator->draw_user_data);
		}

		bool is_overriden = item->leaf.active_index != 0;
		unsigned default_color_index = is_overriden ? DEBUGINATOR_ItemTitleOverridden : DEBUGINATOR_ItemTitle;
		unsigned color_index = item == debuginator->hot_item && !item->leaf.is_active ? DEBUGINATOR_ItemTitleActive : (hot ? DEBUGINATOR_ItemTitleHot : default_color_index);
		DebuginatorFont lolfont = debuginator->theme.fonts[DEBUGINATOR_ItemTitle];
		debuginator->draw_text(item->title, &offset, &debuginator->theme.colors[color_index], &lolfont, debuginator->draw_user_data);

		// Draw quick representation of value
		if (item->leaf.quick_draw_callback) {
			DebuginatorVector2 value_offset = offset;
			value_offset.x = 300 + debuginator->openness * 500 - 500;
			//item->leaf.quick_draw_callback(item, &value_offset.x, default_color_index);
		}
		else if (item->leaf.num_values > 0) {
			DebuginatorVector2 value_offset = offset;
			value_offset.x = 300 + debuginator->openness * 500 - 500;
			debuginator->draw_text(item->leaf.value_titles[item->leaf.active_index], &value_offset, &debuginator->theme.colors[default_color_index], &debuginator->theme.fonts[DEBUGINATOR_ItemTitle], debuginator->draw_user_data);
		}

		if (item->leaf.is_active) {
			offset.x += 20;

			char description_line[256];
			const char* description = item->leaf.description;
			while (description) {
				description = debuginator->word_wrap(description, debuginator->theme.fonts[DEBUGINATOR_ItemDescription], 350 - offset.x, description_line, sizeof(description_line), debuginator->draw_user_data);

				offset.y += 30;
				debuginator->draw_text(description_line, &offset, &debuginator->theme.colors[DEBUGINATOR_ItemDescription], &debuginator->theme.fonts[DEBUGINATOR_ItemDescription], debuginator->draw_user_data);
			}

			for (size_t i = 0; i < item->leaf.num_values; i++) {
				offset.y += 30;

				if (debuginator->hot_item == item && item->leaf.hot_index == i) {
					DebuginatorVector2 pos = debuginator__vector2(0, offset.y - 5);
					DebuginatorVector2 size = debuginator__vector2(500, 30);
					debuginator->draw_rect(pos, size, debuginator->theme.colors[DEBUGINATOR_LineHighlight], debuginator->draw_user_data);
				}

				const char* value_title = item->leaf.value_titles[i];
				bool value_hot = i == item->leaf.hot_index;
				bool value_overridden = i == item->leaf.active_index;
				unsigned value_color_index = value_hot ? DEBUGINATOR_ItemValueHot : (value_overridden ? DEBUGINATOR_ItemTitleOverridden : DEBUGINATOR_ItemValueDefault);
				debuginator->draw_text(value_title, &offset, &debuginator->theme.colors[value_color_index], &debuginator->theme.fonts[value_hot ? DEBUGINATOR_ItemTitle : DEBUGINATOR_ItemTitle], debuginator->draw_user_data);
			}
		}
	}

	return offset.y;
}




//██╗███╗   ██╗██████╗ ██╗   ██╗████████╗
//██║████╗  ██║██╔══██╗██║   ██║╚══██╔══╝
//██║██╔██╗ ██║██████╔╝██║   ██║   ██║
//██║██║╚██╗██║██╔═══╝ ██║   ██║   ██║
//██║██║ ╚████║██║     ╚██████╔╝   ██║
//╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝    ╚═╝

void debuginator_set_open(TheDebuginator* debuginator, bool is_open) {
	debuginator->is_open = is_open;
}

static void debuginator_activate(DebuginatorItem* item) {
	if (item->leaf.num_values == 0) {
		return;
	}

	item->leaf.active_index = item->leaf.hot_index;
	
	if (item->leaf.on_item_changed_callback == NULL) {
		return;
	}

	void* value = ((char*)item->leaf.values) + item->leaf.hot_index * item->leaf.array_element_size;
	item->leaf.on_item_changed_callback(item, value, item->leaf.value_titles[item->leaf.hot_index]);
}

void debuginator_move_sibling_previous(TheDebuginator* debuginator) {
	// This is a bit stupid, consider changing to doubly linked list
	DebuginatorItem* hot_item = debuginator->hot_item;

	if (!hot_item->is_folder && hot_item->leaf.is_active) {
		if (--hot_item->leaf.hot_index < 0) {
			hot_item->leaf.hot_index = hot_item->leaf.num_values - 1;
		}
	}
	else {
		DebuginatorItem* hot_item_new = debuginator->hot_item;
		DebuginatorItem* parent_child = hot_item_new->parent->folder.first_child;
		if (parent_child == hot_item_new) {
			while (parent_child) {
				hot_item_new = parent_child;
				parent_child = parent_child->next_sibling;
			}
		}
		else {
			while (parent_child) {
				if (parent_child->next_sibling == hot_item_new) {
					hot_item_new = parent_child;
					break;
				}

				parent_child = parent_child->next_sibling;
			}
		}

		if (hot_item != hot_item_new) {
			hot_item_new->parent->folder.hot_child = hot_item_new;
			debuginator->hot_item = hot_item_new;
		}
	}
}

void debuginator_move_sibling_next(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator->hot_item;

	if (!hot_item->is_folder && hot_item->leaf.is_active) {
		if (++hot_item->leaf.hot_index == hot_item->leaf.num_values) {
			hot_item->leaf.hot_index = 0;
		}
	}
	else {
		DebuginatorItem* hot_item_new = debuginator->hot_item;
		if (hot_item->next_sibling != NULL) {
			hot_item_new = hot_item->next_sibling;
		}
		else {
			hot_item_new = hot_item->parent->folder.first_child;
		}

		if (hot_item != hot_item_new) {
			hot_item_new->parent->folder.hot_child = hot_item_new;
			debuginator->hot_item = hot_item_new;
		}
	}
}

void debuginator_move_to_next(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator->hot_item;
	DebuginatorItem* hot_item_new = debuginator->hot_item;
	if (!hot_item->is_folder && hot_item->leaf.is_active) {
		if (++hot_item->leaf.hot_index == hot_item->leaf.num_values) {
			hot_item->leaf.hot_index = 0;
		}
	}
	else if (hot_item->next_sibling) {
		hot_item_new = hot_item->next_sibling;
	}
	else {
		DebuginatorItem* parent = hot_item->parent;
		while (parent != NULL) {
			if (parent->next_sibling != NULL) {
				hot_item_new = parent->next_sibling;
				break;
			}

			parent = parent->next_sibling;
		}

		if (parent == NULL) {
			hot_item_new = debuginator->root->folder.first_child;
		}
	}

	if (hot_item != hot_item_new) {
		hot_item_new->parent->folder.hot_child = hot_item_new;
		debuginator->hot_item = hot_item_new;
	}
}

void debuginator_move_to_next_leaf(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator->hot_item;
	DebuginatorItem* hot_item_new = debuginator->hot_item;
	if (!hot_item->is_folder && hot_item->leaf.is_active) {
		if (++hot_item->leaf.hot_index == hot_item->leaf.num_values) {
			hot_item->leaf.hot_index = 0;
		}
	}
	else {
		/*
		MR
		  A
			A1
			A2
			  A3
			  A4
		  B
			B1
			B2
		*/

		DebuginatorItem* sibling = hot_item->next_sibling;
		DebuginatorItem* parent = hot_item->parent;
		hot_item_new = NULL;
		while (hot_item_new == NULL) {
			while (sibling != NULL) {
				DebuginatorItem* leaf_item = debuginator__find_first_leaf(sibling);
				if (leaf_item != NULL) {
					hot_item_new = leaf_item;
					break;
				}

				sibling = sibling->next_sibling;
			}

			if (hot_item_new != NULL) {
				break;
			}

			while (parent->next_sibling == NULL) {
				if (parent == debuginator->root) {
					DebuginatorItem* leaf_item = debuginator__find_first_leaf(parent);
					hot_item_new = leaf_item;
					break;
				}

				parent = parent->parent;
			}

			sibling = parent->next_sibling;
		}

		hot_item_new->parent->folder.hot_child = hot_item_new;
		debuginator->hot_item = hot_item_new;
	}
}


void debuginator_move_to_prev_leaf(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator->hot_item;
	DebuginatorItem* hot_item_new = debuginator->hot_item;
	if (!hot_item->is_folder && hot_item->leaf.is_active) {
		if (--hot_item->leaf.hot_index < 0) {
			hot_item->leaf.hot_index = hot_item->leaf.num_values - 1;
		}
	}
	else {
		/*
		MR
		A
		A1
		A2
		A3
		A4
		B
		B1
		B2
		*/


		DebuginatorItem* sibling = hot_item->prev_sibling;
		DebuginatorItem* parent = hot_item->parent;
		hot_item_new = NULL;
		while (hot_item_new == NULL) {
			while (sibling != NULL) {
				DebuginatorItem* leaf_item = debuginator__find_last_leaf(sibling);
				if (leaf_item != NULL) {
					hot_item_new = leaf_item;
					break;
				}

				sibling = sibling->prev_sibling;
			}

			if (hot_item_new != NULL) {
				break;
			}

			while (parent->prev_sibling == NULL) {
				if (parent == debuginator->root) {
					DebuginatorItem* leaf_item = debuginator__find_last_leaf(parent);
					hot_item_new = leaf_item;
					break;
				}

				parent = parent->parent;
			}

			sibling = parent->prev_sibling;
		}

		hot_item_new->parent->folder.hot_child = hot_item_new;
		debuginator->hot_item = hot_item_new;
	}
}

void debuginator_move_to_child(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator->hot_item;
	DebuginatorItem* hot_item_new = debuginator->hot_item;

	if (!hot_item->is_folder) {
		if (hot_item->leaf.is_active) {
			debuginator_activate(debuginator->hot_item);
		}
		else {
			hot_item->leaf.is_active = true;
		}
	}
	else {
		if (hot_item->folder.hot_child != NULL) {
			hot_item_new = hot_item->folder.hot_child;
		}
		else if (hot_item->folder.first_child != NULL) {
			hot_item_new = hot_item->folder.first_child;
			hot_item_new->parent->folder.hot_child = hot_item_new;
		}

		if (hot_item != hot_item_new) {
			hot_item_new->parent->folder.hot_child = hot_item_new;
			debuginator->hot_item = hot_item_new;
		}
	}
}

void debuginator_move_to_parent(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator->hot_item;
	DebuginatorItem* hot_item_new = debuginator->hot_item;
	if (!hot_item->is_folder && hot_item->leaf.is_active) {
		hot_item->leaf.is_active = false;
	}
	else if (hot_item->parent != debuginator->root) {
		hot_item_new = debuginator->hot_item->parent;
	}

	if (hot_item != hot_item_new) {
		hot_item_new->parent->folder.hot_child = hot_item_new;
		debuginator->hot_item = hot_item_new;
	}
}

typedef struct DebuginatorInput {
	//bool activate;
	bool move_sibling_previous;
	bool move_sibling_next;
	bool move_to_parent;
	bool move_to_child;
} DebuginatorInput;

void debug_menu_handle_input(TheDebuginator* debuginator, DebuginatorInput* input) {
	if (input->move_sibling_next) {
		debuginator_move_sibling_next(debuginator);
	}

	if (input->move_to_child) {
		debuginator_move_to_child(debuginator);
	}

	if (input->move_to_parent) {
		debuginator_move_to_parent(debuginator);		
	}

	// HACK
	//if (hot_item != hot_item_new) {
		//debuginator->hot_item = hot_item_new;
	//}

	//if (input->activate && hot_item->!is_folder) {
	//	debuginator_activate(debuginator->hot_item);		
	//}
}

// ██╗   ██╗████████╗██╗██╗     ██╗████████╗██╗   ██╗
// ██║   ██║╚══██╔══╝██║██║     ██║╚══██╔══╝╚██╗ ██╔╝
// ██║   ██║   ██║   ██║██║     ██║   ██║    ╚████╔╝
// ██║   ██║   ██║   ██║██║     ██║   ██║     ╚██╔╝
// ╚██████╔╝   ██║   ██║███████╗██║   ██║      ██║
//  ╚═════╝    ╚═╝   ╚═╝╚══════╝╚═╝   ╚═╝      ╚═╝

void debuginator_copy_1byte(DebuginatorItem* item, void* value, const char* value_title) {
	(void)value_title;
	memcpy(item->user_data, value, 1);
}

void debuginator_create_bool_item(TheDebuginator* debuginator, const char* path, const char* description, void* user_data) {
	static bool bool_values[2] = { false, true }; 
	static const char* bool_titles[2] = { "False", "True" };
	DEBUGINATOR_assert(sizeof(bool_values[0]) == 1);
	debuginator_create_array_item(debuginator, NULL, path,
		description, debuginator_copy_1byte, user_data,
		bool_titles, bool_values, 2, sizeof(bool_values[0]));
}

#endif // INCLUDE_THE_DEBUGINATOR_H
