
/*
the_debuginator.h - v0.01 - public domain - Anders Elfgren @srekel, 2017

# THE DEBUGINATOR

A super sweet hierarchical scrollable accordion debug menu intended for games.

See github for latest version: https://github.com/Srekel/the-debuginator

## Usage

In *ONE* source file, put:

```C
#define DEBUGINATOR_IMPLEMENTATION

// Define any of these if you wish to override them.
#define DEBUGINATOR_assert
#define DEBUGINATOR_memcpy
#define DEBUGINATOR_fabs
#define DEBUGINATOR_own_value_titles

#include "the_debuginator.h"
```

Other source files should just include the_debuginator.h

## Notes

See the accompanying SDL demo and unit test projects for references on how to use it.

ASCII banners generated using
http://patorjk.com/software/taag/#p=display&f=ANSI%20Shadow&t=update

## License

See end of file for license information.

*/

#ifndef INCLUDE_THE_DEBUGINATOR_H
#define INCLUDE_THE_DEBUGINATOR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DebuginatorItem DebuginatorItem;

#ifndef DEBUGINATOR_max_title_length
#define DEBUGINATOR_max_title_length 20
#endif

#ifndef DEBUGINATOR_max_themes
#define DEBUGINATOR_max_themes 16
#endif

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

typedef enum DebuginatorDrawTypes {
	DEBUGINATOR_Background,
	DEBUGINATOR_FolderTitle,
	DEBUGINATOR_ItemTitle,
	DEBUGINATOR_ItemTitleOverridden,
	DEBUGINATOR_ItemTitleHot,
	DEBUGINATOR_ItemTitleActive,
	DEBUGINATOR_ItemTitleActive1,
	DEBUGINATOR_ItemTitleActive2,
	DEBUGINATOR_ItemDescription,
	DEBUGINATOR_LineHighlight,
	DEBUGINATOR_ItemValueDefault,
	DEBUGINATOR_ItemValueOverridden,
	DEBUGINATOR_ItemValueHot,
	DEBUGINATOR_ItemEditorOff,
	DEBUGINATOR_ItemEditorOn,
	DEBUGINATOR_ItemEditorBackground,
	DEBUGINATOR_ItemEditorForeground,
	DEBUGINATOR_NumDrawTypes
} DebuginatorDrawTypes;

typedef struct DebuginatorTheme {
	DebuginatorColor colors[DEBUGINATOR_NumDrawTypes];
	DebuginatorFont fonts[DEBUGINATOR_NumDrawTypes];
} DebuginatorTheme;

typedef struct DebuginatorItem DebuginatorItem;
typedef struct TheDebuginator TheDebuginator;

typedef void (*DebuginatorDrawTextCallback)
	(const char* text, DebuginatorVector2* position, DebuginatorColor* color, DebuginatorFont* font, void* userdata);
typedef void (*DebuginatorDrawRectCallback)
	(DebuginatorVector2 position, DebuginatorVector2 size, DebuginatorColor color, void* userdata);
typedef void(*DebuginatorWordWrapCallback)
	(const char* text, DebuginatorFont font, float max_width, unsigned* row_count, unsigned* row_lengths, int row_lengths_buffer_size, void* app_userdata);
typedef DebuginatorVector2 (*DebuginatorTextSizeCallback)
	(const char* text, DebuginatorFont* font, void* userdata);

typedef void(*DebuginatorOnItemChangedCallback)(DebuginatorItem* item, void* value, const char* value_title, void* app_userdata);
typedef int(*DebuginatorSaveItemCallback)(const char* path, const char* value, char* save_buffer, int save_buffer_size);

typedef enum DebuginatorItemEditorDataType {
	DEBUGINATOR_EditTypeArray, // The default
	DEBUGINATOR_EditTypeActionArray, // For items with direct actions and no state
	DEBUGINATOR_EditTypeBoolean,
	DEBUGINATOR_EditTypePreset,
	/*DEBUGINATOR_EditTypeUserType1,
	...
	DEBUGINATOR_EditTypeUserTypeN,*/
	DEBUGINATOR_EditTypeCount = 16,
} DebuginatorItemEditorDataType;

bool debuginator_is_open(TheDebuginator* debuginator);
void debuginator_set_open(TheDebuginator* debuginator, bool open);

DebuginatorItem* debuginator_create_array_item(TheDebuginator* debuginator,
	DebuginatorItem* parent, const char* path, const char* description,
	DebuginatorOnItemChangedCallback on_item_changed_callback, void* user_data,
	const char** value_titles, void* values, int num_values, size_t value_size);

DebuginatorItem* debuginator_create_bool_item(TheDebuginator* debuginator, const char* path, const char* description, void* user_data);
DebuginatorItem* debuginator_create_preset_item(TheDebuginator* debuginator, const char* path, const char** paths, const char** value_titles, int** value_indices, int num_paths);

DebuginatorItem* debuginator_new_folder_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* title, int title_length);
DebuginatorItem* debuginator_get_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* path, bool create_if_not_exist);
void debuginator_set_hot_item(TheDebuginator* debuginator, const char* path);
DebuginatorItem* debuginator_get_hot_item(TheDebuginator* debuginator);
void debuginator_remove_item(TheDebuginator* debuginator, const char* path);

int debuginator_save(TheDebuginator* debuginator, DebuginatorSaveItemCallback callback, char* save_buffer, int save_buffer_size);
void debuginator_load_item(TheDebuginator* debuginator, const char* path, const char* value_title);
void debuginator_set_default_value(TheDebuginator* debuginator, const char* path, const char* value_title, int value_index); // value index is used if value_title == NULL
void debuginator_set_edit_type(TheDebuginator* debuginator, const char* path, DebuginatorItemEditorDataType edit_type);

void debuginator_activate(TheDebuginator* debuginator, DebuginatorItem* item);

void debuginator_move_to_next_leaf(TheDebuginator* debuginator, bool long_move);
void debuginator_move_to_prev_leaf(TheDebuginator* debuginator, bool long_move);
void debuginator_move_to_child(TheDebuginator* debuginator, bool toggle_and_activate);
void debuginator_move_to_parent(TheDebuginator* debuginator);

bool debuginator_is_filtering_enabled(TheDebuginator* debuginator);
void debuginator_set_filtering_enabled(TheDebuginator* debuginator, bool enabled);
char* debuginator_get_filter(TheDebuginator* debuginator);
void debuginator_update_filter(TheDebuginator* debuginator, const char* wanted_filter);

typedef struct DebuginatorFolderData {
	DebuginatorItem* first_child;
	DebuginatorItem* hot_child;
	int num_visible_children;
} DebuginatorFolderData;

typedef struct DebuginatorLeafData {
	char* description;

	bool is_active;
	int hot_index;
	int active_index;
	int default_index;

	const char** value_titles;
	const char** value_descriptions;
	void* values;

	int num_values;
	size_t array_element_size;

	DebuginatorOnItemChangedCallback on_item_changed_callback;
	DebuginatorItemEditorDataType edit_type;
	float draw_t;
} DebuginatorLeafData;

typedef enum DebuginatorAnimationType {
	DEBUGINATOR_ItemActivate
} DebuginatorAnimationType;

typedef struct DebuginatorItemEditorData {
	void(*quick_draw)(TheDebuginator* debuginator, DebuginatorItem* item_data, DebuginatorVector2* position);
	float(*expanded_height)(DebuginatorItem* item, void* userdata);
	void(*expanded_draw)(TheDebuginator* debuginator, DebuginatorItem* item_data, DebuginatorVector2* position);
	bool forget_state;
} DebuginatorItemEditorData;

typedef struct DebuginatorItem {
	char* title;
	bool is_folder;
	bool is_filtered;
	void* user_data;
	float total_height; // Including self and children


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

	char* memory_arena;
	int memory_arena_capacity;

	DebuginatorTheme themes[16];

	DebuginatorItemEditorData edit_types[DEBUGINATOR_EditTypeCount];

	void* app_user_data;
	DebuginatorDrawTextCallback draw_text;
	DebuginatorDrawRectCallback draw_rect;
	DebuginatorWordWrapCallback word_wrap;
	DebuginatorTextSizeCallback text_size;

	DebuginatorVector2 size; // Might not be needed in the future
	DebuginatorVector2 screen_resolution;
	bool left_aligned;
	int open_direction;
	float focus_height;
} TheDebuginatorConfig;


#ifdef __cplusplus
}
#endif

#ifdef DEBUGINATOR_IMPLEMENTATION


#ifndef DEBUGINATOR_assert
#include <assert.h>
#define DEBUGINATOR_assert assert;
#endif

#ifndef DEBUGINATOR_memcpy
#include <string.h>
#define DEBUGINATOR_memcpy memcpy
#endif

#ifndef DEBUGINATOR_memset
#include <string.h>
#define DEBUGINATOR_memset memset
#endif

#ifndef DEBUGINATOR_strchr
#include <string.h>
#define DEBUGINATOR_strchr strchr
#endif

#ifndef DEBUGINATOR_strlen
#include <string.h>
#define DEBUGINATOR_strlen strlen
#endif

#ifndef DEBUGINATOR_strstr
#include <string.h>
#define DEBUGINATOR_strstr strstr
#endif

#ifndef DEBUGINATOR_strcpy_s
#include <string.h>
#define DEBUGINATOR_strcpy_s strcpy_s
#endif

#ifndef DEBUGINATOR_strncpy_s
#include <string.h>
#define DEBUGINATOR_strncpy_s strncpy_s
#endif

#ifndef DEBUGINATOR_tolower
#include <ctype.h>
#define DEBUGINATOR_tolower tolower
#endif


#ifndef DEBUGINATOR_isalpha
#include <ctype.h>
#define DEBUGINATOR_isalpha isalpha
#define DEBUGINATOR_isdigit isdigit // must override both
#endif

#ifndef DEBUGINATOR_fabs
#include <math.h>
#define DEBUGINATOR_fabs fabs
#endif

#ifndef DEBUGINATOR_min
#define DEBUGINATOR_min(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef DEBUGINATOR_sin
#include <math.h>
#define DEBUGINATOR_sin sin
#endif

#ifndef __cplusplus
#include <stdbool.h>
#endif
//
//#ifndef DEBUGINATOR_FREE_LIST_CAPACITY
//#define DEBUGINATOR_FREE_LIST_CAPACITY 256
//#endif
//
//#ifndef DEBUGINATOR_max_title_length
//#define DEBUGINATOR_max_title_length 20
//#endif
//
//#ifndef DEBUGINATOR_max_themes
//#define DEBUGINATOR_max_themes 16
//#endif

#ifndef DEBUGINATOR_debug_print
#define DEBUGINATOR_debug_print
#endif


typedef struct DebuginatorSlotAllocator DebuginatorSlotAllocator;
typedef struct DebuginatorBlockAllocator DebuginatorBlockAllocator;
typedef struct DebuginatorSlotAllocator {
	char* arena;
	int size;
	int capacity;
	int element_size;
	void* next_free_slot;
	void*(*allocate)(DebuginatorSlotAllocator* allocator, int num_bytes);
	void(*deallocate)(DebuginatorSlotAllocator* allocator, void* ptr);
} DebuginatorSlotAllocator;

typedef struct DebuginatorBlockAllocatorStaticData {
	char* arena_end;
	int arena_capacity;
	int block_capacity;
	char* next_free_block;
} DebuginatorBlockAllocatorStaticData;

typedef struct DebuginatorBlockAllocator {
	// void*(*allocate)(DebuginatorBlockAllocator* allocator, int num_bytes);
	// void(*deallocate)(DebuginatorBlockAllocator* allocator, void* ptr);
	DebuginatorBlockAllocatorStaticData* data;
	int element_size;
	char* current_block;
	int current_block_size;
	char* next_free_slot;
} DebuginatorBlockAllocator;

void debuginator__block_allocator_init(DebuginatorBlockAllocator* allocator, int element_size, DebuginatorBlockAllocatorStaticData* data) {
	DEBUGINATOR_memset(allocator, 0, sizeof(*allocator));
	allocator->data = data;
	allocator->element_size = element_size;
	allocator->current_block = data->next_free_block;
	allocator->current_block_size = allocator->element_size; // Make room for allocator ptr at start of block
	data->next_free_block += data->block_capacity;
	DEBUGINATOR_assert(allocator->data->arena_end > allocator->data->next_free_block);
	*((DebuginatorBlockAllocator**)allocator->current_block) = allocator;
}

void* debuginator__block_allocate(DebuginatorBlockAllocator* allocator, int num_bytes) {
	(void)num_bytes;
	if (allocator->data->block_capacity - allocator->current_block_size < num_bytes) {
		if (allocator->data->arena_end < allocator->data->next_free_block) {
			return NULL;
		}

		allocator->current_block_size = allocator->element_size; // Make room for allocator ptr at start of block
		allocator->current_block = allocator->data->next_free_block;
		allocator->data->next_free_block += allocator->data->block_capacity;
		DEBUGINATOR_assert(allocator->data->arena_end > allocator->data->next_free_block);
		*((DebuginatorBlockAllocator**)allocator->current_block) = allocator;
	}

	// TODO: Use the freed slots.

	void* result = allocator->current_block + allocator->current_block_size;
	allocator->current_block_size += allocator->element_size;
	return result;
}

void debuginator__block_deallocate(DebuginatorBlockAllocator* allocator, void* ptr) {
	uintptr_t* next_ptr = (uintptr_t*)ptr;
	if (allocator->next_free_slot == NULL) {
		*next_ptr = 0;
	}
	else {
		*next_ptr = (uintptr_t)allocator->next_free_slot;
	}
	allocator->next_free_slot = (char*)ptr;
}

//
//void* debuginator__slot_allocate(DebuginatorSlotAllocator* allocator, int num_bytes) {
//	(void)num_bytes;
//	if (allocator->next_free_slot == NULL) {
//		if (allocator->size == allocator->capacity) {
//			return NULL;
//		}
//
//		void* result = allocator->arena + allocator->size;
//		allocator->size += allocator->element_size;
//		return result;
//	}
//
//	void* result = allocator->next_free_slot;
//	allocator->next_free_slot = result;
//	return result;
//}
//
//void debuginator__slot_deallocate(DebuginatorSlotAllocator* allocator, void* ptr) {
//	if (allocator->next_free_slot == NULL) {
//		int* int_ptr = (int*)ptr;
//		*int_ptr = 0;
//		allocator->next_free_slot = int_ptr;
//	}
//}

typedef struct DebuginatorAnimation {
	DebuginatorAnimationType type;
	union {
		struct {
			DebuginatorItem* item;
			int value_index;
			DebuginatorVector2 start_pos;
		} item_activate;
	} data;

	float duration;
	float time;
} DebuginatorAnimation;

typedef struct TheDebuginator {
	DebuginatorItem* root;
	DebuginatorItem* hot_item;

	bool is_open;
	float openness_timer; // range [0,1]
	float openness; // range [0,1]

	DebuginatorTheme themes[16];
	DebuginatorTheme theme; // current theme
	int theme_index;

	DebuginatorItemEditorData edit_types[DEBUGINATOR_EditTypeCount];

	float dt;
	float draw_timer;
	float filter_timer;
	void* app_user_data;
	DebuginatorDrawTextCallback draw_text;
	DebuginatorDrawRectCallback draw_rect;
	DebuginatorWordWrapCallback word_wrap;
	DebuginatorTextSizeCallback text_size;

	DebuginatorVector2 size;
	DebuginatorVector2 root_position;
	DebuginatorVector2 screen_resolution;
	bool left_aligned;
	int open_direction;
	float focus_height;
	float current_height_offset;

	DebuginatorAnimation animations[8];
	int animation_count;

	bool filter_enabled;
	char filter[32];
	int filter_length;

	char* memory_arena; // char* for pointer arithmetic
	int memory_arena_capacity;
	DebuginatorBlockAllocatorStaticData allocator_data;
	//int allocator_by_allocation[4];
	DebuginatorBlockAllocator allocators[4];
	/*
	DebuginatorItem* memory_item_buffer;
	int memory_item_buffer_size;
	int memory_item_buffer_capacity;
	int memory_item_buffer_free_index;
	char* memory_fixed_string_buffer;
	int memory_fixed_string_buffer_size;
	int memory_fixed_string_buffer_capacity;
	int memory_fixed_string_buffer_index;
	char* memory_string_buffer;
	int memory_string_buffer_size;
	int memory_string_buffer_capacity;
	char* memory_string_buffer_free_index;
	size_t item_buffer_capacity;
	size_t item_buffer_size;
	DebuginatorItem* item_buffer;

	size_t free_list_size;
	size_t free_list[256];*/

} TheDebuginator;

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

float debuginator__ease_out(float t, float start_value, float change, float duration) {
	t /= duration;
	return -change * t * (t - 2) + start_value;
}

float debuginator__lerp(float a, float b, float t) {
	return a * (1 - t) + b * t;
}

void debuginator__quick_draw_default(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	if (item->leaf.num_values > 0) {
		DebuginatorVector2 value_offset = *position;
		if (debuginator->left_aligned) {
			value_offset.x = debuginator->openness * debuginator->size.x - 200;
		}
		else {
			value_offset.x = debuginator->screen_resolution.x + debuginator->size.x * (1 - debuginator->openness) - 200;
		}

		bool is_overriden = item->leaf.active_index != item->leaf.default_index;
		unsigned default_color_index = is_overriden ? DEBUGINATOR_ItemTitleOverridden : DEBUGINATOR_ItemTitle;
		debuginator->draw_text(item->leaf.value_titles[item->leaf.active_index], &value_offset, &debuginator->theme.colors[default_color_index], &debuginator->theme.fonts[DEBUGINATOR_ItemTitle], debuginator->app_user_data);
	}
}

void debuginator__expanded_draw_default(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	for (size_t i = 0; i < item->leaf.num_values; i++) {
		position->y += 30;

		if (debuginator->hot_item == item && item->leaf.hot_index == i) {
			DebuginatorVector2 pos = debuginator__vector2(0, position->y - 5);
			DebuginatorVector2 size = debuginator__vector2(500, 30);
			if (!debuginator->left_aligned) {
				pos.x = debuginator->screen_resolution.x - debuginator->openness * debuginator->size.x;
			}
			debuginator->draw_rect(pos, size, debuginator->theme.colors[DEBUGINATOR_LineHighlight], debuginator->app_user_data);
		}

		const char* value_title = item->leaf.value_titles[i];
		bool value_hot = i == item->leaf.hot_index;
		bool value_overridden = i == item->leaf.active_index;
		unsigned value_color_index = value_hot ? DEBUGINATOR_ItemValueHot : (value_overridden ? DEBUGINATOR_ItemTitleOverridden : DEBUGINATOR_ItemValueDefault);
		debuginator->draw_text(value_title, position, &debuginator->theme.colors[value_color_index], &debuginator->theme.fonts[value_hot ? DEBUGINATOR_ItemTitleHot : DEBUGINATOR_ItemTitle], debuginator->app_user_data);
	}
}

void debuginator__quick_draw_boolean(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	DebuginatorVector2 pos = debuginator__vector2(debuginator->size.x * debuginator->openness - 200, position->y);
	DebuginatorVector2 size = debuginator__vector2(50, 20);
	DebuginatorColor background = debuginator__color(0, 0, 0, 100);
	debuginator->draw_rect(pos, size, background, debuginator->app_user_data);

	item->leaf.draw_t += debuginator->dt * 5;
	if (item->leaf.draw_t > 1) {
		item->leaf.draw_t = 1;
	}

	DebuginatorVector2 slider_pos = pos;
	if (item->leaf.active_index == 0) {
		//slider_pos.x = debuginator__lerp(pos.x + 27, pos.x + 2, item->leaf.draw_t);
		slider_pos.x = debuginator__ease_out(item->leaf.draw_t, pos.x + 27, -25, 1);
	}
	else {
		//slider_pos.x = debuginator__lerp(pos.x + 2, pos.x + 27, item->leaf.draw_t);
		slider_pos.x = debuginator__ease_out(item->leaf.draw_t, pos.x + 2, 25, 1);
	}
	//slider_pos.x += item->leaf.active_index == 0 ? 2 : 27;
	slider_pos.y += 2;
	size.x = 21;
	size.y = 16;
	unsigned char alpha = item->leaf.active_index == item->leaf.default_index ? 100 : 255;
	DebuginatorColor slider = item->leaf.active_index == 0 ? debuginator->theme.colors[DEBUGINATOR_ItemEditorOff] : debuginator->theme.colors[DEBUGINATOR_ItemEditorOn];
	slider.a = alpha;

	debuginator->draw_rect(slider_pos, size, slider, debuginator->app_user_data);
}

void debuginator__expanded_draw_boolean(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	for (size_t i = 0; i < item->leaf.num_values; i++) {
		position->y += 30;

		if (debuginator->hot_item == item && item->leaf.hot_index == i) {
			DebuginatorVector2 pos = debuginator__vector2(0, position->y - 5);
			DebuginatorVector2 size = debuginator__vector2(500, 30);
			if (!debuginator->left_aligned) {
				pos.x = debuginator->screen_resolution.x - debuginator->openness * debuginator->size.x;
			}
			debuginator->draw_rect(pos, size, debuginator->theme.colors[DEBUGINATOR_LineHighlight], debuginator->app_user_data);
		}

		const char* value_title = item->leaf.value_titles[i];
		bool value_hot = i == item->leaf.hot_index;
		bool value_overridden = i == item->leaf.active_index;
		unsigned value_color_index = value_hot ? DEBUGINATOR_ItemValueHot : (value_overridden ? DEBUGINATOR_ItemTitleOverridden : DEBUGINATOR_ItemValueDefault);
		debuginator->draw_text(value_title, position, &debuginator->theme.colors[value_color_index], &debuginator->theme.fonts[value_hot ? DEBUGINATOR_ItemTitleHot : DEBUGINATOR_ItemTitle], debuginator->app_user_data);
	}
}

void debuginator__quick_draw_preset(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	(void)debuginator, item, position;
}

void debuginator__expanded_draw_preset(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	for (size_t i = 0; i < item->leaf.num_values; i++) {
		position->y += 30;

		//if (debuginator->hot_item == item && item->leaf.hot_index == i) {
		//	DebuginatorVector2 pos = debuginator__vector2(0, position->y - 5);
		//	DebuginatorVector2 size = debuginator__vector2(500, 30);
		//	if (!debuginator->left_aligned) {
		//		pos.x = debuginator->screen_resolution.x - debuginator->openness * debuginator->size.x;
		//	}
		//	debuginator->draw_rect(pos, size, debuginator->theme.colors[DEBUGINATOR_LineHighlight], debuginator->app_user_data);
		//}

		const char* value_title = item->leaf.value_titles[i];
		bool value_hot = i == item->leaf.hot_index;
		bool value_overridden = i == item->leaf.active_index;
		unsigned value_color_index = value_hot ? DEBUGINATOR_ItemValueHot : (value_overridden ? DEBUGINATOR_ItemTitleOverridden : DEBUGINATOR_ItemValueDefault);
		debuginator->draw_text(value_title, position, &debuginator->theme.colors[value_color_index], &debuginator->theme.fonts[value_hot ? DEBUGINATOR_ItemTitleHot : DEBUGINATOR_ItemTitle], debuginator->app_user_data);
	}
}

void* debuginator__allocate(TheDebuginator* debuginator, int bytes/*, const void* origin*/) {
	for (int i = 0; i < 4; i++) {
		if (bytes <= debuginator->allocators[i].element_size) {
			void* result = debuginator__block_allocate(&debuginator->allocators[i], bytes);
			DEBUGINATOR_assert(result != NULL);
			DEBUGINATOR_memset(result, 0, bytes);
			return result;
		}
	}

	DEBUGINATOR_assert(false);
	return NULL;
}

void debuginator__deallocate(TheDebuginator* debuginator, void* void_ptr) {
	char* ptr = (char*)void_ptr;
	DEBUGINATOR_assert(debuginator->memory_arena <= ptr && ptr < debuginator->memory_arena + debuginator->memory_arena_capacity);
	uintptr_t block_address = (uintptr_t)ptr;
	int capacity = debuginator->allocator_data.block_capacity;
	block_address /= capacity;
	block_address *= capacity;
	//char* block_ptr = (char*)block_address;
	DebuginatorBlockAllocator* allocator = *(DebuginatorBlockAllocator**)block_address;
	debuginator__block_deallocate(allocator, void_ptr);
	DEBUGINATOR_memset(void_ptr, 0xcc, allocator->element_size);
/*
	for (int i = 0; i < 4; i++) {
		if (ptr < debuginator->allocators[i].arena + debuginator->allocators[i].capacity) {
			debuginator->allocators[i].deallocate(&debuginator->allocators[i], void_ptr);
		}
	}*/
}

char* debuginator__copy_string(TheDebuginator* debuginator, const char* string, int length) {
	if (length == 0) {
		length = (int)DEBUGINATOR_strlen(string);
	}

	char* memory = (char*)debuginator__allocate(debuginator, length + 1);
	memcpy(memory, string, length);
	memory[length] = '\0';
	return memory;
}

DebuginatorAnimation* debuginator__get_free_animation(TheDebuginator* debuginator) {
	if (debuginator->animation_count == sizeof(debuginator->animations) / sizeof(*debuginator->animations)) {
		return NULL;
	}

	DebuginatorAnimation* animation = &debuginator->animations[debuginator->animation_count++];
	DEBUGINATOR_memset(animation, 0, sizeof(*animation));
	return animation;
}

//DebuginatorItem* debuginator_get_free_item(TheDebuginator* debuginator) {
	//DebuginatorItem* item;
	//if (debuginator->free_list_size > 0) {
	//	size_t free_index = debuginator->free_list[--debuginator->free_list_size];
	//	DEBUGINATOR_assert(free_index < debuginator->item_buffer_capacity);
	//	item = &debuginator->item_buffer[free_index];
	//}
	//else {
	//	DEBUGINATOR_assert(debuginator->item_buffer_size < debuginator->item_buffer_capacity);
	//	item = &debuginator->item_buffer[debuginator->item_buffer_size++];
	//}

	//DEBUGINATOR_memset(item, 0, sizeof(*item));
	//return item;
//}

void debuginator__set_total_height(DebuginatorItem* item, float height) {
	//if (height == 30 && item->is_folder) {
	//	height = 0;
	//}

	if (item->total_height == height) {
		return;
	}

	float diff = height - item->total_height;
	item->total_height = height;
	if (item->parent) {
		debuginator__set_total_height(item->parent, item->parent->total_height + diff);
	}
}

void debuginator__set_num_visible_children(DebuginatorItem* item, int diff) {
	DEBUGINATOR_assert(item->is_folder);
	DEBUGINATOR_assert(diff != 0 && item->folder.num_visible_children + diff >= 0);
	item->folder.num_visible_children += diff;
	if (item->folder.num_visible_children == 0 && item->parent != NULL) {
		debuginator__set_num_visible_children(item->parent, -1);
	}
	else if (item->folder.num_visible_children == diff && item->parent != NULL) {
		debuginator__set_num_visible_children(item->parent, 1);
	}
}

void debuginator__on_change_theme(DebuginatorItem* item, void* value, const char* value_title, void* app_userdata) {
	(void)app_userdata;
	(void)value_title;
	TheDebuginator* debuginator = (TheDebuginator*)item->user_data;
	debuginator->theme_index = *(int*)value;
	debuginator->theme = debuginator->themes[debuginator->theme_index];
}

void debuginator_set_title(TheDebuginator* debuginator, DebuginatorItem* item, const char* title, int title_length) {
	if (title_length == 0) {
		title_length = (int)DEBUGINATOR_strlen(title);
	}

	item->title = (char*)debuginator__allocate(debuginator, title_length + 1);
	memcpy((void*)item->title, title, title_length);
	item->title[title_length] = '\0';
}

DebuginatorItem* debuginator__next_visible_sibling(DebuginatorItem* item) {
	DebuginatorItem* sibling = item->next_sibling;
	while (sibling) {
		if (sibling->is_folder) {
			if (sibling->folder.num_visible_children > 0) {
				return sibling;
			}
		}
		else if (sibling->leaf.hot_index != -2 && !sibling->is_filtered) {
			return sibling;
		}

		sibling = sibling->next_sibling;
	}

	return NULL;
}

DebuginatorItem* debuginator__prev_visible_sibling(DebuginatorItem* item) {
	DebuginatorItem* sibling = item->prev_sibling;
	while (sibling) {
		if (sibling->is_folder) {
			if (sibling->folder.num_visible_children > 0) {
				return sibling;
			}
		}
		else if (sibling->leaf.hot_index != -2 && !sibling->is_filtered) {
			return sibling;
		}

		sibling = sibling->prev_sibling;
	}

	return NULL;
}

DebuginatorItem* debuginator__first_visible_child(DebuginatorItem* item) {
	if (item->folder.first_child == NULL) {
		return NULL;
	}

	if (item->folder.first_child->is_folder) {
		if (item->folder.first_child->folder.num_visible_children > 0) {
			return item->folder.first_child;
		}

		//return NULL;
	}
	else if (item->folder.first_child->leaf.hot_index != -2 && !item->folder.first_child->is_filtered) {
		return item->folder.first_child;
	}

	return debuginator__next_visible_sibling(item->folder.first_child);
}

DebuginatorItem* debuginator__find_first_leaf(DebuginatorItem* item) {
	if (!item->is_folder) {
		if (item->leaf.hot_index != -2 && !item->is_filtered) {
			return item;
		}
	}
	else {
		DebuginatorItem* child = debuginator__first_visible_child(item);
		DebuginatorItem* leaf = child == NULL ? NULL : debuginator__find_first_leaf(child);
		if (leaf) {
			return leaf;
		}
	}

	DebuginatorItem* sibling = debuginator__next_visible_sibling(item);
	while (sibling != NULL  ) {
		item = debuginator__find_first_leaf(sibling);
		if (item != NULL) {
			return item;
		}
		sibling = debuginator__next_visible_sibling(sibling);
	}

	return NULL;
}

DebuginatorItem* debuginator__find_last_leaf(DebuginatorItem* item) {
	if (!item->is_folder) {
		if (item->leaf.hot_index != -2 && !item->is_filtered) {
			return item;
		}
	}
	else {
		DebuginatorItem* child = debuginator__first_visible_child(item);
		while (debuginator__next_visible_sibling(child) != NULL) {
			child = debuginator__next_visible_sibling(child);
		}

		if (child) {
			DebuginatorItem* leaf = debuginator__find_last_leaf(child);
			if (leaf != NULL) {
				return leaf;
			}
		}
	}

	DebuginatorItem* sibling = debuginator__prev_visible_sibling(item);
	while (sibling != NULL) {
		item = debuginator__find_last_leaf(sibling);
		if (item != NULL) {
			return item;
		}
		sibling = debuginator__prev_visible_sibling(sibling);
	}

	return NULL;
}

DebuginatorItem* debuginator__next_item(DebuginatorItem* item) {
	while (item != NULL) {
		DebuginatorItem* sibling = debuginator__next_visible_sibling(item);
		DebuginatorItem* item_new = sibling == NULL ? NULL : debuginator__find_first_leaf(sibling);
		if (item_new != NULL) {
			return item_new;
		}

		item = item->parent;
	}

	return NULL;
}

DebuginatorItem* debuginator__prev_item(DebuginatorItem* item) {
	while (item != NULL) {
		DebuginatorItem* sibling = debuginator__prev_visible_sibling(item);
		DebuginatorItem* item_new = sibling == NULL ? NULL : debuginator__find_last_leaf(sibling);
		if (item_new != NULL) {
			return item_new;
		}

		item = item->parent;
	}

	return NULL;
}

DebuginatorItem* debuginator_nearest_visible_item(DebuginatorItem* item) {
	DebuginatorItem* found_item = debuginator__next_item(item);
	if (found_item == NULL) {
		found_item = debuginator__prev_item(item);
	}

	return found_item;
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

			// TODO do alphanumerical comparison here for sorting

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

DebuginatorItem* debuginator_new_folder_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* title, int title_length) {
	DebuginatorItem* folder_item = (DebuginatorItem*)debuginator__allocate(debuginator, sizeof(DebuginatorItem));
	folder_item->is_folder = true;
	folder_item->folder.num_visible_children = 0;
	debuginator_set_title(debuginator, folder_item, title, title_length);
	debuginator_set_parent(folder_item, parent);
	debuginator__set_total_height(folder_item, 30);
	return folder_item;
}

DebuginatorItem* debuginator_get_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* path, bool create_if_not_exist) {
	parent = parent == NULL ? debuginator->root : parent;
	const char* temp_path = path;
	while (true) {
		const char* next_slash = DEBUGINATOR_strchr(temp_path, '/');
		size_t path_part_length = next_slash ? next_slash - temp_path : DEBUGINATOR_strlen(temp_path);

		DebuginatorItem* current_item = NULL;
		DebuginatorItem* parent_child = parent->folder.first_child;
		while (parent_child) {
			const char* item_title = parent_child->title;
			size_t title_length = DEBUGINATOR_strlen(item_title); // strlen :(
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
				current_item = (DebuginatorItem*)debuginator__allocate(debuginator, sizeof(DebuginatorItem));
				debuginator_set_title(debuginator, current_item, temp_path, 0);
				debuginator_set_parent(current_item, parent);
			}

			return current_item;
		}
		else {
			// Found a folder
			if (current_item == NULL) {
				// Parent item doesn't exist yet
				parent = debuginator_new_folder_item(debuginator, parent, temp_path, (int)(next_slash - temp_path));
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
	debuginator__set_total_height(item, 30);

#ifndef DEBUGINATOR_own_value_titles
	// TODO
#endif

	if (item->leaf.hot_index >= num_values) {
		item->leaf.hot_index = num_values - 1;
	}

	if (debuginator->hot_item == NULL && !item->is_folder) {
		item->parent->folder.hot_child = item;
		debuginator->hot_item = item;
	}

	// In case the item existed previously from a load.
	if (item->leaf.description != NULL) {
		const char* loaded_value_title = item->leaf.description;
		for (int i = 0; i < item->leaf.num_values; i++) {
			if (strcmp(item->leaf.value_titles[i], loaded_value_title) == 0) {
				item->leaf.hot_index = i;
				debuginator_activate(debuginator, item);
				break;
			}
		}
	}
	item->leaf.description = description == NULL ? NULL : debuginator__copy_string(debuginator, description, 0);
	debuginator__set_num_visible_children(item->parent, 1);

	//TODO preserve hot item
	return item;
}

int debuginator_save(TheDebuginator* debuginator, DebuginatorSaveItemCallback callback, char* save_buffer, int save_buffer_size) {
	char current_full_path[256] = { 0 };
	int path_indices[8] = { 0 };
	int current_path_index = 0;

	DebuginatorItem* item = debuginator->root->folder.first_child;
	while (item != NULL) {
		if (item->is_folder) {
			if (item->folder.first_child != NULL) {
				DEBUGINATOR_strcpy_s(current_full_path + path_indices[current_path_index], 50, item->title); // HACK

				path_indices[current_path_index + 1] = path_indices[current_path_index] + (int)DEBUGINATOR_strlen(item->title);
				*(current_full_path + path_indices[current_path_index + 1]) = '/';
				path_indices[current_path_index + 1]++;

				++current_path_index;
				item = item->folder.first_child;

				continue;
			}
		}
		else {
			DEBUGINATOR_strcpy_s(current_full_path + path_indices[current_path_index], 50, item->title); // HACK
			path_indices[current_path_index + 1] = path_indices[current_path_index] + (int)DEBUGINATOR_strlen(item->title);
			//int current_path_length = path_indices[current_path_index + 1];

			if (item->leaf.active_index != item->leaf.default_index) {
				int saved = callback(current_full_path, item->leaf.value_titles[item->leaf.active_index], save_buffer, save_buffer_size);
				if (saved < 0) {
					return -1;
				}

				save_buffer += saved;
				save_buffer_size -= saved;
			}
		}

		if (item->next_sibling != NULL) {
			item = item->next_sibling;
		}
		else {
			while (item->parent != NULL && item->parent->next_sibling == NULL) {
				item = item->parent;
				--current_path_index;
			}

			if (item->parent == NULL) {
				// Went all the way 'back' to the menu root.
				break;
			}

			item = item->parent->next_sibling;
			--current_path_index;
		}
	}

	return save_buffer_size;
}


void debuginator_load_item(TheDebuginator* debuginator, const char* path, const char* value_title) {
	DebuginatorItem* item = debuginator_get_item(debuginator, NULL, path, false);
	if (item == NULL) {
		item = debuginator_create_array_item(debuginator, NULL, path, NULL, NULL, NULL, NULL, NULL, 0, 0);
		item->leaf.description = debuginator__copy_string(debuginator, value_title, 0); // Temporarily reuse description field
		item->leaf.hot_index = -2;
		debuginator__set_total_height(item, 0);
		debuginator__set_num_visible_children(item->parent, -1);
	}
	else if (item->is_folder) {
	}
	else {
		for (int i = 0; i < item->leaf.num_values; i++) {
			if (strcmp(item->leaf.value_titles[i], value_title) == 0) {
				item->leaf.hot_index = i;
				if (!debuginator->edit_types[item->leaf.edit_type].forget_state) {
					debuginator_activate(debuginator, item);
				}
				break;
			}
		}
	}
}

DebuginatorItem* debuginator_get_hot_item(TheDebuginator* debuginator) {
	return debuginator->hot_item;
}

void debuginator_set_hot_item(TheDebuginator* debuginator, const char* path) {
	DebuginatorItem* item = debuginator_get_item(debuginator, NULL, path, false);
	if (item == NULL) {
		return;
	}

	debuginator->hot_item = item;
	item->parent->folder.hot_child = item;
}

void debuginator_set_default_value(TheDebuginator* debuginator, const char* path, const char* value_title, int value_index) {
	DebuginatorItem* item = debuginator_get_item(debuginator, NULL, path, false);
	if (item == NULL || item->is_folder) {
		return;
	}
	
	if (value_title != NULL) {
		for (int i = 0; i < item->leaf.num_values; i++) {
			if (strcmp(value_title, item->leaf.value_titles[i]) == 0) {
				value_index = i;
				break;
			}
		}
	}

	if (0 <= value_index && value_index < item->leaf.num_values) {
		item->leaf.default_index = value_index;
	}
}

void debuginator_set_edit_type(TheDebuginator* debuginator, const char* path, DebuginatorItemEditorDataType edit_type) {
	DebuginatorItem* item = debuginator_get_item(debuginator, NULL, path, false);
	if (item == NULL) {
		return;
	}

	item->leaf.edit_type = edit_type;
}

// Note: If you remove the last visible item, you must create a new one under the root.
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
		debuginator->hot_item = debuginator_nearest_visible_item(item);
	}

	debuginator__set_total_height(item->parent, item->parent->total_height - item->total_height);
	debuginator__set_num_visible_children(item->parent, -1);

	debuginator__deallocate(debuginator, item->title);
	if (item->is_folder) {
		debuginator__deallocate(debuginator, item->leaf.description);
	}
	// TODO: Release item...
}

bool debuginator__distance_to_hot_item(DebuginatorItem* item, DebuginatorItem* hot_item, float* distance) {
	if (item == hot_item) {
		if (!item->is_folder && item->leaf.is_active) {
			*distance += 30 * (item->leaf.hot_index + 1);
		}
		return true;
	}

	*distance += 30;
	if (item->is_folder) {
		DebuginatorItem* child = debuginator__first_visible_child(item);
		while (child) {
			bool found = debuginator__distance_to_hot_item(child, hot_item, distance);
			if (found) {
				return true;
			}

			child = debuginator__next_visible_sibling(child);
		}
	}

	return false;
}

bool debuginator_is_filtering_enabled(TheDebuginator* debuginator) {
	return debuginator->filter_enabled;
}

void debuginator_set_filtering_enabled(TheDebuginator* debuginator, bool enabled) {
	debuginator->filter_enabled = enabled;
}

char* debuginator_get_filter(TheDebuginator* debuginator) {
	return debuginator->filter;
}

void debuginator_update_filter(TheDebuginator* debuginator, const char* wanted_filter) {
	const int filter_length = (int)DEBUGINATOR_strlen(wanted_filter);
	bool expanding_search = false;
	if (filter_length < DEBUGINATOR_strlen(debuginator->filter)) {
		expanding_search = true;
		if (debuginator->hot_item->user_data == (void*)0x12345678) {
			debuginator_remove_item(debuginator, debuginator->hot_item->title);
		}
	}
	else if (filter_length > DEBUGINATOR_strlen(debuginator->filter)) {
		// TODO do memcmp here to check for completely new filter.
		if (debuginator->hot_item->user_data == (void*)0x12345678) {
			DEBUGINATOR_strcpy_s(debuginator->filter, sizeof(debuginator->filter), wanted_filter);
			return;
		}
	}

	// Exact search
	// "el eb" matches "Debuginator/Help"
	// "oo " doesn't match "lolol"
	// "aa aa" doesn't match "Cars/Saab and Volvo"
	// "aa aa" matches "Caars/Saab"
	bool exact_search = false;
	for (size_t i = 0; i < DEBUGINATOR_strlen(wanted_filter); i++) {
		if (wanted_filter[i] == ' ') {
			exact_search = true;
			break;
		}
	}

	char filter[32] = { 0 };
	//if (!exact_search) {
		for (size_t i = 0; i < 20; i++) {
			filter[i] = (char)DEBUGINATOR_tolower(wanted_filter[i]);
		}
	//}

	char current_full_path[128] = { 0 };
	int path_indices[8] = { 0 };
	int current_path_index = 0;

	int best_score = -1;
	DebuginatorItem* best_item = NULL;

	DebuginatorItem* item = debuginator->root->folder.first_child;
	while (item != NULL) {
		if (item->is_folder) {
			if (item->folder.first_child != NULL) {
				DEBUGINATOR_strcpy_s(current_full_path + path_indices[current_path_index], 20, item->title);

				path_indices[current_path_index+1] = path_indices[current_path_index] + (int)DEBUGINATOR_strlen(item->title);
				*(current_full_path + path_indices[current_path_index + 1]) = ' ';
				path_indices[current_path_index + 1]++;

				//if (!exact_search) {
					for (int i = path_indices[current_path_index]; i < path_indices[current_path_index + 1]; i++) {
						current_full_path[i] = (char)DEBUGINATOR_tolower(current_full_path[i]);
					}
				//}

				++current_path_index;
				item = item->folder.first_child;

				continue;
			}
		}
		else {
			bool taken_chars[128] = { 0 };
			DEBUGINATOR_strcpy_s(current_full_path + path_indices[current_path_index], 50, item->title);
			path_indices[current_path_index + 1] = path_indices[current_path_index] + (int)DEBUGINATOR_strlen(item->title);
			int current_path_length = path_indices[current_path_index + 1];
			
			//if (!exact_search) {
				for (size_t i = path_indices[current_path_index]; i < current_path_length; i++) {
					current_full_path[i] = (char)DEBUGINATOR_tolower(current_full_path[i]);
				}
			//}

			int score = -1;
			bool is_filtered = false;
			
			// LOL1010
			// 100
			int filter_part = 0;
			while (filter[filter_part] != '\0') {
				if (filter[filter_part] == ' ') {
					++filter_part;
					continue;
				}

				int path_part = 0;
				int matches[8] = { 0 };
				int match_count = 0;
				while (current_full_path[path_part] != '\0') {
					bool filter_part_found = false;
					for (int path_i = path_part; path_i < current_path_length; path_i++) {
						if (current_full_path[path_i] == filter[filter_part] && taken_chars[path_i] == false) {
							path_part = path_i;
							filter_part_found = true;
							break;
						}
					}

					if (!filter_part_found) {
						//match_count = 0;
						break;
					}

					int match_length = 0;
					const char* filter_char = filter + filter_part;
					const char* path_char = current_full_path + path_part;
					while (*filter_char++ == *path_char++) {
						match_length++;
						if (*filter_char == '\0' || *filter_char == ' ' || taken_chars[path_part + match_length] == true) {
							break;
						}
					}

					if (exact_search) {
						if (filter[filter_part + match_length] != '\0' && filter[filter_part + match_length] != ' ') {
							path_part += 1;
							continue;
						}
					}

					matches[match_count++] = path_part;
					matches[match_count++] = match_length;
					path_part += match_length;

					if (match_count == 8) {
						break;
					}
				}

				int best_match_index = -1;
				int best_match_score = -1;
				for (int i = 0; i < match_count; i += 2) {
					int match_index = matches[i];
					int match_length = matches[i + 1];
					int word_break_start = match_index == 0 
						|| current_full_path[match_index - 1] == ' '
						|| (!DEBUGINATOR_isalpha(current_full_path[match_index - 1]) && DEBUGINATOR_isalpha(current_full_path[match_index]))
						|| (!DEBUGINATOR_isdigit(current_full_path[match_index - 1]) && DEBUGINATOR_isdigit(current_full_path[match_index]));
					int word_break_end = match_index + match_length == current_path_length
						|| current_full_path[match_index + match_length] == ' '
						|| (!DEBUGINATOR_isalpha(current_full_path[match_index + match_length]) && DEBUGINATOR_isalpha(current_full_path[match_index]))
						|| (!DEBUGINATOR_isdigit(current_full_path[match_index + match_length]) && DEBUGINATOR_isdigit(current_full_path[match_index]));
					int match_in_item_title = match_index >= path_indices[current_path_index];
					int match_score = (word_break_start * 10 + word_break_end * 5 + match_in_item_title * 10 + match_length) * match_length;
					if (match_score > best_match_score) {
						best_match_score = match_score;
						best_match_index = i;
					}
				}

				if (best_match_index == -1) {
					is_filtered = true;
					score = -1;
					break; // Filter not valid
				}
				else {
					filter_part += matches[best_match_index + 1];
					score += best_match_score;
					//is_filtered = false;
					for (int match_i = 0; match_i < matches[best_match_index + 1]; match_i++) {
						taken_chars[matches[best_match_index] + match_i] = true;
					}
				}
			}

			if (is_filtered && !item->is_filtered) {
				debuginator__set_total_height(item, 0);
				debuginator__set_num_visible_children(item->parent, -1);
				item->leaf.is_active = false;
			}
			else if (!is_filtered && item->is_filtered) {
				debuginator__set_total_height(item, 30); //Hacky
				debuginator__set_num_visible_children(item->parent, 1);
			}

			item->is_filtered = is_filtered;

			if (score > best_score) {
				if (item == debuginator->hot_item) {
					score++;
				}
				best_score = score;
				best_item = item;
			}
		}

		if (item->next_sibling != NULL) {
			item = item->next_sibling;
		}
		else {
			while (item->parent != NULL && item->parent->next_sibling == NULL) {
				item = item->parent;
				--current_path_index;
			}

			if (item->parent == NULL) {
				// Went all the way 'back' to the menu root.
				break;
			}

			item = item->parent->next_sibling;
			--current_path_index;
		}
	}

	if (expanding_search && debuginator->hot_item != NULL) {
		// We're good. Just keep the previously hot item; it can't have disappeared.
	}
	else if (best_item != NULL) {
		debuginator->hot_item = best_item;
		best_item->parent->folder.hot_child = best_item;
	}
	else if (filter_length == 0) {
		// Happens when we remove the last letter of the search, and go from only
		// "No items found" to all items
		debuginator->hot_item = debuginator__find_first_leaf(debuginator->root);
	}
	else {
		DebuginatorItem* fallback = debuginator_create_array_item(debuginator, NULL, "No items found", "Your search filter returned no results.", NULL, (void*)0x12345678, NULL, NULL, 0, 0);
		debuginator->hot_item = fallback;
		fallback->parent->folder.hot_child = fallback;
	}

	float distance_from_root_to_hot_item = 0;
	debuginator__distance_to_hot_item(debuginator->root, debuginator->hot_item, &distance_from_root_to_hot_item);
	float wanted_y = debuginator->size.y * debuginator->focus_height;
	float distance_to_wanted_y = wanted_y - distance_from_root_to_hot_item;
	debuginator->current_height_offset = distance_to_wanted_y;

	DEBUGINATOR_strcpy_s(debuginator->filter, sizeof(debuginator->filter), filter);
}

void debuginator_get_default_config(TheDebuginatorConfig* config) {
	DEBUGINATOR_memset(config, 0, sizeof(*config));

	config->create_default_debuginator_items = true;
	config->open_direction = 1;
	config->focus_height = 0.65f;
	config->left_aligned = true;

	// Initialize default themes
	DebuginatorTheme* themes = config->themes;

	// Classic theme
	themes[0].colors[DEBUGINATOR_Background] = debuginator__color(25, 50, 25, 220);
	themes[0].colors[DEBUGINATOR_FolderTitle] = debuginator__color(255, 255, 255, 255);
	themes[0].colors[DEBUGINATOR_ItemTitle] = debuginator__color(120, 120, 0, 250);
	themes[0].colors[DEBUGINATOR_ItemTitleOverridden] = debuginator__color(200, 200, 0, 255);
	themes[0].colors[DEBUGINATOR_ItemTitleHot] = debuginator__color(230, 230, 200, 255);
	themes[0].colors[DEBUGINATOR_ItemTitleActive] = debuginator__color(100, 255, 100, 255);
	themes[0].colors[DEBUGINATOR_ItemTitleActive1] = debuginator__color(100, 255, 100, 255);
	themes[0].colors[DEBUGINATOR_ItemTitleActive2] = debuginator__color(200, 255, 200, 255);
	themes[0].colors[DEBUGINATOR_ItemDescription] = debuginator__color(150, 150, 150, 255);
	themes[0].colors[DEBUGINATOR_ItemValueDefault] = debuginator__color(50, 150, 50, 200);
	themes[0].colors[DEBUGINATOR_ItemValueOverridden] = debuginator__color(100, 255, 100, 200);
	themes[0].colors[DEBUGINATOR_ItemValueHot] = debuginator__color(100, 255, 100, 200);
	themes[0].colors[DEBUGINATOR_LineHighlight] = debuginator__color(100, 100, 50, 150);
	themes[0].colors[DEBUGINATOR_ItemEditorOff] = debuginator__color(250, 100, 100, 200);
	themes[0].colors[DEBUGINATOR_ItemEditorOn] = debuginator__color(130, 220, 255, 200);
	themes[0].fonts[DEBUGINATOR_ItemDescription].italic = true;

	// Neon theme
	themes[1].colors[DEBUGINATOR_Background] = debuginator__color(15, 15, 30, 220);
	themes[1].colors[DEBUGINATOR_FolderTitle] = debuginator__color(255, 255, 255, 255);
	themes[1].colors[DEBUGINATOR_ItemTitle] = debuginator__color(120, 120, 180, 250);
	themes[1].colors[DEBUGINATOR_ItemTitleOverridden] = debuginator__color(150, 150, 200, 255);
	themes[1].colors[DEBUGINATOR_ItemTitleHot] = debuginator__color(220, 220, 250, 255);
	themes[1].colors[DEBUGINATOR_ItemTitleActive] = debuginator__color(100, 100, 255, 255);
	themes[1].colors[DEBUGINATOR_ItemTitleActive1] = debuginator__color(100, 255, 100, 255);
	themes[1].colors[DEBUGINATOR_ItemTitleActive2] = debuginator__color(200, 200, 255, 255);
	themes[1].colors[DEBUGINATOR_ItemDescription] = debuginator__color(150, 150, 150, 255);
	themes[1].colors[DEBUGINATOR_ItemValueDefault] = debuginator__color(50, 50, 150, 200);
	themes[1].colors[DEBUGINATOR_ItemValueOverridden] = debuginator__color(100, 100, 255, 200);
	themes[1].colors[DEBUGINATOR_ItemValueHot] = debuginator__color(100, 100, 255, 200);
	themes[1].colors[DEBUGINATOR_LineHighlight] = debuginator__color(70, 70, 130, 150);
	themes[1].colors[DEBUGINATOR_ItemEditorOff] = debuginator__color(0, 0, 255, 200);
	themes[1].colors[DEBUGINATOR_ItemEditorOn] = debuginator__color(130, 220, 255, 200);

	// Black & White theme
	themes[2].colors[DEBUGINATOR_Background] = debuginator__color(25, 25, 25, 220);
	themes[2].colors[DEBUGINATOR_FolderTitle] = debuginator__color(255, 255, 255, 255);
	themes[2].colors[DEBUGINATOR_ItemTitle] = debuginator__color(120, 120, 120, 250);
	themes[2].colors[DEBUGINATOR_ItemTitleOverridden] = debuginator__color(200, 200, 200, 255);
	themes[2].colors[DEBUGINATOR_ItemTitleHot] = debuginator__color(230, 230, 230, 255);
	themes[2].colors[DEBUGINATOR_ItemTitleActive] = debuginator__color(100, 100, 100, 255);
	themes[2].colors[DEBUGINATOR_ItemTitleActive1] = debuginator__color(100, 100, 100, 255);
	themes[2].colors[DEBUGINATOR_ItemTitleActive2] = debuginator__color(200, 200, 200, 255);
	themes[2].colors[DEBUGINATOR_ItemDescription] = debuginator__color(150, 150, 150, 255);
	themes[2].colors[DEBUGINATOR_ItemValueDefault] = debuginator__color(100, 100, 100, 200);
	themes[2].colors[DEBUGINATOR_ItemValueOverridden] = debuginator__color(200, 200, 200, 200);
	themes[2].colors[DEBUGINATOR_ItemValueHot] = debuginator__color(255, 255, 255, 200);
	themes[2].colors[DEBUGINATOR_LineHighlight] = debuginator__color(100, 100, 100, 150);
	themes[2].colors[DEBUGINATOR_ItemEditorOff] = debuginator__color(100, 100, 100, 200);
	themes[2].colors[DEBUGINATOR_ItemEditorOn] = debuginator__color(250, 250, 250, 200);

	// Beige
	themes[3].colors[DEBUGINATOR_Background] = debuginator__color(255, 240, 220, 220);
	themes[3].colors[DEBUGINATOR_FolderTitle] = debuginator__color(0, 0, 0, 255);
	themes[3].colors[DEBUGINATOR_ItemTitle] = debuginator__color(120, 120, 120, 250);
	themes[3].colors[DEBUGINATOR_ItemTitleOverridden] = debuginator__color(200, 200, 200, 255);
	themes[3].colors[DEBUGINATOR_ItemTitleHot] = debuginator__color(230, 230, 230, 255);
	themes[3].colors[DEBUGINATOR_ItemTitleActive] = debuginator__color(100, 100, 100, 255);
	themes[3].colors[DEBUGINATOR_ItemTitleActive1] = debuginator__color(100, 100, 100, 255);
	themes[3].colors[DEBUGINATOR_ItemTitleActive2] = debuginator__color(0, 0, 0, 255);
	themes[3].colors[DEBUGINATOR_ItemDescription] = debuginator__color(150, 150, 150, 255);
	themes[3].colors[DEBUGINATOR_ItemValueDefault] = debuginator__color(100, 100, 100, 200);
	themes[3].colors[DEBUGINATOR_ItemValueOverridden] = debuginator__color(200, 200, 200, 200);
	themes[3].colors[DEBUGINATOR_ItemValueHot] = debuginator__color(255, 255, 255, 200);
	themes[3].colors[DEBUGINATOR_LineHighlight] = debuginator__color(100, 100, 100, 150);
	themes[3].colors[DEBUGINATOR_ItemEditorOff] = debuginator__color(200, 150, 100, 200);
	themes[3].colors[DEBUGINATOR_ItemEditorOn] = debuginator__color(250, 250, 100, 200);

	config->edit_types[DEBUGINATOR_EditTypeArray].quick_draw = debuginator__quick_draw_default;
	config->edit_types[DEBUGINATOR_EditTypeArray].expanded_draw = debuginator__expanded_draw_default;
	config->edit_types[DEBUGINATOR_EditTypeActionArray].expanded_draw = debuginator__expanded_draw_default;
	config->edit_types[DEBUGINATOR_EditTypeActionArray].forget_state = true;
	config->edit_types[DEBUGINATOR_EditTypeBoolean].quick_draw = debuginator__quick_draw_boolean;
	config->edit_types[DEBUGINATOR_EditTypeBoolean].expanded_draw = debuginator__expanded_draw_boolean;
	config->edit_types[DEBUGINATOR_EditTypePreset].quick_draw = debuginator__quick_draw_preset;
	config->edit_types[DEBUGINATOR_EditTypePreset].expanded_draw = debuginator__expanded_draw_preset;
}

void debuginator_create(TheDebuginatorConfig* config, TheDebuginator* debuginator) {
	DEBUGINATOR_assert(config->draw_rect != NULL);
	DEBUGINATOR_assert(config->draw_text != NULL);
	//DEBUGINATOR_assert(config->app_user_data);
	DEBUGINATOR_assert(config->word_wrap != NULL);
	DEBUGINATOR_assert(config->memory_arena != NULL);
	DEBUGINATOR_assert(config->memory_arena_capacity > 0);
	//DEBUGINATOR_assert(config->open_direction == -1 || config->open_direction == 1);
	DEBUGINATOR_assert(config->size.x > 0 && config->size.y > 0);
	DEBUGINATOR_assert(config->screen_resolution.x > 0 && config->screen_resolution.y > 0);

	DEBUGINATOR_memset(debuginator, 0, sizeof(*debuginator));
	debuginator->memory_arena = config->memory_arena;
	debuginator->memory_arena_capacity = config->memory_arena_capacity;

	debuginator->allocator_data.arena_end = debuginator->memory_arena + debuginator->memory_arena_capacity;
	debuginator->allocator_data.block_capacity = 0x1000;
	debuginator->allocator_data.next_free_block = (char*)((((uintptr_t)debuginator->memory_arena + 0x1000 - 1) / 0x1000) * 0x1000);
	debuginator__block_allocator_init(&debuginator->allocators[0], 8, &debuginator->allocator_data);
	debuginator__block_allocator_init(&debuginator->allocators[1], 32, &debuginator->allocator_data);
	debuginator__block_allocator_init(&debuginator->allocators[2], sizeof(DebuginatorItem), &debuginator->allocator_data);
	debuginator__block_allocator_init(&debuginator->allocators[3], 1000, &debuginator->allocator_data);

	debuginator->draw_rect = config->draw_rect;
	debuginator->draw_text = config->draw_text;
	debuginator->word_wrap = config->word_wrap;
	debuginator->text_size = config->text_size;
	debuginator->app_user_data = config->app_user_data;

	debuginator->size = config->size;
	debuginator->open_direction = config->open_direction;
	debuginator->focus_height = config->focus_height;
	debuginator->screen_resolution = config->screen_resolution;
	debuginator->left_aligned= config->left_aligned;

	if (debuginator->left_aligned) {
		debuginator->open_direction = 1;
		debuginator->root_position.x = -debuginator->size.x;
	}
	else {
		debuginator->open_direction = -1;
		debuginator->root_position.x = debuginator->screen_resolution.x;
	}

	memcpy(debuginator->edit_types, config->edit_types, sizeof(debuginator->edit_types));
	memcpy(debuginator->themes, config->themes, sizeof(debuginator->themes));
	debuginator->theme_index = 0;
	debuginator->theme = debuginator->themes[0];

	// Create root
	DebuginatorItem* item = debuginator_new_folder_item(debuginator, NULL, "Menu Root", 0);
	debuginator->root = item;

	if (config->create_default_debuginator_items) {
		{
			debuginator_create_array_item(debuginator, NULL, "Debuginator/Help/About",
				"The Debuginator is an open source debug menu. New versions can be found here: https://github.com/Srekel/the-debuginator", NULL, NULL,
				NULL, NULL, 0, 0);
			debuginator_create_array_item(debuginator, NULL, "Debuginator/Help/Keyboard default usage",
				"Open the menu with Right Arrow. \nClose it with Left Arrow. \nUse all arrow keys to navigate. \nRight Arrow is also used to change value on a menu item.", NULL, NULL,
				NULL, NULL, 0, 0);
			debuginator_create_array_item(debuginator, NULL, "Debuginator/Help/Keyboard default advanced usage",
				"Hold CTRL for faster navigation and item toggling. \nEscape to quickly close the menu.\nBackspace to toggle search.", NULL, NULL,
				NULL, NULL, 0, 0);
		}
		{
			static int theme_indices[4] = { 0, 1, 2, 3 };
			static const char* string_titles[4] = { "Classic", "Blue", "Black & White", "Beige" };
			debuginator_create_array_item(debuginator, NULL, "Debuginator/Theme",
				"Change color theme of The Debuginator.", debuginator__on_change_theme, debuginator,
				string_titles, (void*)theme_indices, 4, sizeof(theme_indices[0]));
		}
	}
}

void debuginator_print(DebuginatorItem* item, int indentation) {
	DEBUGINATOR_debug_print("%*s%s\n", indentation, "", item->title);
	if (item->is_folder) {
		item = debuginator__first_visible_child(item);
		while (item) {
			debuginator_print(item, indentation + 4);
			item = debuginator__next_visible_sibling(item);
		}
	}
	else {
		for (size_t i = 0; i < item->leaf.num_values; i++) {
			DEBUGINATOR_debug_print("%*s%s\n", indentation + 4, "", item->leaf.value_titles[i]);
		}
	}
}

void debuginator_update(TheDebuginator* debuginator, float dt) {
	debuginator->dt = dt;
	debuginator->draw_timer += dt * 5;
	if (debuginator->is_open && debuginator->openness < 1) {
		debuginator->openness_timer += dt * 5;
		if (debuginator->openness_timer > 1) {
			debuginator->openness_timer = 1;
		}

		debuginator->openness = debuginator__ease_out(debuginator->openness_timer, 0, 1, 1);
	}

	else if (!debuginator->is_open && debuginator->openness > 0) {
		debuginator->openness_timer -= dt * 5;
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


float debuginator_draw_item(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2 offset, bool hot);

void debuginator_draw(TheDebuginator* debuginator, float dt) {
	// TODO return if fully closed

	// update theme opacity
	DebuginatorTheme* source_theme = &debuginator->themes[debuginator->theme_index];
	for (size_t i = 0; i < DEBUGINATOR_NumDrawTypes; i++) {
		debuginator->theme.colors[i].a = (unsigned char)(source_theme->colors[i].a * debuginator->openness);
	}

	float lerp_t = (float)(DEBUGINATOR_sin(debuginator->draw_timer * 2) + 1) * 0.5f;
	debuginator->theme.colors[DEBUGINATOR_ItemTitleActive].r = (unsigned char)debuginator__lerp((float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive1].r, (float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive2].r, lerp_t);
	debuginator->theme.colors[DEBUGINATOR_ItemTitleActive].g = (unsigned char)debuginator__lerp((float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive1].g, (float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive2].g, lerp_t);
	debuginator->theme.colors[DEBUGINATOR_ItemTitleActive].b = (unsigned char)debuginator__lerp((float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive1].b, (float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive2].b, lerp_t);

	// Background
	DebuginatorVector2 offset = debuginator->root_position;
	offset.x += debuginator__lerp(0, debuginator->open_direction * debuginator->size.x, debuginator->openness);
	debuginator->draw_rect(offset, debuginator->size, debuginator->theme.colors[DEBUGINATOR_Background], debuginator->app_user_data);

	offset.y = debuginator->current_height_offset;

	// Draw all items
	offset.x += 10;
	DebuginatorItem* item_to_draw = debuginator__first_visible_child(debuginator->root);
	while (item_to_draw && offset.y < -30) {
		// We'll start to draw off-screen which we don't want.
		if (offset.y + item_to_draw->total_height < 0) {
			// Whole item is off-screen, skip to sibling
			offset.y += item_to_draw->total_height;
			item_to_draw = debuginator__next_visible_sibling(item_to_draw);
		}
		else if (item_to_draw->is_folder) {
			// Part of item is off-screen, find which child to draw
			offset.x += 20;
			offset.y += 30;
			item_to_draw = debuginator__first_visible_child(item_to_draw);
		}
		else {
			DEBUGINATOR_assert(false);
		}
	}

	while (item_to_draw && offset.y < debuginator->size.y) {
		debuginator_draw_item(debuginator, item_to_draw, offset, debuginator->hot_item == item_to_draw);
		offset.y += item_to_draw->total_height;
		while (item_to_draw && debuginator__next_visible_sibling(item_to_draw) == NULL) {
			offset.x -= 20;
			item_to_draw = item_to_draw->parent;
		}
		
		if (item_to_draw == NULL) {
			break;
		}

		item_to_draw = debuginator__next_visible_sibling(item_to_draw);
	}

	// Update animations
	int running_animations = debuginator->animation_count; // Can be cleverer I guess
	for (int i = 0; i < debuginator->animation_count; i++) {
		DebuginatorAnimation* animation = &debuginator->animations[i];
		if (animation->time == animation->duration) {
			--running_animations;
			continue;
		}

		animation->time += dt;
		if (animation->time > animation->duration) {
			animation->time = animation->duration;
		}
		if (animation->type == DEBUGINATOR_ItemActivate) {
			float distance_from_root_to_item = 0;
			debuginator__distance_to_hot_item(debuginator->root, animation->data.item_activate.item, &distance_from_root_to_item);
			
			DebuginatorVector2 start_position = animation->data.item_activate.start_pos;
			DebuginatorVector2 end_position;
			end_position.x = debuginator->openness * 500 - 200;
			end_position.y = distance_from_root_to_item + debuginator->current_height_offset - 30;
			if (animation->data.item_activate.item->leaf.is_active) {
				end_position.y -= (animation->data.item_activate.item->leaf.hot_index + 1) * 30; // HACK! for description :(
			}


			DebuginatorVector2 position;
			position.x = debuginator__ease_out(animation->time, start_position.x, end_position.x - start_position.x, animation->duration);
			position.y = debuginator__ease_out(animation->time, start_position.y, end_position.y - start_position.y, animation->duration);
			DebuginatorFont* font = &debuginator->theme.fonts[DEBUGINATOR_ItemTitle];
			const char* text = animation->data.item_activate.item->leaf.value_titles[animation->data.item_activate.value_index];
			debuginator->draw_text(text, &position, &debuginator->theme.colors[DEBUGINATOR_ItemTitleActive], font, debuginator->app_user_data);

		}
	}

	if (running_animations == 0) {
		debuginator->animation_count = 0;
	}
	offset.x -= 10;

	bool filter_hint_mode = !debuginator->filter_enabled && debuginator->current_height_offset > 100;
	if (debuginator->filter_enabled || filter_hint_mode) {
		debuginator->filter_timer += dt * 5;
		if (debuginator->filter_timer > 1) {
			debuginator->filter_timer = 1;
		}
		if (filter_hint_mode && debuginator->filter_timer > 0.5f) {
			debuginator->filter_timer = 0.5f;
		}
	}
	else {
		debuginator->filter_timer -= dt * 10;
		if (debuginator->filter_timer < 0) {
			debuginator->filter_timer = 0;
		}
	}

	if (debuginator->filter_timer > 0) {
		float alpha = debuginator->filter_timer * (filter_hint_mode ? 0.5f : 1);
		DebuginatorVector2 filter_pos = debuginator__vector2(debuginator->openness * debuginator->size.x - 450, 25);
		DebuginatorVector2 filter_size = debuginator__vector2(150 + (debuginator->size.x - 150) * debuginator->filter_timer, 50);
		DebuginatorColor filter_color = debuginator__color(50, 100, 50, (int)(200 * debuginator->filter_timer * alpha));
		debuginator->draw_rect(filter_pos, filter_size, filter_color, debuginator->app_user_data);

		DebuginatorVector2 header_text_size = debuginator->text_size("Search: ", &debuginator->theme.fonts[DEBUGINATOR_ItemTitleActive], debuginator->app_user_data);
		filter_size.x = header_text_size.x + 40;
		debuginator->draw_rect(filter_pos, filter_size, filter_color, debuginator->app_user_data);

		filter_pos.x += 20;
		filter_pos.y = filter_pos.y + filter_size.y / 2 - header_text_size.y / 2;

		DebuginatorColor header_color = debuginator->theme.colors[DEBUGINATOR_ItemTitleActive];
		header_color.a = (unsigned char)(header_color.a * alpha);
		debuginator->draw_text("Search: ", &filter_pos, &header_color, &debuginator->theme.fonts[DEBUGINATOR_ItemTitleActive], debuginator->app_user_data);

		filter_pos.x += 40;
		filter_pos.x += header_text_size.x;
		if (strchr(debuginator->filter, ' ')) {
			// Exact search mode
			DebuginatorVector2 underline_size;
			underline_size.y = 1;
			char letter[2] = { 0 };
			for (int i = 0; i < DEBUGINATOR_strlen(debuginator->filter); i++) {
				letter[0] = debuginator->filter[i];
				debuginator->draw_text(letter, &filter_pos, &debuginator->theme.colors[DEBUGINATOR_ItemTitleActive], &debuginator->theme.fonts[DEBUGINATOR_ItemTitleActive], debuginator->app_user_data);
				DebuginatorVector2 letter_text_size = debuginator->text_size(letter, &debuginator->theme.fonts[DEBUGINATOR_ItemTitleActive], debuginator->app_user_data);
				underline_size.x = letter_text_size.x;
				if (letter[0] != ' ') {
					DebuginatorVector2 underline_pos = debuginator__vector2(filter_pos.x, filter_pos.y + letter_text_size.y);
					debuginator->draw_rect(underline_pos, underline_size, debuginator->theme.colors[DEBUGINATOR_ItemValueHot], debuginator->app_user_data);
				}
				filter_pos.x += letter_text_size.x;
			}
		}
		else {
			debuginator->draw_text(debuginator->filter, &filter_pos, &debuginator->theme.colors[DEBUGINATOR_ItemTitleActive], &debuginator->theme.fonts[DEBUGINATOR_ItemTitleActive], debuginator->app_user_data);
			DebuginatorVector2 filter_text_size = debuginator->text_size(debuginator->filter, &debuginator->theme.fonts[DEBUGINATOR_ItemTitleActive], debuginator->app_user_data);
			filter_pos.x += filter_text_size.x;
		}

		if (debuginator->filter_enabled) {
			DebuginatorVector2 caret_size = debuginator__vector2(10, header_text_size.y);
			DebuginatorVector2 caret_pos = debuginator__vector2(filter_pos.x, filter_pos.y);
			filter_color.r = 150;
			filter_color.g = 250;
			filter_color.b = 150;
			filter_color.a = alpha * DEBUGINATOR_sin(debuginator->draw_timer) < 0.5 ? 220 : 50;
			debuginator->draw_rect(caret_pos, caret_size, filter_color, debuginator->app_user_data);
		}
		else if (filter_hint_mode) {
			//filter_pos.x += 50;
			DebuginatorColor hint_color = header_color;
			hint_color.a = (unsigned char)(hint_color.a * alpha);
			debuginator->draw_text("(backspace)", &filter_pos, &hint_color, &debuginator->theme.fonts[DEBUGINATOR_ItemTitleActive], debuginator->app_user_data);
		}
	}
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
			debuginator->draw_rect(debuginator__vector2(0, offset.y - 5), debuginator__vector2(500, 30), debuginator->theme.colors[DEBUGINATOR_LineHighlight], debuginator->app_user_data);
		}

		unsigned color_index = item == debuginator->hot_item ? DEBUGINATOR_ItemTitleActive : (hot ? DEBUGINATOR_ItemTitleHot : DEBUGINATOR_FolderTitle);
		debuginator->draw_text(item->title, &offset, &debuginator->theme.colors[color_index], &debuginator->theme.fonts[DEBUGINATOR_ItemTitle], debuginator->app_user_data);
		offset.x += 20;
		DebuginatorItem* child = debuginator__first_visible_child(item);
		while (child) {
			offset.y += 30;
			if (offset.y > debuginator->size.y) {
				break;
			}

			offset.y = debuginator_draw_item(debuginator, child, offset, debuginator->hot_item == child);
			child = debuginator__next_visible_sibling(child);
		}
	}
	else {
		if (debuginator->hot_item == item && (!item->leaf.is_active || item->leaf.num_values == 0)) {
			DebuginatorVector2 line_pos = debuginator__vector2(debuginator->openness * 500 - 500, offset.y - 5);
			if (!debuginator->left_aligned) {
				line_pos.x = debuginator->screen_resolution.x - debuginator->openness * debuginator->size.x;
			}
			debuginator->draw_rect(line_pos, debuginator__vector2(500, 30), debuginator->theme.colors[DEBUGINATOR_LineHighlight], debuginator->app_user_data);
		}

		bool is_overriden = item->leaf.active_index != item->leaf.default_index && !debuginator->edit_types[item->leaf.edit_type].forget_state;
		unsigned default_color_index = is_overriden ? DEBUGINATOR_ItemTitleOverridden : DEBUGINATOR_ItemTitle;
		unsigned color_index = item == debuginator->hot_item && !item->leaf.is_active ? DEBUGINATOR_ItemTitleActive : (hot ? DEBUGINATOR_ItemTitleHot : default_color_index);
		DebuginatorFont* font = &debuginator->theme.fonts[DEBUGINATOR_ItemTitle];
		debuginator->draw_text(item->title, &offset, &debuginator->theme.colors[color_index], font, debuginator->app_user_data);

		if (debuginator->edit_types[item->leaf.edit_type].quick_draw != NULL) {
			debuginator->edit_types[item->leaf.edit_type].quick_draw(debuginator, item, &offset);
		}
		
		if (item->leaf.is_active) {
			offset.x += 20;

			const char* description = item->leaf.description;
			float description_width = debuginator->size.x - 50 - offset.x;
			if (!debuginator->left_aligned) {
				description_width = debuginator->screen_resolution.x - offset.x;
			}

			char description_line_to_draw[64];
			float description_height = 0;
			unsigned row_lengths[32];
			unsigned row_count = 0;
			debuginator->word_wrap(description, debuginator->theme.fonts[DEBUGINATOR_ItemDescription], description_width, &row_count, row_lengths, 32, debuginator->app_user_data);
			unsigned row_index = 0;
			for (unsigned i = 0; i < row_count; i++) {
				unsigned row_index_end = row_index + row_lengths[i];
				const char* description_line = description + row_index;
				DEBUGINATOR_strncpy_s(description_line_to_draw, 64, description_line, DEBUGINATOR_min(row_index_end - row_index, 64));
				row_index = row_index_end;
				while (description[row_index] == '\n') {
					++row_index;
				}
				offset.y += 30;
				description_height += 30;
				debuginator->draw_text(description_line_to_draw, &offset, &debuginator->theme.colors[DEBUGINATOR_ItemDescription], &debuginator->theme.fonts[DEBUGINATOR_ItemDescription], debuginator->app_user_data);
			}

			// Feels kinda ugly to do this here but... works for now.
			debuginator__set_total_height(item, 30 + description_height + 30 * (item->leaf.num_values));

			debuginator->edit_types[item->leaf.edit_type].expanded_draw(debuginator, item, &offset);

		}
	}

	return offset.y;
}

bool debuginator_is_open(TheDebuginator* debuginator) {
	return debuginator->is_open;
}

void debuginator_set_open(TheDebuginator* debuginator, bool is_open) {
	debuginator->is_open = is_open;
}

void debuginator_activate(TheDebuginator* debuginator, DebuginatorItem* item) {
	item->leaf.draw_t = 0;
	if (item->leaf.num_values == 0) {
		return;
	}

	item->leaf.active_index = item->leaf.hot_index;

	DebuginatorAnimation* animation = debuginator__get_free_animation(debuginator);
	if (animation != NULL) {
		animation->type = DEBUGINATOR_ItemActivate;
		animation->data.item_activate.item = item;
		animation->data.item_activate.value_index = item->leaf.hot_index;
		animation->duration = 0.5f;

		DebuginatorItem* parent = item->parent;
		while (parent) {
			animation->data.item_activate.start_pos.x += 20;
			parent = parent->parent;
		}

		float y_dist_to_root = 0;
		debuginator__distance_to_hot_item(debuginator->root, item, &y_dist_to_root);
		animation->data.item_activate.start_pos.y = y_dist_to_root + debuginator->current_height_offset - 30; // whyyy
		if (item->leaf.is_active) {
			animation->data.item_activate.start_pos.y += 30;
		}
	}

	if (item->leaf.on_item_changed_callback == NULL) {
		return;
	}

	void* value = ((char*)item->leaf.values) + item->leaf.hot_index * item->leaf.array_element_size;
	item->leaf.on_item_changed_callback(item, value, item->leaf.value_titles[item->leaf.hot_index], debuginator->app_user_data);
}

void debuginator_move_sibling_previous(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator->hot_item;

	if (!hot_item->is_folder && hot_item->leaf.is_active) {
		if (--hot_item->leaf.hot_index < 0) {
			hot_item->leaf.hot_index = hot_item->leaf.num_values - 1;
		}
	}
	else {
		DebuginatorItem* hot_item_new = debuginator->hot_item;
		DebuginatorItem* parent_child = debuginator__first_visible_child(hot_item_new->parent);
		if (parent_child == hot_item_new) {
			while (parent_child) {
				hot_item_new = parent_child;
				parent_child = debuginator__next_visible_sibling(parent_child);
			}
		}
		else {
			while (parent_child) {
				if (debuginator__next_visible_sibling(parent_child) == hot_item_new) {
					hot_item_new = parent_child;
					break;
				}

				parent_child = debuginator__next_visible_sibling(parent_child);
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
		if (debuginator__next_visible_sibling(hot_item) != NULL) {
			hot_item_new = debuginator__next_visible_sibling(hot_item);
		}
		else {
			hot_item_new = debuginator__first_visible_child(hot_item->parent);
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
	else if (debuginator__next_visible_sibling(hot_item)) {
		hot_item_new = debuginator__next_visible_sibling(hot_item);
	}
	else {
		DebuginatorItem* parent = hot_item->parent;
		while (parent != NULL) {
			if (debuginator__next_visible_sibling(parent) != NULL) {
				hot_item_new = debuginator__next_visible_sibling(parent);
				break;
			}

			parent = debuginator__next_visible_sibling(parent);
		}

		if (parent == NULL) {
			hot_item_new = debuginator__first_visible_child(debuginator->root);
		}
	}

	if (hot_item != hot_item_new) {
		hot_item_new->parent->folder.hot_child = hot_item_new;
		debuginator->hot_item = hot_item_new;
	}
}

void debuginator_move_to_next_leaf(TheDebuginator* debuginator, bool long_move) {
	DebuginatorItem* hot_item = debuginator->hot_item;
	if (!hot_item->is_folder && hot_item->leaf.is_active) {
		int steps = 1;
		if (long_move && hot_item->leaf.num_values > 5) {
			steps = hot_item->leaf.num_values / 5;
		}

		hot_item->leaf.hot_index += steps;
		if (hot_item->leaf.hot_index >= hot_item->leaf.num_values) {
			hot_item->leaf.hot_index = 0;
		}

		return;
	}

	DebuginatorItem* hot_item_new = debuginator__next_item(hot_item);
	if (long_move && hot_item_new != NULL) {
		while (hot_item_new != NULL && hot_item_new->parent == hot_item->parent && (hot_item_new->is_folder || hot_item_new->leaf.active_index == hot_item_new->leaf.default_index)) {
			hot_item_new = debuginator__next_item(hot_item_new);
		}

		if (hot_item_new == NULL) {
			hot_item_new = debuginator__find_last_leaf(debuginator->root);
		}
	}

	if (hot_item_new == NULL) {
		hot_item_new = debuginator__find_first_leaf(debuginator->root);
	}
	
	hot_item_new->parent->folder.hot_child = hot_item_new;
	debuginator->hot_item = hot_item_new;
}


void debuginator_move_to_prev_leaf(TheDebuginator* debuginator, bool long_move) {
	DebuginatorItem* hot_item = debuginator->hot_item;
	if (!hot_item->is_folder && hot_item->leaf.is_active) {
		int steps = 1;
		if (long_move && hot_item->leaf.num_values > 5) {
			steps = hot_item->leaf.num_values / 5;
		}

		hot_item->leaf.hot_index -= steps;
		if (hot_item->leaf.hot_index < 0) {
			hot_item->leaf.hot_index = hot_item->leaf.num_values - 1;
		}

		return;
	}

	DebuginatorItem* hot_item_new = debuginator__prev_item(hot_item);
	if (long_move && hot_item_new != NULL) {
		while (hot_item_new != NULL && hot_item_new->parent == hot_item->parent && (hot_item_new->is_folder || hot_item_new->leaf.active_index == hot_item_new->leaf.default_index)) {
			hot_item_new = debuginator__prev_item(hot_item_new);
		}

		if (hot_item_new == NULL) {
			hot_item_new = debuginator__find_first_leaf(debuginator->root);
		}
	}
	if (hot_item_new == NULL) {
		hot_item_new = debuginator__find_last_leaf(debuginator->root);
	}
	
	hot_item_new->parent->folder.hot_child = hot_item_new;
	debuginator->hot_item = hot_item_new;
}

void debuginator_move_to_child(TheDebuginator* debuginator, bool toggle_and_activate) {
	DebuginatorItem* hot_item = debuginator->hot_item;
	DebuginatorItem* hot_item_new = debuginator->hot_item;

	if (!hot_item->is_folder) {
		if (toggle_and_activate) {
			if (++hot_item->leaf.hot_index == hot_item->leaf.num_values) {
				hot_item->leaf.hot_index = 0;
			}
			debuginator_activate(debuginator, debuginator->hot_item);
		}
		else if (hot_item->leaf.is_active) {
			debuginator_activate(debuginator, debuginator->hot_item);
		}
		else {
			hot_item->leaf.is_active = true;
			debuginator__set_total_height(hot_item, (float)30 * (hot_item->leaf.num_values) + 30); // for description, HACK! :(
		}
	}
	else {
		if (hot_item->folder.hot_child != NULL) {
			hot_item_new = hot_item->folder.hot_child;
		}
		else if (debuginator__first_visible_child(hot_item) != NULL) {
			hot_item_new = debuginator__first_visible_child(hot_item);
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
		debuginator__set_total_height(hot_item, 30);
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
		debuginator_move_to_child(debuginator, false);
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

void debuginator_copy_1byte(DebuginatorItem* item, void* value, const char* value_title, void* app_userdata) {
	(void)value_title;
	(void)app_userdata;
	memcpy(item->user_data, value, 1);
}

DebuginatorItem* debuginator_create_bool_item(TheDebuginator* debuginator, const char* path, const char* description, void* user_data) {
	bool value_before_creation = *(bool*)user_data;
	static bool bool_values[2] = { false, true };
	static const char* bool_titles[2] = { "False", "True" };
	DEBUGINATOR_assert(sizeof(bool_values[0]) == 1);
	DebuginatorItem* item = debuginator_create_array_item(debuginator, NULL, path,
		description, debuginator_copy_1byte, user_data,
		bool_titles, bool_values, 2, sizeof(bool_values[0]));
	item->leaf.edit_type = DEBUGINATOR_EditTypeBoolean;

	if (value_before_creation == true) {
		item->leaf.default_index = 1;
		item->leaf.hot_index = 1;
		item->leaf.active_index = 1;
	}

	return item;
}

void debuginator_activate_preset(DebuginatorItem* item, void* value, const char* value_title, void* app_userdata) {
	(void)value_title;
	(void)app_userdata;
	(void)value;
	(void)item;
	const char** preset_value_titles = (const char**)item->leaf.values;
	TheDebuginator* debuginator = (TheDebuginator*)item->user_data;
	for (int i = 0; i < item->leaf.num_values; i++) {
		const char* path = item->leaf.value_titles[i];
		DebuginatorItem* item_to_activate = debuginator_get_item(debuginator, NULL, path, false);
		if (item_to_activate == NULL) {
			continue;
		}

		const char* preset_value_title = preset_value_titles[i];
		for (int value_i = 0; value_i < item_to_activate->leaf.num_values; value_i++) {
			if (strcmp(item_to_activate->leaf.value_titles[value_i], preset_value_title) == 0) {
				item_to_activate->leaf.hot_index = value_i;
				debuginator_activate(debuginator, item_to_activate);
				break;
			}
		}
	}

	item->leaf.active_index = 0;
}

DebuginatorItem* debuginator_create_preset_item(TheDebuginator* debuginator, const char* path, const char** paths, const char** value_titles, int** value_indices, int num_paths) {
	(void)value_indices; // TODO
	
	char description[1024] = { 0 };
	DEBUGINATOR_strcpy_s(description, 1000, "Preset: \n");
	char* description_end = description + DEBUGINATOR_strlen(description);
	const char** path_part = paths;
	const char** value_title_part = value_titles;
	for (int i = 0; i < num_paths; i++) {
		DEBUGINATOR_strcpy_s(description_end, sizeof(description) - (description_end - description), *path_part);
		description_end += DEBUGINATOR_strlen(*path_part);
		DEBUGINATOR_strcpy_s(description_end, sizeof(description) - (description_end - description), " => ");
		description_end += 4;

		DEBUGINATOR_strcpy_s(description_end, sizeof(description) - (description_end - description), *value_title_part);
		description_end += DEBUGINATOR_strlen(*value_title_part);
		DEBUGINATOR_strcpy_s(description_end, sizeof(description) - (description_end - description), "\n");
		description_end += 1;

		++path_part;
		++value_title_part;
	}

	DEBUGINATOR_assert(description_end < description + sizeof(description));

	DebuginatorItem* item = debuginator_create_array_item(debuginator, NULL, path,
		description, debuginator_activate_preset, debuginator,
		paths, (void*)value_titles, num_paths, 0);

	item->leaf.edit_type = DEBUGINATOR_EditTypePreset;

	return item;
}

#endif // DEBUGINATOR_IMPLEMENTATION
#endif // INCLUDE_THE_DEBUGINATOR_H


/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
