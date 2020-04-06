// clang-format off

/*
the_debuginator.h - v0.01 - public domain - Anders Elfgren @srekel, 2017

# THE DEBUGINATOR

A super sweet hierarchical debug menu intended for games.

See github for latest version: https://github.com/Srekel/the-debuginator

## Usage

In *ONE* source file, put:

```C
#define DEBUGINATOR_IMPLEMENTATION

// Define any of these if you wish to override them.
// (There are more. Find them in the beginning of the code.)
#define DEBUGINATOR_assert
#define DEBUGINATOR_memcpy
#define DEBUGINATOR_fabs

#include "the_debuginator.h"
```

Other source files should just include the_debuginator.h

## Notes

See the accompanying SDL demo project for references on how to use it.
Or the documentation on the github page.

## License

Basically Public Domain / MIT.
See end of file for license information.

*/

#ifndef INCLUDE_THE_DEBUGINATOR_H
#define INCLUDE_THE_DEBUGINATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DEBUGINATOR_ENABLE_WARNINGS
#ifdef _MSC_VER
#pragma warning( push, 0 )
// member padding, nameless struct/union
#pragma warning( disable: 4820 4201)
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"

// cast from 'char *' to 'DebuginatorBlockAllocator **'
// increases required alignment from 1 to 8 [-Werror,-Wcast-align]
// Seems *slighly* scary but should be fine.
#pragma clang diagnostic ignored "-Wcast-align"

// if (mouse_over || debuginator->hot_item == item && item->leaf.hot_index == i) {
#pragma clang diagnostic ignored "-Wlogical-op-parentheses"

// (void)debuginator, item, position;
#pragma clang diagnostic ignored "-Wunused-value"

// (void)debuginator, item, position;
#pragma clang diagnostic ignored "-Wcomma"

// block_address *= capacity;
#pragma clang diagnostic ignored "-Wsign-conversion"

// DEBUGINATOR_assert(path_indices[current_path_index + 1] < sizeof(current_full_path));
#pragma clang diagnostic ignored "-Wsign-compare"

// filter_color.a = alpha * DEBUGINATOR_sin(debuginator->draw_timer) < 0.5 ? 220u : 50u;
#pragma clang diagnostic ignored "-Wdouble-promotion"

#endif
#endif // DEBUGINATOR_ENABLE_WARNINGS

typedef struct DebuginatorItem DebuginatorItem;

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

// TODO: Do something better with this
typedef struct DebuginatorFont {
	void* userdata;
	int size;
	bool bold;
	bool italic;
} DebuginatorFont;

typedef struct DebuginatorSortedItem {
	DebuginatorItem* item;
	int score;
	DebuginatorSortedItem* prev;
	DebuginatorSortedItem* next;
} DebuginatorSortedItem;

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
	DEBUGINATOR_LineHighlightMouse,
	DEBUGINATOR_ItemValueDefault,
	DEBUGINATOR_ItemValueOverridden,
	DEBUGINATOR_ItemValueHot,
	DEBUGINATOR_ItemEditorOff,
	DEBUGINATOR_ItemEditorOn,
	DEBUGINATOR_ItemEditorBackground,
	DEBUGINATOR_ItemEditorForeground,
	DEBUGINATOR_NumDrawTypes
} DebuginatorDrawTypes;

typedef enum DebuginatorExpand {
	DEBUGINATOR_Collapse = 0,
	DEBUGINATOR_Expand,
	DEBUGINATOR_Toggle
} DebuginatorExpand;

typedef struct DebuginatorTheme {
	DebuginatorColor colors[DEBUGINATOR_NumDrawTypes];
	DebuginatorFont fonts[DEBUGINATOR_NumDrawTypes];
} DebuginatorTheme;

typedef struct DebuginatorItem DebuginatorItem;
typedef struct TheDebuginator TheDebuginator;
typedef struct TheDebuginatorConfig TheDebuginatorConfig;

typedef struct DebuginatorImageHandle {
	union {
		const char* str_value;
		const void* ptr_value;
		unsigned long long ull_value;
		signed long long sll_value;
	} h;
} DebuginatorImageHandle;

typedef void (*DebuginatorDrawTextCallback)
	(const char* text, DebuginatorVector2* position, DebuginatorColor* color, DebuginatorFont* font, void* userdata);
typedef void (*DebuginatorDrawRectCallback)
	(DebuginatorVector2* position, DebuginatorVector2* size, DebuginatorColor* color, void* userdata);
typedef void (*DebuginatorDrawImageCallback)
	(DebuginatorVector2* position, DebuginatorVector2* size, DebuginatorImageHandle handle, void* userdata);

// Of note: New line characters should be at the beginning of any row rather than at the end of them.
typedef void(*DebuginatorWordWrapCallback)
	(const char* text, DebuginatorFont font, float max_width, int* row_count, int* row_lengths, int row_lengths_buffer_size, void* app_userdata);
typedef DebuginatorVector2 (*DebuginatorTextSizeCallback)
	(const char* text, DebuginatorFont* font, void* userdata);
typedef void (*DebuginatorOnOpenChangedCallback)
	(bool opened, bool done, void* app_userdata);

typedef void(*DebuginatorOnItemChangedCallback)(DebuginatorItem* item, void* value, const char* value_title, void* app_userdata);
typedef bool(*DebuginatorSaveItemCallback)(const char* key, const char* value, void* userdata);

typedef enum DebuginatorItemEditorDataType {
	DEBUGINATOR_EditTypeArray, // The default
	DEBUGINATOR_EditTypeArrayExpand, // When you want it to expand rather than toggle
	DEBUGINATOR_EditTypeActionArray, // For items with direct actions and no state
	DEBUGINATOR_EditTypeActionArrayExpand, // When you want it to expand rather than toggle
	DEBUGINATOR_EditTypeBoolean,
	DEBUGINATOR_EditTypePreset,
	DEBUGINATOR_EditTypeColorPicker,
	DEBUGINATOR_EditTypeNumberRange,
	/*DEBUGINATOR_EditTypeUserType1,
	...
	DEBUGINATOR_EditTypeUserTypeN,*/
	DEBUGINATOR_EditTypeCount = 16,
} DebuginatorItemEditorDataType;

typedef enum DebuginatorDrawMode {
	DEBUGINATOR_DrawModeHierarchy,
	DEBUGINATOR_DrawModeSortedFilter,
} DebuginatorDrawMode;

// Call to create an instance of the debuginator. Make sure the config has
// all the necessary stuff in it.
// debuginator_get_default_config is recommended but not necessary.
void debuginator_create(TheDebuginatorConfig* config, TheDebuginator* debuginator);

// Provides a decent default config for debuginator_create. NOTE Does not initialize
// EVERYTHING that's needed, you still need to set some things.
void debuginator_get_default_config(TheDebuginatorConfig* config);

// Returns true when it's started to show, returns false as soon as it's closing.
bool debuginator_is_open(TheDebuginator* debuginator);

// Starts opening or closing
void debuginator_set_open(TheDebuginator* debuginator, bool open);

void debuginator_update(TheDebuginator* debuginator, float dt);
void debuginator_draw(TheDebuginator* debuginator, float dt);

// parent is optional, can be used for a bit of performance I suppose.
// path is on the format a_parent/the_new_item
// description is optional
// on item changed callback is optional
// userdata is optional
// value_titles is optional if num_values == 0.
// values is optional if num_values == 0. Should otherwise be an array of num_values items that are value_size bytes large.
// num_values can be >= 0.
// value size can be 0 if num_values == 0
DebuginatorItem* debuginator_create_array_item(TheDebuginator* debuginator,
	DebuginatorItem* parent, const char* path, const char* description,
	DebuginatorOnItemChangedCallback on_item_changed_callback, void* user_data,
	const char** value_titles, void* values, int num_values, int value_size);

// Wraps create_array_item. user_data should point to a single byte. It'll get 1 or 0 written to it.
DebuginatorItem* debuginator_create_bool_item(TheDebuginator* debuginator, const char* path, const char* description, void* user_data);

// Like above but when you want a custom callback
DebuginatorItem* debuginator_create_bool_item_with_callback(TheDebuginator* debuginator, const char* path, const char* description, void* user_data, DebuginatorOnItemChangedCallback callback);

// Useful simple callback function for setting a small value
void debuginator_copy_1byte(DebuginatorItem* item, void* value, const char* value_title, void* app_userdata);

// Wraps create_array_item. Creates an item which, upon activation, sets the value of all items referenced to by paths, to
// have the value from the corresponding index in value_titles.
// value_indices is currently not used
// paths and value_titles need to be of length num_paths.
DebuginatorItem* debuginator_create_preset_item(TheDebuginator* debuginator, const char* path, const char** paths, const char** value_titles, int** value_indices, int num_paths);

// Wraps create_array_item. Creates an item that contains a color picker.
DebuginatorItem* debuginator_create_colorpicker_item(TheDebuginator* debuginator, const char* path, const char* description, DebuginatorOnItemChangedCallback on_item_changed_callback, void* user_data, DebuginatorColor* start_color);

DebuginatorItem* debuginator_create_numberrange_float_item(TheDebuginator* debuginator, const char* path, const char* description, float* user_data, float range_min, float range_max);

// If you want to create a new empty folder.
DebuginatorItem* debuginator_create_folder_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* path);
DebuginatorItem* debuginator_new_folder_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* title, int title_length);

// Get an item by its path.
// If create_if_not_exist is a valid pointer, the item will be created if not found. The pointer will then be set to true.
DebuginatorItem* debuginator_get_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* path, bool* create_if_not_exist);

// Returns the menu root item.
DebuginatorItem* debuginator_get_root_item(TheDebuginator* debuginator);

// Returns first child of the folder item, or NULL if it oesn't have any children.
DebuginatorItem* debuginator_get_first_child(TheDebuginator* debuginator, DebuginatorItem* item);

// Returns the item's next sibling, or NULL if it doesn't have any, i.e. it's the last item under a folder.
DebuginatorItem* debuginator_get_next_sibling(TheDebuginator* debuginator, DebuginatorItem* item);

// Remove an item by reference
void debuginator_remove_item(TheDebuginator* debuginator, DebuginatorItem* item);

// Remove an item by its path
void debuginator_remove_item_by_path(TheDebuginator* debuginator, const char* path);

void debuginator_set_hot_item(TheDebuginator* debuginator, DebuginatorItem* item);
DebuginatorItem* debuginator_get_hot_item(TheDebuginator* debuginator, int* out_hot_item_index);

// Get item's parent
DebuginatorItem* debuginator_get_parent(DebuginatorItem* item);

// Get item's path. If buffer size isn't big enough, it'll be set to the needed amount.
void debuginator_get_path(TheDebuginator* debuginator, DebuginatorItem* item, char* buffer, int* buffer_size);

// Is folder
bool debuginator_is_folder(DebuginatorItem* item);


// Save the state. Each item whose current active_index is different from its default_index will be saved, by a call to the
// callback you pass in.
bool debuginator_save(TheDebuginator* debuginator, DebuginatorSaveItemCallback callback, void* userdata);

// Preload an item. If the item doesn't exist, it will be created but be hidden until properly created later.
// value_title should be the value you want it to have when finally created.
void debuginator_load_item(TheDebuginator* debuginator, const char* key, const char* value);

// Set an item's default value. If value_title is NULL, value_index will be used instead.
void debuginator_set_default_value(TheDebuginator* debuginator, const char* path, const char* value_title, int value_index); // value index is used if value_title == NULL

// For number range and color pickers etc
void debuginator_modify_value(TheDebuginator* debuginator, DebuginatorItem* item, float x_axis, float y_axis, bool snap);

// Set an item's edit type.
void debuginator_set_edit_type(TheDebuginator* debuginator, const char* path, DebuginatorItemEditorDataType edit_type);

// Set an item's callback. Only for leaves.
void debuginator_item_set_on_changed_callback_by_path(TheDebuginator* debuginator, const char* path, DebuginatorOnItemChangedCallback callback);

// Set an item's callback. Only for leaves.
void debuginator_item_set_on_changed_callback(DebuginatorItem* item, DebuginatorOnItemChangedCallback callback);

// Set an item's userdata. Only for leaves.
void debuginator_item_set_user_data_by_path(TheDebuginator* debuginator, const char* path, void* user_data);

// Set an item's userdata. Only for leaves.
void debuginator_item_set_user_data(DebuginatorItem* item, void* user_data);

// Change an item's active index to its hot index, and trigger an activation - callbacks and animations and all.
void debuginator_activate(TheDebuginator* debuginator, DebuginatorItem* item, bool animate);

// Collapsability (for folders)
bool debuginator_is_collapsed(DebuginatorItem* item);
void debuginator_set_collapsed(TheDebuginator* debuginator, DebuginatorItem* item, bool collapsed);

// Navigation functions
// Moves the hot item or hot index to the next/previous visible item or index.
// If long_move is true, it will move to the next item that either has a different parent
// or a different overridden state.
void debuginator_move_to_next_leaf(TheDebuginator* debuginator, bool long_move);
void debuginator_move_to_prev_leaf(TheDebuginator* debuginator, bool long_move);

// Expands a leaf item (makes it "active") if it's closed, activates it if it's already opened.
// If toggle_and_activate is true, it will increase the hot index once and then activate,
// even if it's not expanded. If the item's edit type's toggle_by_default is true,
// this behaviour is inverted.
void debuginator_move_to_child(TheDebuginator* debuginator, bool toggle_and_activate);

// Closes an expanded item.
void debuginator_move_to_parent(TheDebuginator* debuginator);

void debuginator_move_sibling_next(TheDebuginator* debuginator);
void debuginator_move_sibling_previous(TheDebuginator* debuginator);
void debuginator_move_to_root(TheDebuginator* debuginator);

// Filter (search) functionality
bool debuginator_is_filtering_enabled(TheDebuginator* debuginator);
void debuginator_set_filtering_enabled(TheDebuginator* debuginator, bool enabled);
const char* debuginator_get_filter(TheDebuginator* debuginator);
void debuginator_set_filter(TheDebuginator* debuginator, const char* wanted_filter);
void debuginator_update_filter(TheDebuginator* debuginator, const char* wanted_filter);

// Mouse / Touch API
void debuginator_apply_scroll(TheDebuginator* debuginator, int distance);
void debuginator_reset_scrolling(TheDebuginator* debuginator);
void debuginator_set_mouse_cursor_pos(TheDebuginator* debuginator, DebuginatorVector2* mouse_cursor_pos);
void debuginator_activate_item_at_mouse_cursor(TheDebuginator* debuginator);
void debuginator_expand_item_at_mouse_cursor(TheDebuginator* debuginator, DebuginatorExpand expand);
DebuginatorItem* debuginator_get_item_at_mouse_cursor(TheDebuginator* debuginator, int* out_hot_item_index);
bool debuginator_is_mouse_over(TheDebuginator* debuginator, bool* out_over_quick_draw_area);

// Hot key API
// Activate returns true if there was an item bound to that key
void debuginator_assign_hot_key(TheDebuginator* debuginator, const char* key, const char* path, int value_index, const char* optional_value_title);
void debuginator_unassign_hot_key(TheDebuginator* debuginator, const char* key);
DebuginatorItem* debuginator_get_first_assigned_hot_key_item(TheDebuginator* debuginator, const char* key);
bool debuginator_activate_hot_key(TheDebuginator* debuginator, const char* key);
void debuginator_clear_hot_keys(TheDebuginator* debuginator);

// Sets the height of all items. Default 30.
void debuginator_set_item_height(TheDebuginator* debuginator, int item_height);

// Sets width and height of The Debuginator
void debuginator_set_size(TheDebuginator* debuginator, int width, int height);

// Sets screen resolution for The Debuginator. Only used for right-aligned mode.
void debuginator_set_screen_resolution(TheDebuginator* debuginator, int width, int height);

// Not sure there'a valid use case for this but.. it doesn't exactly hurt.
int debuginator_total_height(TheDebuginator* debuginator);

// True for left, false for right. Use in conjunction with screen resolution.
void debuginator_set_left_aligned(TheDebuginator* debuginator, bool left_aligned);
bool debuginator_is_left_aligned(TheDebuginator* debuginator);
float debuginator_distance_from_edge(TheDebuginator* debuginator);

// Copies a string and returns a pointer to one that the debuginator owns and will
// free if assigned as the description. (TODO: Add for value_titles)
char* debuginator_copy_string(TheDebuginator* debuginator, const char* string, int length);

typedef struct DebuginatorFolderData {
	DebuginatorItem* first_child;
	DebuginatorItem* hot_child;
	int num_visible_children;
	bool is_collapsed;           // Note collapsed as opposed to expanded - because I want false/0 to be default
	bool is_sorted;              // Uses alphanumeric sorting
} DebuginatorFolderData;

typedef struct DebuginatorLeafData {
	// A helpful text for the user
	const char* description;
	int description_line_count;

	// The values and the UI titles
	const char** value_titles;
	void* values;
	int num_values;

	// How big each element in values is
	int array_element_size;

	// Gets called when item is activated
	DebuginatorOnItemChangedCallback on_item_changed_callback;

	// How the item is visualized and how its user interaction works.
	DebuginatorItemEditorDataType edit_type;

	// For animations.
	float draw_t;

	// The currently "hovered" value index.
	int hot_index;

	// The currently set value index. Will be set to hot_index upon activation.
	int active_index;

	// The item's default index. Used for saving, and for UI.
	int default_index;

	// If the item is expanded (opened).
	bool is_expanded;

	// -1 (DEBUGINATOR_NO_HOT_INDEX) if not assigned to hot key.
	int hot_key_index;
} DebuginatorLeafData;

typedef enum DebuginatorAnimationType {
	DEBUGINATOR_ItemActivate
} DebuginatorAnimationType;

typedef struct DebuginatorItemEditorData {
	// Draws the active value to the right of the item title.
	void(*quick_draw)(TheDebuginator* debuginator, DebuginatorItem* item_data, DebuginatorVector2* position);

	// Draws stuff under the title when the item is expanded.
	void(*expanded_draw)(TheDebuginator* debuginator, DebuginatorItem* item_data, DebuginatorVector2* position);

	// Currently not used
	void(*on_expanded)(TheDebuginator* debuginator, DebuginatorItem* item, void* userdata);

	// For controls that don't have simple list style values
	void(*modify_value)(TheDebuginator* debuginator, DebuginatorItem* item, float x_axis, float y_axis, bool snap);

	// If the item should revert to its default value after activation
	bool forget_state;

	// If the default behaviour should be to insta-activate the item rather than expand it.
	bool toggle_by_default;
} DebuginatorItemEditorData;

typedef struct DebuginatorItem {
	// The 'name' of the item.
	const char* title;

	// Gets passed in the on changed callback function
	void* user_data;

	// Intrinsic linked list to navigate
	DebuginatorItem* prev_sibling;
	DebuginatorItem* next_sibling;
	DebuginatorItem* parent;

	union {
		DebuginatorLeafData leaf;
		DebuginatorFolderData folder;
	};

	// For folders: includes own title and children.
	// For leaves: includes own title, and if expanded, description and values
	int total_height;

	// So we know to look in leaf or folder.
	bool is_folder;

	// If it's filtered out by the search
	bool is_filtered;
} DebuginatorItem;

// Used for creating an instance of TheDebuginator
// You can initialize this with good default values by calling debuginator_get_default_config.
// However, you will always need to set some fields.
typedef struct TheDebuginatorConfig {
	// Whether or not to add things like About and Help.
	bool create_default_debuginator_items;

	// Must be set. Where The Debuginator is allowed to do stuff.
	char* memory_arena;
	int memory_arena_capacity;

	// Color and font themes
	DebuginatorTheme themes[16];

	// Edit types.
	DebuginatorItemEditorData edit_types[DEBUGINATOR_EditTypeCount];

	// Gets passed to draw functions.
	void* app_user_data;

	// Must be set. Functions that will get called during the draw step.
	DebuginatorDrawTextCallback draw_text;
	DebuginatorDrawRectCallback draw_rect;
	DebuginatorWordWrapCallback word_wrap;
	DebuginatorTextSizeCallback text_size;

	// Optional. Will be called during draw.
	DebuginatorDrawImageCallback draw_image;

	// Optional. Gets called when The Debuginator is opened or closed.
	DebuginatorOnOpenChangedCallback on_opened_changed;

	// The dimensions of the "panel".
	DebuginatorVector2 size; // Might not be needed in the future

	// How much space the quick draw functions will get
	int quick_draw_size;

	// Screen resolution. Used when setting open_direction to -1.
	DebuginatorVector2 screen_resolution;

	// Set to -1 to put The Debuginator on the right side of the screen.
	// Remember to keep screen_resolution up to date.
	int open_direction;

	// If items should be sorted automatically or not
	bool sort_items;

	// Where the hot item should be, height-wise, on the screen.
	float focus_height;

	// The height of each item.
	int item_height;

	// Optional, use if you want to use a color picker editor.
	DebuginatorImageHandle colorpicker_image;
} TheDebuginatorConfig;

typedef struct NumberRangeFloatState {
	float value_max;
	float value_min;
	float value_current;
} NumberRangeFloatState;

#ifdef __cplusplus
}
#endif

#ifdef DEBUGINATOR_IMPLEMENTATION


#ifndef DEBUGINATOR_assert
#include <assert.h>
#define DEBUGINATOR_assert assert;
#endif

#ifndef DEBUGINATOR_static_assert
#include <assert.h>
#define DEBUGINATOR_static_assert assert;
#endif

#ifndef DEBUGINATOR_memcpy
#include <string.h>
#define DEBUGINATOR_memcpy memcpy
#endif

#ifndef DEBUGINATOR_memcmp
#include <string.h>
#define DEBUGINATOR_memcmp memcmp
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

#ifndef DEBUGINATOR_strcpy_s
#include <string.h>
#define DEBUGINATOR_strcpy_s strcpy_s
#endif

#ifndef DEBUGINATOR_strncpy_s
#include <string.h>
#define DEBUGINATOR_strncpy_s strncpy_s
#endif

#ifndef DEBUGINATOR_sprintf_s
#include <stdio.h>
#define DEBUGINATOR_sprintf_s sprintf_s
#endif

#ifndef DEBUGINATOR_strcmp
#include <string.h>
#define DEBUGINATOR_strcmp strcmp
#endif

#ifndef DEBUGINATOR_tolower
#include <ctype.h>
#define DEBUGINATOR_tolower tolower
#endif

#ifndef DEBUGINATOR_toupper
#include <ctype.h>
#define DEBUGINATOR_toupper toupper
#endif

#ifndef DEBUGINATOR_isalpha
#include <ctype.h>
// must override all
#define DEBUGINATOR_isalpha isalpha
#define DEBUGINATOR_isdigit isdigit
#define DEBUGINATOR_isupper isupper
#endif

#ifndef DEBUGINATOR_fabs
#include <math.h>
#define DEBUGINATOR_fabs fabs
#endif

#ifndef DEBUGINATOR_min
#define DEBUGINATOR_min(a,b) (((a)<(b))?(a):(b))
#define DEBUGINATOR_max(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef DEBUGINATOR_sin
#include <math.h>
#define DEBUGINATOR_sin sin
#endif

#ifndef DEBUGINATOR_FOLDER_COLLAPSED_STRING
#define DEBUGINATOR_FOLDER_COLLAPSED_STRING "folder_collapsed"
#endif

#ifndef DEBUGINATOR_BOOL_OVERRIDE
#ifndef __cplusplus
#include <stdbool.h>
#endif
#endif

#ifndef DEBUGINATOR_intptr
#define DEBUGINATOR_intptr uintptr_t
#endif

#ifndef DEBUGINATOR_LEFT_MARGIN
#define DEBUGINATOR_LEFT_MARGIN 10
#endif

#ifndef DEBUGINATOR_INDENT
#define DEBUGINATOR_INDENT 20
#endif

#ifndef DEBUGINATOR_FILTER_HEIGHT
#define DEBUGINATOR_FILTER_HEIGHT 50.0f
#endif

#ifndef DEBUGINATOR_SCORE_OVERRIDE
#define DEBUGINATOR_SCORE_WORD_BREAK_START 10
#define DEBUGINATOR_SCORE_WORD_BREAK_END 5
#define DEBUGINATOR_SCORE_ITEM_TITLE_MATCH 5
#endif

#ifndef DEBUGINATOR_MAX_NUM_HOT_KEYS
#define DEBUGINATOR_MAX_NUM_HOT_KEYS 128
#endif

#ifndef DEBUGINATOR_MAX_HIERARCHY_SIZE
#define DEBUGINATOR_MAX_HIERARCHY_SIZE 16
#endif

#ifndef DEBUGINATOR_MAX_PATH_LENGTH
#define DEBUGINATOR_MAX_PATH_LENGTH 256
#endif

#define DEBUGINATOR_NO_HOT_INDEX -1
#define DEBUGINATOR_CUSTOM_VALUE_STATE_COUNT -1

#ifndef DEBUGINATOR_DO_NOT_HOT_KEY_UPPERCASING
#define DEBUGINATOR_DO_HOT_KEY_UPPERCASING 1
#endif

#ifndef DEBUGINATOR_ALLOCATOR_BLOCK_SIZE
#define DEBUGINATOR_ALLOCATOR_BLOCK_SIZE 0x10000 // 64 kilobytes
#endif

#ifndef DEBUGINATOR_UNUSED
#define DEBUGINATOR_UNUSED(x) ((void)x)
#endif

#ifndef DEBUGINATOR_SORTED_ITEM_COUNT
#define DEBUGINATOR_SORTED_ITEM_COUNT 4
#endif

static float debuginator__ceil(float v) {
	if ((int)v == v) {
		return v;
	}
	return (float)((int)v) + 1;
}

static float debuginator__floor(float v) {
	if ((int)v == v) {
		return v;
	}
	return (float)((int)v);
}

typedef struct DebuginatorBlockAllocator DebuginatorBlockAllocator;

typedef struct DebuginatorBlockAllocatorStaticData {
	char* arena_end;
	int arena_capacity;
	int block_capacity;
	char* next_free_block;
} DebuginatorBlockAllocatorStaticData;

typedef struct DebuginatorBlockAllocator {
	DebuginatorBlockAllocatorStaticData* data;
	int element_size;
	char* current_block;
	int current_block_size;
	char* next_free_slot;
	int stat_total_used;
	int stat_num_allocations;
	int stat_num_freed;
	int stat_num_blocks;
	int stat_wasted_block_space;
} DebuginatorBlockAllocator;

static void debuginator__block_allocator_init(DebuginatorBlockAllocator* allocator, int element_size, DebuginatorBlockAllocatorStaticData* data) {
	DEBUGINATOR_memset(allocator, 0, sizeof(*allocator));
	allocator->data = data;
	allocator->element_size = element_size;
	allocator->current_block = data->next_free_block;
	// We're throwing some memory away here to not have to check if the current block is NULL for every allocate.
	allocator->current_block_size = sizeof(DebuginatorBlockAllocator*); // Make room for allocator ptr at start of block
	data->next_free_block += data->block_capacity;
	DEBUGINATOR_assert(allocator->data->arena_end > allocator->data->next_free_block);
	*((DebuginatorBlockAllocator**)(void*)allocator->current_block) = allocator;
	allocator->stat_wasted_block_space += sizeof(DebuginatorBlockAllocator*);
	allocator->stat_num_blocks++;
}

static void* debuginator__block_allocate(DebuginatorBlockAllocator* allocator, int num_bytes) {
	(void)num_bytes;
	if (allocator->data->block_capacity - allocator->current_block_size < allocator->element_size) {
		if (allocator->data->arena_end <= allocator->data->next_free_block) {
			return NULL;
		}

		allocator->stat_wasted_block_space += allocator->data->block_capacity - allocator->current_block_size;
		allocator->current_block_size = sizeof(DebuginatorBlockAllocator*); // Make room for allocator ptr at start of block
		allocator->current_block = allocator->data->next_free_block;
		allocator->data->next_free_block += allocator->data->block_capacity;
		DEBUGINATOR_assert(allocator->data->arena_end >= allocator->data->next_free_block);
		*((DebuginatorBlockAllocator**)allocator->current_block) = allocator;
		allocator->stat_wasted_block_space += sizeof(DebuginatorBlockAllocator*);
		allocator->stat_num_blocks++;
	}

	allocator->stat_num_allocations++;
	allocator->stat_total_used += allocator->element_size;
	allocator->stat_wasted_block_space += allocator->element_size - num_bytes;

	if (allocator->next_free_slot) {
		void* result = allocator->next_free_slot;
		allocator->next_free_slot = *(char**)allocator->next_free_slot;
		allocator->stat_num_freed--;
		return result;
	}

	void* result = allocator->current_block + allocator->current_block_size;
	allocator->current_block_size += allocator->element_size;
	return result;
}

static void debuginator__block_deallocate(DebuginatorBlockAllocator* allocator, const void* ptr) {
	DEBUGINATOR_intptr* next_ptr = (DEBUGINATOR_intptr*)(DEBUGINATOR_intptr)ptr;
	if (allocator->next_free_slot == NULL) {
		*next_ptr = 0;
	}
	else {
		*next_ptr = (DEBUGINATOR_intptr)allocator->next_free_slot;
	}
	allocator->next_free_slot = (char*)(DEBUGINATOR_intptr)ptr;
	allocator->stat_total_used -= allocator->element_size;
	allocator->stat_num_freed++;
	allocator->stat_num_allocations--;
}

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
	DebuginatorItem* hot_mouse_item;
	int hot_mouse_item_index;

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
	DebuginatorDrawImageCallback draw_image;
	DebuginatorDrawRectCallback draw_rect;
	DebuginatorDrawTextCallback draw_text;
	DebuginatorWordWrapCallback word_wrap;
	DebuginatorTextSizeCallback text_size;
	DebuginatorOnOpenChangedCallback on_opened_changed;
	int item_height;

	DebuginatorVector2 size;
	DebuginatorVector2 root_position; // The fixed position where The Debuginator is when it's closed
	DebuginatorVector2 top_left; // The top-left position where The Debuginator currently is
	DebuginatorVector2 screen_resolution;
	int quick_draw_size;
	int scroll_wanted;
	int scroll_current;
	DebuginatorVector2 mouse_cursor_pos;
	char open_direction; // char so I can be lazy and use copy_1_byte.
	float focus_height;
	float current_height_offset;

	bool sort_items;

	DebuginatorAnimation animations[8];
	int animation_count;

	bool filter_enabled;
	char filter[32];
	int filter_length;
	DebuginatorSortedItem sorted_items[DEBUGINATOR_SORTED_ITEM_COUNT];
	DebuginatorSortedItem* best_sorted_item;
	DebuginatorDrawMode draw_mode;

	char* memory_arena; // char* for pointer arithmetic
	int memory_arena_capacity;
	DebuginatorBlockAllocatorStaticData allocator_data;
	DebuginatorBlockAllocator allocators[6];

	const char** loaded_settings;
	int num_loaded_settings;
	int loaded_settings_capacity;

	struct {
		const char* key;
		const char* path;
		int value_index;
	} hot_keys[DEBUGINATOR_MAX_NUM_HOT_KEYS];
	int num_hot_keys;

	bool bool_values[2];
	const char* bool_titles[2];

	DebuginatorImageHandle colorpicker_image;

} TheDebuginator;

static DebuginatorVector2 debuginator__vector2(float x, float y) {
	DebuginatorVector2 v; v.x = x; v.y = y;
	return v;
}

static DebuginatorColor debuginator__color(int r, int g, int b, int a) {
	DEBUGINATOR_assert(0 <= r && r <= 255);
	DEBUGINATOR_assert(0 <= g && g <= 255);
	DEBUGINATOR_assert(0 <= b && b <= 255);
	DEBUGINATOR_assert(0 <= a && a <= 255);
	DebuginatorColor c;
	c.r = (unsigned char)r; c.g = (unsigned char)g;
	c.b = (unsigned char)b; c.a = (unsigned char)a;
	return c;
}

static float debuginator__ease_out(float t, float start_value, float change, float duration) {
	t /= duration;
	return -change * t * (t - 2) + start_value;
}

static float debuginator__lerp(float a, float b, float t) {
	return a * (1 - t) + b * t;
}

static void debuginator__quick_draw_default(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	if (item->leaf.num_values > 0) {
		DebuginatorVector2 pos = debuginator__vector2(debuginator->top_left.x + debuginator->size.x - debuginator->quick_draw_size, position->y + debuginator->item_height / 2.0f);

		bool is_overriden = item->leaf.active_index != item->leaf.default_index;
		unsigned default_color_index = is_overriden ? DEBUGINATOR_ItemTitleOverridden : DEBUGINATOR_ItemTitle;
		debuginator->draw_text(item->leaf.value_titles[item->leaf.active_index], &pos, &debuginator->theme.colors[default_color_index], &debuginator->theme.fonts[DEBUGINATOR_ItemTitle], debuginator->app_user_data);
	}
}

static void debuginator__expanded_draw_default(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	for (int i = 0; i < item->leaf.num_values; i++) {
		position->y += debuginator->item_height;

		bool mouse_over =
			debuginator->top_left.x <= debuginator->mouse_cursor_pos.x && debuginator->mouse_cursor_pos.x < debuginator->top_left.x + debuginator->size.x - debuginator->quick_draw_size &&
			position->y <= debuginator->mouse_cursor_pos.y && debuginator->mouse_cursor_pos.y < position->y + debuginator->item_height;

		if (mouse_over || debuginator->hot_item == item && item->leaf.hot_index == i) {
			debuginator->hot_mouse_item_index = mouse_over ? i : debuginator->hot_mouse_item_index;
			DebuginatorVector2 pos = debuginator__vector2(debuginator->top_left.x, position->y);
			DebuginatorVector2 size = debuginator__vector2(debuginator->size.x, (float)debuginator->item_height);
			debuginator->draw_rect(&pos, &size, &debuginator->theme.colors[mouse_over ? DEBUGINATOR_LineHighlightMouse : DEBUGINATOR_LineHighlight], debuginator->app_user_data);
		}

		bool forget_state = debuginator->edit_types[item->leaf.edit_type].forget_state;
		const char* value_title = item->leaf.value_titles[i];
		bool value_hot = i == item->leaf.hot_index || mouse_over;
		bool value_overridden = i == item->leaf.active_index && !forget_state;
		unsigned value_color_index = value_hot ? DEBUGINATOR_ItemValueHot : (value_overridden ? DEBUGINATOR_ItemTitleOverridden : DEBUGINATOR_ItemValueDefault);
		DebuginatorVector2 text_pos = debuginator__vector2(position->x, position->y + debuginator->item_height / 2.0f);
		debuginator->draw_text(value_title, &text_pos, &debuginator->theme.colors[value_color_index], &debuginator->theme.fonts[value_hot ? DEBUGINATOR_ItemTitleHot : DEBUGINATOR_ItemTitle], debuginator->app_user_data);

		if (item->leaf.hot_key_index != DEBUGINATOR_NO_HOT_INDEX) {
			const char* key = debuginator->hot_keys[item->leaf.hot_key_index].key;
			int value_index = debuginator->hot_keys[item->leaf.hot_key_index].value_index;
			if (value_index == i) {
				DebuginatorVector2 key_rect_pos = debuginator__vector2(debuginator->top_left.x + debuginator->size.x, position->y);
				DebuginatorVector2 key_rect_size = debuginator__vector2((float)debuginator->item_height, (float)debuginator->item_height);
				debuginator->draw_rect(&key_rect_pos, &key_rect_size, &debuginator->theme.colors[DEBUGINATOR_Background], debuginator->app_user_data);

				DebuginatorVector2 key_text_pos = debuginator__vector2(debuginator->top_left.x + debuginator->size.x + 10, position->y + debuginator->item_height / 2.0f);
				debuginator->draw_text(key, &key_text_pos, &debuginator->theme.colors[DEBUGINATOR_ItemTitleOverridden], &debuginator->theme.fonts[DEBUGINATOR_ItemTitle], debuginator->app_user_data);
			}
		}
	}
}

static void debuginator__quick_draw_boolean(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	DebuginatorVector2 size = debuginator__vector2(50, 20);
	DebuginatorVector2 pos = debuginator__vector2(debuginator->top_left.x + debuginator->size.x - debuginator->quick_draw_size, position->y + debuginator->item_height / 2.0f - size.y / 2.0f);
	DebuginatorColor background = debuginator__color(0, 0, 0, 100);
	debuginator->draw_rect(&pos, &size, &background, debuginator->app_user_data);

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
	unsigned char alpha = item->leaf.active_index == item->leaf.default_index ? 100u : 255u;
	DebuginatorColor slider = item->leaf.active_index == 0 ? debuginator->theme.colors[DEBUGINATOR_ItemEditorOff] : debuginator->theme.colors[DEBUGINATOR_ItemEditorOn];
	slider.a = alpha;

	debuginator->draw_rect(&slider_pos, &size, &slider, debuginator->app_user_data);
}

static void debuginator__expanded_draw_boolean(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	for (int i = 0; i < item->leaf.num_values; i++) {
		position->y += debuginator->item_height;

		bool mouse_over =
			debuginator->top_left.x <= debuginator->mouse_cursor_pos.x && debuginator->mouse_cursor_pos.x < debuginator->top_left.x + debuginator->size.x - debuginator->quick_draw_size &&
			position->y <= debuginator->mouse_cursor_pos.y && debuginator->mouse_cursor_pos.y < position->y + debuginator->item_height;

		if (mouse_over || debuginator->hot_item == item && item->leaf.hot_index == i) {
			DebuginatorVector2 pos = debuginator__vector2(debuginator->top_left.x, position->y);
			DebuginatorVector2 size = debuginator__vector2(debuginator->size.x, (float)debuginator->item_height);
			debuginator->draw_rect(&pos, &size, &debuginator->theme.colors[mouse_over ? DEBUGINATOR_LineHighlightMouse : DEBUGINATOR_LineHighlight], debuginator->app_user_data);
		}

		const char* value_title = item->leaf.value_titles[i];
		bool value_hot = i == item->leaf.hot_index;
		bool value_overridden = i == item->leaf.active_index;
		unsigned value_color_index = value_hot ? DEBUGINATOR_ItemValueHot : (value_overridden ? DEBUGINATOR_ItemTitleOverridden : DEBUGINATOR_ItemValueDefault);
		DebuginatorVector2 text_pos = debuginator__vector2(position->x, position->y + debuginator->item_height / 2.0f);
		debuginator->draw_text(value_title, &text_pos, &debuginator->theme.colors[value_color_index], &debuginator->theme.fonts[value_hot ? DEBUGINATOR_ItemTitleHot : DEBUGINATOR_ItemTitle], debuginator->app_user_data);

		if (item->leaf.hot_key_index != DEBUGINATOR_NO_HOT_INDEX) {
			const char* key = debuginator->hot_keys[item->leaf.hot_key_index].key;
			int value_index = debuginator->hot_keys[item->leaf.hot_key_index].value_index;
			if (value_index == i) {
				DebuginatorVector2 key_rect_pos = debuginator__vector2(debuginator->top_left.x + debuginator->size.x, position->y);
				DebuginatorVector2 key_rect_size = debuginator__vector2((float)debuginator->item_height, (float)debuginator->item_height);
				debuginator->draw_rect(&key_rect_pos, &key_rect_size, &debuginator->theme.colors[DEBUGINATOR_Background], debuginator->app_user_data);

				DebuginatorVector2 key_text_pos = debuginator__vector2(debuginator->top_left.x + debuginator->size.x + 10, position->y + debuginator->item_height / 2.0f);
				debuginator->draw_text(key, &key_text_pos, &debuginator->theme.colors[DEBUGINATOR_ItemTitleOverridden], &debuginator->theme.fonts[DEBUGINATOR_ItemTitle], debuginator->app_user_data);
			}
		}
	}
}

static void debuginator__quick_draw_preset(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	(void)debuginator, item, position;
}

static void debuginator__expanded_draw_preset(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	(void)debuginator, item, position;
	// We don't really want to draw these I think, better to show in the description.

	// for (int i = 0; i < item->leaf.num_values; i++) {
	// 	position->y += debuginator->item_height;
	// 	const char* value_title = item->leaf.value_titles[i];
	// 	bool value_hot = i == item->leaf.hot_index;
	// 	bool value_overridden = i == item->leaf.active_index;
	// 	unsigned value_color_index = value_hot ? DEBUGINATOR_ItemValueHot : (value_overridden ? DEBUGINATOR_ItemTitleOverridden : DEBUGINATOR_ItemValueDefault);
	// 	debuginator->draw_text(value_title, position, &debuginator->theme.colors[value_color_index], &debuginator->theme.fonts[value_hot ? DEBUGINATOR_ItemTitleHot : DEBUGINATOR_ItemTitle], debuginator->app_user_data);
	// }
}

static void debuginator__quick_draw_colorpicker(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	(void)debuginator, item, position;

	float anim_t = (float)(DEBUGINATOR_sin(debuginator->draw_timer * 0.2) + 1) * 0.5f;
	float margin = 3.0f;
	float square_side_size = debuginator->item_height - margin * 2.0f;
	DebuginatorVector2 square_pos = debuginator__vector2(debuginator->top_left.x + debuginator->size.x - debuginator->quick_draw_size, position->y);
	square_pos.x = square_pos.x;
	square_pos.y += margin;
	DebuginatorVector2 square_size;
	square_size.x = square_side_size;
	square_size.y = debuginator__floor(square_side_size * anim_t);

	DebuginatorColor square_color = debuginator__color(255, 255, 255, 255);
	debuginator->draw_rect(&square_pos, &square_size, &square_color, debuginator->app_user_data);

	square_pos.y += square_size.y;
	square_size.y = debuginator__ceil(square_side_size * (1 - anim_t));
	square_color = debuginator__color(0, 0, 0, 255);
	debuginator->draw_rect(&square_pos, &square_size, &square_color, debuginator->app_user_data);

	square_pos.x += margin;
	square_pos.y = position->y + 2.0f * margin;
	square_size.x -= margin * 2.0f;
	square_size.y = square_side_size - margin * 2.0f;
	square_color = *(DebuginatorColor*)item->leaf.values;
	debuginator->draw_rect(&square_pos, &square_size, &square_color, debuginator->app_user_data);
}

static void debuginator__expanded_draw_colorpicker(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	DEBUGINATOR_UNUSED(item);
	DebuginatorVector2 image_size = debuginator__vector2(100, 100);
	debuginator->draw_image(position, &image_size, debuginator->colorpicker_image, debuginator->app_user_data);
}

// static void debuginator__on_expanded_colorpicker(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
// 	DEBUGINATOR_UNUSED(item);
// 	DebuginatorVector2 image_size = debuginator__vector2(100, 100);
// 	debuginator->draw_image(position, &image_size, debuginator->colorpicker_image, debuginator->app_user_data);
// }

static void debuginator__quick_draw_numberrange(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	(void)debuginator, item, position;

	// float* state = (float*)item->leaf.values;

	// float default_value = state[2];
	float current_value = *(float*)item->user_data;
	char value_str[64];
	DEBUGINATOR_sprintf_s(value_str, sizeof(value_str), "%.4f", current_value);

	DebuginatorVector2 pos = debuginator__vector2(debuginator->top_left.x + debuginator->size.x - debuginator->quick_draw_size, position->y + debuginator->item_height / 2.0f);
	DebuginatorFont* font = &debuginator->theme.fonts[DEBUGINATOR_ItemValueOverridden];
	debuginator->draw_text(value_str, &pos, &debuginator->theme.colors[DEBUGINATOR_ItemTitleActive], font, debuginator->app_user_data);
}

static void debuginator__expanded_draw_numberrange(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	(void)debuginator, item, position;
	// DebuginatorVector2 image_size = debuginator__vector2(100, 100);
	// debuginator->draw_image(position, &image_size, debuginator->colorpicker_image, debuginator->app_user_data);
}

static void debuginator__modify_value_numberrange(TheDebuginator* debuginator, DebuginatorItem* item, float x_axis, float y_axis, bool snap) {
	(void)debuginator, item, snap, x_axis;
	float* state = (float*)item->leaf.values;
	float min_value = state[0];
	float max_value = state[1];
	float active_value = state[3];
	float current_percent = (active_value - min_value ) / (max_value - min_value);
	float new_percent = DEBUGINATOR_max(0, DEBUGINATOR_min(1, current_percent + y_axis));
	float new_value = min_value + (max_value - min_value) * new_percent;
	state[3] = new_value;
	debuginator_activate(debuginator, item, false);
}

// static void debuginator__activate_numberrange(TheDebuginator* debuginator, DebuginatorItem* item) {
// 	(void)debuginator, item;
// 	float* state = (float*)item->leaf.values;
// 	float min_value = state[0];
// 	float max_value = state[1];
// 	float active_value = state[3];
// 	float current_percent = (active_value - min_value ) / (max_value - min_value);
// 	float new_percent = DEBUGINATOR_max(0, DEBUGINATOR_min(1, current_percent + y_axis));
// 	float new_value = min_value + (max_value - min_value) * new_percent;
// 	state[3] = new_value;
// 	debuginator_activate(debuginator, item, false);
// }

// static void debuginator__on_expanded_colorpicker(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
// 	DebuginatorVector2 image_size = debuginator__vector2(100, 100);
// 	debuginator->draw_image(position, &image_size, debuginator->colorpicker_image, debuginator->app_user_data);
// }

static void* debuginator__allocate(TheDebuginator* debuginator, int bytes/*, const void* origin*/) {
	for (int i = 0; i < 6; i++) {
		if (bytes <= debuginator->allocators[i].element_size) {
			void* result = debuginator__block_allocate(&debuginator->allocators[i], bytes);

			// There's no point in gracefully handling running out of memory, just assert.
			DEBUGINATOR_assert(result != NULL);
			DEBUGINATOR_assert(bytes > 0);

			DEBUGINATOR_memset(result, 0, (unsigned int)bytes);
			return result;
		}
	}

	DEBUGINATOR_assert(false);
	return NULL;
}

static void debuginator__deallocate(TheDebuginator* debuginator, const void* void_ptr) {
	// We remove the const part and that's fine, if it's our string we can do whatever we want with it,
	// and if not, then we don't do anything (see right below). It makes the API a bit nicer.
	char* ptr = (char*)(DEBUGINATOR_intptr)void_ptr;
	if (!(debuginator->memory_arena <= ptr && ptr < debuginator->memory_arena + debuginator->memory_arena_capacity)) {
		// Yes, to simplify other code we do this check here. That way we can always attempt to
		// deallocate a piece of memory even though we don't have ownership of it.
		return;
	}

	DEBUGINATOR_intptr block_address = (DEBUGINATOR_intptr)ptr;
	int capacity = debuginator->allocator_data.block_capacity;
	block_address /= capacity;
	block_address *= capacity;
	//char* block_ptr = (char*)block_address;
	DebuginatorBlockAllocator* allocator = *(DebuginatorBlockAllocator**)block_address;
	DEBUGINATOR_memset(ptr, 0xcd, (unsigned int)allocator->element_size);
	debuginator__block_deallocate(allocator, void_ptr);
}

char* debuginator_copy_string(TheDebuginator* debuginator, const char* string, int length) {
	if (length == 0) {
		length = (int)DEBUGINATOR_strlen(string);
	}

	char* memory = (char*)debuginator__allocate(debuginator, length + 1);
	DEBUGINATOR_memcpy(memory, string, (unsigned int)length);
	memory[length] = '\0';
	return memory;
}

static DebuginatorAnimation* debuginator__get_free_animation(TheDebuginator* debuginator) {
	if (debuginator->animation_count == sizeof(debuginator->animations) / sizeof(*debuginator->animations)) {
		return NULL;
	}

	DebuginatorAnimation* animation = &debuginator->animations[debuginator->animation_count++];
	DEBUGINATOR_memset(animation, 0, sizeof(*animation));
	return animation;
}

static void debuginator__set_total_height(DebuginatorItem* item, int height) {
	if (item->total_height == height) {
		return;
	}

	int diff = height - item->total_height;
	item->total_height = height;
	DEBUGINATOR_assert(item->total_height >= 0);
	if (item->parent && !item->parent->folder.is_collapsed) {
		debuginator__set_total_height(item->parent, item->parent->total_height + diff);
	}
}

static int debuginator__set_item_total_height_recursively(DebuginatorItem* item, int item_height) {
	if (item->is_folder) {
		if (item->folder.is_collapsed) {
			if (item->folder.num_visible_children > 0) {
				// If a folder doesn't have children, it shouldn't be visible in any case.
				item->total_height = item_height;
			}
			else {
				item->total_height = 0;
			}
		}
		else {
			item->total_height = 0;
			DebuginatorItem* child = item->folder.first_child;
			while (child != NULL) {
				item->total_height += debuginator__set_item_total_height_recursively(child, item_height);
				child = child->next_sibling;
			}

			if (item->total_height > 0) {
				item->total_height += item_height;
			}
		}
	}
	else {
		if (item->leaf.is_expanded) {
			item->total_height = item_height + item_height * item->leaf.description_line_count + item_height * item->leaf.num_values;
		}
		else if (!item->is_filtered) {
			item->total_height = item_height;
		}
		else {
			item->total_height = 0;
		}
	}

	return item->total_height;
}

static void debuginator__adjust_num_visible_children(DebuginatorItem* item, int diff) {
	DEBUGINATOR_assert(item->is_folder);
	DEBUGINATOR_assert(diff != 0 && item->folder.num_visible_children + diff >= 0);
	item->folder.num_visible_children += diff;
	if (item->folder.num_visible_children == 0 && item->parent != NULL) {
		// Hide us as well
		debuginator__adjust_num_visible_children(item->parent, -1);
	}
	else if (item->folder.num_visible_children == diff && item->parent != NULL) {
		debuginator__adjust_num_visible_children(item->parent, 1);
	}
}

static void debuginator__on_change_theme(DebuginatorItem* item, void* value, const char* value_title, void* app_userdata) {
	(void)app_userdata;
	(void)value_title;
	TheDebuginator* debuginator = (TheDebuginator*)item->user_data;
	debuginator->theme_index = *(int*)value;
	debuginator->theme = debuginator->themes[debuginator->theme_index];
}

static void debuginator__set_title(TheDebuginator* debuginator, DebuginatorItem* item, const char* title, int title_length) {
	if (title_length == 0) {
		title_length = (int)DEBUGINATOR_strlen(title);
	}

	item->title = debuginator_copy_string(debuginator, title, title_length);
}

static DebuginatorItem* debuginator__next_visible_sibling(DebuginatorItem* item) {
	DebuginatorItem* sibling = item->next_sibling;
	while (sibling) {
		if (sibling->is_folder) {
			if (sibling->folder.num_visible_children > 0) {
				return sibling;
			}
		}
		else if (!sibling->is_filtered) {
			return sibling;
		}

		sibling = sibling->next_sibling;
	}

	return NULL;
}

static DebuginatorItem* debuginator__prev_visible_sibling(DebuginatorItem* item) {
	DebuginatorItem* sibling = item->prev_sibling;
	while (sibling) {
		if (sibling->is_folder) {
			if (sibling->folder.num_visible_children > 0) {
				return sibling;
			}
		}
		else if (!sibling->is_filtered) {
			return sibling;
		}

		sibling = sibling->prev_sibling;
	}

	return NULL;
}

static DebuginatorItem* debuginator__first_visible_child(DebuginatorItem* item) {
	if (item->folder.first_child == NULL) {
		return NULL;
	}

	if (item->folder.is_collapsed) {
		return NULL;
	}

	if (item->folder.first_child->is_folder) {
		if (item->folder.first_child->folder.num_visible_children > 0) {
			return item->folder.first_child;
		}
	}
	else if (!item->folder.first_child->is_filtered) {
		return item->folder.first_child;
	}

	return debuginator__next_visible_sibling(item->folder.first_child);
}

static DebuginatorItem* debuginator__find_first_leaf(DebuginatorItem* item) {
	if (!item->is_folder) {
		if (!item->is_filtered) {
			return item;
		}
	}
	else if (item->folder.num_visible_children > 0 && item->parent != NULL) {
		// Checking parent is a minor hack to never return the menu root item.
		return item;
	}
	else {
		DebuginatorItem* child = debuginator__first_visible_child(item);
		DebuginatorItem* leaf = child == NULL ? NULL : debuginator__find_first_leaf(child);
		if (leaf) {
			return leaf;
		}
	}

	DebuginatorItem* sibling = debuginator__next_visible_sibling(item);
	while (sibling != NULL) {
		item = debuginator__find_first_leaf(sibling);
		if (item != NULL) {
			return item;
		}
		sibling = debuginator__next_visible_sibling(sibling);
	}

	return NULL;
}

static DebuginatorItem* debuginator__find_last_leaf(DebuginatorItem* item) {
	if (!item->is_folder) {
		if (!item->is_filtered) {
			return item;
		}
	}
	else if (item->folder.is_collapsed) {
		return item;
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

// Returns the very first visible item below item
static DebuginatorItem* debuginator__next_visible_item(DebuginatorItem* item) {
	if (item->is_folder) {
		DebuginatorItem* child = debuginator__first_visible_child(item);
		if (child != NULL) {
			return child;
		}
	}

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

// Returns the very first visible item above item
static DebuginatorItem* debuginator__prev_visible_item(DebuginatorItem* item) {
	while (item != NULL) {
		DebuginatorItem* sibling = debuginator__prev_visible_sibling(item);
		DebuginatorItem* item_new = sibling == NULL ? NULL : debuginator__find_last_leaf(sibling);
		if (item_new != NULL) {
			return item_new;
		}

		item = item->parent;
		if (item->parent == NULL) {
			return NULL;
		}

		return item;
	}

	return NULL;
}

static DebuginatorItem* debuginator__nearest_visible_item(DebuginatorItem* item) {
	DebuginatorItem* found_item = debuginator__next_visible_item(item);
	if (found_item == NULL) {
		found_item = debuginator__prev_visible_item(item);
	}

	return found_item;
}

static void debuginator__set_parent(DebuginatorItem* item, DebuginatorItem* parent) {
	if (parent == NULL)
		return;

	DEBUGINATOR_assert(item->parent == NULL || item->parent == parent);
	item->parent = parent;
	if (parent->folder.first_child == NULL) {
		parent->folder.first_child = item;
	}
	else {
		DebuginatorItem* last_sibling = parent->folder.first_child;
		while (last_sibling != NULL) {
			if (last_sibling == item) {
				// Item was already in parent
				return;
			}

			if (parent->folder.is_sorted) {
				// TODO: Alphanumeric sorting
				if (DEBUGINATOR_strcmp(last_sibling->title, item->title) > 0) {
					// Add before the existing item
					if (last_sibling->prev_sibling == NULL) {
						parent->folder.first_child = item;
					}
					else {
						last_sibling->prev_sibling->next_sibling = item;
					}

					item->next_sibling = last_sibling;
					item->prev_sibling = last_sibling->prev_sibling;
					last_sibling->prev_sibling = item;
					return;
				}
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

static void debuginator__store_item_setting(TheDebuginator* debuginator, const char* path, const char* value_title) {
	// If it already exists, we override the value
	for (int i = 0; i < debuginator->num_loaded_settings; ++i) {
		int setting_index = i * 2;
		if (DEBUGINATOR_strcmp(debuginator->loaded_settings[setting_index], path) == 0) {
			debuginator__deallocate(debuginator, debuginator->loaded_settings[setting_index + 1]);
			debuginator->loaded_settings[setting_index + 1] = debuginator_copy_string(debuginator, value_title, 0);
			return;
		}
	}

	if (debuginator->num_loaded_settings == debuginator->loaded_settings_capacity) {
		int current_size = debuginator->num_loaded_settings * 2 * (int)(sizeof(char*));
		int grow_factor = 16;
		int initial_size = sizeof(char*) * 2 * 8;
		int new_size = current_size == 0 ? initial_size : current_size * grow_factor;
		void* buffer = debuginator__allocate(debuginator, new_size);
		DEBUGINATOR_assert(buffer); // TODO: This will hit if new_size is too large.
		DEBUGINATOR_memcpy(buffer, debuginator->loaded_settings, (unsigned int)current_size);
		debuginator__deallocate(debuginator, debuginator->loaded_settings);
		debuginator->loaded_settings = (const char**)buffer;
		debuginator->loaded_settings_capacity = new_size / 2 / sizeof(void*);
	}

	int setting_index = debuginator->num_loaded_settings * 2;
	debuginator->loaded_settings[setting_index + 0] = debuginator_copy_string(debuginator, path, 0);
	debuginator->loaded_settings[setting_index + 1] = debuginator_copy_string(debuginator, value_title, 0);
	debuginator->num_loaded_settings++;
}

static const char* debuginator__get_item_setting(TheDebuginator* debuginator, const char* path) {
	for (int i = 0; i < debuginator->num_loaded_settings; ++i) {
		int setting_index = i * 2;
		if (DEBUGINATOR_strcmp(debuginator->loaded_settings[setting_index], path) == 0) {
			return debuginator->loaded_settings[setting_index + 1];
		}
	}

	return "";
}

// Note: This will allocate a string and return it. I'm ok with that since it's an internal function.
static const char* debuginator__compute_path(TheDebuginator* debuginator, DebuginatorItem* parent, const char* path, int path_length) {
	if (path_length == 0) {
		path_length = (int)DEBUGINATOR_strlen(path);
	}

	// Find first parent that is not the root
	DebuginatorItem* parents[DEBUGINATOR_MAX_HIERARCHY_SIZE];
	int num_parents = 0;
	while (parent && parent->parent) {
		parents[num_parents++] = parent;
		parent = parent->parent;
	}

	char full_path[DEBUGINATOR_MAX_PATH_LENGTH]; // TODO: Unhardcodify this.
	char* curr_path_pos = full_path;
	for (int i = num_parents - 1; i >= 0; i--) {
		int title_length = (int)DEBUGINATOR_strlen(parents[i]->title);
		DEBUGINATOR_strcpy_s(curr_path_pos, title_length + 1, parents[i]->title);
		curr_path_pos += title_length;
		if (i > 0 || path_length > 0) {
			*curr_path_pos = '/';
			++curr_path_pos;
		}
	}

	// unsigned int lol = 3;
	// DEBUGINATOR_strncpy_s(curr_path_pos, lol, path, path_length + 1);
	DEBUGINATOR_strncpy_s(curr_path_pos, (unsigned int)(sizeof(full_path) - (curr_path_pos - full_path)), path, (unsigned int)(path_length + 1));
	curr_path_pos += path_length;
	*curr_path_pos = '\0';
	++curr_path_pos;
	const char* result = debuginator_copy_string(debuginator, full_path, (int)(curr_path_pos - full_path) + 1);
	return result;
}

DebuginatorItem* debuginator_new_folder_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* title, int title_length) {
	DebuginatorItem* folder_item = (DebuginatorItem*)debuginator__allocate(debuginator, sizeof(DebuginatorItem));
	folder_item->is_folder = true;
	folder_item->folder.num_visible_children = 0;
	folder_item->folder.is_sorted = debuginator->sort_items;
	debuginator__set_title(debuginator, folder_item, title, title_length);
	debuginator__set_parent(folder_item, parent);
	debuginator__set_total_height(folder_item, debuginator->item_height);

	const char* full_path = debuginator__compute_path(debuginator, parent, title, title_length);
	const char* item_setting = debuginator__get_item_setting(debuginator, full_path);
	if (DEBUGINATOR_strcmp(item_setting, DEBUGINATOR_FOLDER_COLLAPSED_STRING) == 0) {
		folder_item->folder.is_collapsed = true;
	}
	debuginator__deallocate(debuginator, item_setting);
	debuginator__deallocate(debuginator, full_path);

	return folder_item;
}

DebuginatorItem* debuginator_create_folder_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* path) {
	bool create_if_not_exist;
	DebuginatorItem* folder_item = debuginator_get_item(debuginator, parent, path, &create_if_not_exist);
	folder_item->is_folder = true;
	folder_item->folder.is_sorted = debuginator->sort_items;
	debuginator__set_total_height(folder_item, debuginator->item_height);

	const char* full_path = debuginator__compute_path(debuginator, parent, path, 0);
	const char* item_setting = debuginator__get_item_setting(debuginator, full_path);
	if (DEBUGINATOR_strcmp(item_setting, DEBUGINATOR_FOLDER_COLLAPSED_STRING) == 0) {
		folder_item->folder.is_collapsed = true;
	}
	debuginator__deallocate(debuginator, item_setting);
	debuginator__deallocate(debuginator, full_path);

	return folder_item;
}

DebuginatorItem* debuginator_get_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* path, bool* create_if_not_exist) {
	parent = parent == NULL ? debuginator->root : parent;
	const char* temp_path = path;
	while (true) {
		DEBUGINATOR_assert(parent->is_folder); // Maybe you've added A/B/C as a leaf, then try to add A/B/C/D
		const char* next_slash = DEBUGINATOR_strchr(temp_path, '/');
		size_t path_part_length = next_slash ? next_slash - temp_path : DEBUGINATOR_strlen(temp_path);

		DebuginatorItem* current_item = NULL;
		DebuginatorItem* parent_child = parent->folder.first_child;
		while (parent_child) {
			const char* item_title = parent_child->title;
			size_t title_length = DEBUGINATOR_strlen(item_title);

			if (title_length == path_part_length && DEBUGINATOR_memcmp(parent_child->title, temp_path, title_length * sizeof(char)) == 0) {
				current_item = parent_child;
				break;
			}

			parent_child = parent_child->next_sibling;
		}

		if (current_item == NULL && create_if_not_exist == NULL) {
			// Item wasn't found and we don't want to create it, so be done.
			return NULL;
		}

		if (create_if_not_exist) {
			*create_if_not_exist = current_item == NULL;
		}

		// If current_item is set, it means the item existed.
		// If create_if_not_exist s set, we're just going to reuse the item instead of creating a new one.
		if (next_slash == NULL) {
			// Found the last part of the path
			if (current_item == NULL) {
				current_item = (DebuginatorItem*)debuginator__allocate(debuginator, sizeof(DebuginatorItem));
				debuginator__set_title(debuginator, current_item, temp_path, 0);
				debuginator__set_parent(current_item, parent);
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

DebuginatorItem* debuginator_get_root_item(TheDebuginator* debuginator) {
	return debuginator->root;
}

DebuginatorItem* debuginator_get_first_child(TheDebuginator* debuginator, DebuginatorItem* item) {
	DEBUGINATOR_UNUSED(debuginator);
	DEBUGINATOR_assert(item->is_folder);
	return item->folder.first_child;
}

DebuginatorItem* debuginator_get_next_sibling(TheDebuginator* debuginator, DebuginatorItem* item) {
	DEBUGINATOR_UNUSED(debuginator);
	return item->next_sibling;
}

DebuginatorItem* debuginator_get_parent(DebuginatorItem* item) {
	return item->parent;
}

void debuginator_get_path(TheDebuginator* debuginator, DebuginatorItem* item, char* buffer, int* buffer_size) {
	const char* full_path = debuginator__compute_path(debuginator, item, "", 0);
	int length = (int)DEBUGINATOR_strlen(full_path);
	if (length > *buffer_size) {
		*buffer_size = length;
		debuginator__deallocate(debuginator, full_path);
		return;
	}

	DEBUGINATOR_strcpy_s(buffer, *buffer_size, full_path);
	debuginator__deallocate(debuginator, full_path);
}


bool debuginator_is_folder(DebuginatorItem* item) {
	return item->is_folder;
}

DebuginatorItem* debuginator_create_array_item(TheDebuginator* debuginator,
	DebuginatorItem* parent, const char* path, const char* description,
	DebuginatorOnItemChangedCallback on_item_changed_callback, void* user_data,
	const char** value_titles, void* values, int num_values, int value_size) {

	bool create_if_not_exist;
	DebuginatorItem* item = debuginator_get_item(debuginator, parent, path, &create_if_not_exist);
	item->is_folder = false;
	item->leaf.num_values = num_values;
	item->leaf.values = values;
	item->leaf.array_element_size = value_size;
	item->leaf.value_titles = value_titles;
	item->leaf.on_item_changed_callback = on_item_changed_callback;
	item->leaf.hot_key_index = DEBUGINATOR_NO_HOT_INDEX;
	item->user_data = user_data;
	debuginator__set_total_height(item, debuginator->item_height);

	if (item->leaf.hot_index >= num_values) {
		item->leaf.hot_index = num_values - 1;
	}

	if (debuginator->hot_item == NULL && !item->is_folder) {
		item->parent->folder.hot_child = item;
		debuginator->hot_item = item;
	}

	const char* full_path = debuginator__compute_path(debuginator, parent, path, 0);
	const char* item_setting = debuginator__get_item_setting(debuginator, full_path);
	if (DEBUGINATOR_strcmp(item_setting, "") != 0) {
		for (int i = 0; i < item->leaf.num_values; i++) {
			if (DEBUGINATOR_strcmp(item->leaf.value_titles[i], item_setting) == 0) {
				item->leaf.hot_index = i;
				debuginator_activate(debuginator, item, false);
				break;
			}
		}
	}

	item->leaf.description = description == NULL ? "" : description;

	if (create_if_not_exist) {
		// Only want to update this if the item didn't already exist.
		debuginator__adjust_num_visible_children(item->parent, 1);
	}

	if (description && !item->is_folder) {
		int indent = DEBUGINATOR_LEFT_MARGIN;
		DebuginatorItem* temp_parent = item->parent;
		while (temp_parent) {
			indent += DEBUGINATOR_INDENT;
			temp_parent = temp_parent->parent;
		}

		float description_width = debuginator->size.x - 50 - indent;
		int row_lengths[32];
		int row_count = 0;
		debuginator->word_wrap(item->leaf.description, debuginator->theme.fonts[DEBUGINATOR_ItemDescription], description_width, &row_count, row_lengths, 32, debuginator->app_user_data);
		item->leaf.description_line_count = row_count;
	} else if (!item->is_folder) {
		item->leaf.description_line_count = 0;
	}

	char hot_path_key[DEBUGINATOR_MAX_PATH_LENGTH + 16] = { 0 };
	char hot_path_value[DEBUGINATOR_MAX_PATH_LENGTH + 16] = { 0 };
	DEBUGINATOR_sprintf_s(hot_path_key, sizeof(hot_path_key), "%s__HotKey_Key", full_path);
	DEBUGINATOR_sprintf_s(hot_path_value, sizeof(hot_path_value), "%s__HotKey_Value", full_path);
	const char* hot_key_key = debuginator__get_item_setting(debuginator, hot_path_key);
	if (DEBUGINATOR_strcmp(hot_key_key, "") != 0) {
		const char* hot_key_value = debuginator__get_item_setting(debuginator, hot_path_value);
		if (DEBUGINATOR_strcmp(hot_key_value, "") != 0) {
			debuginator_assign_hot_key(debuginator, hot_key_key, full_path, 0, hot_key_value);
		}
		else {
			debuginator_assign_hot_key(debuginator, hot_key_key, full_path, DEBUGINATOR_NO_HOT_INDEX, NULL);
		}
	}

	debuginator__deallocate(debuginator, full_path);

	return item;
}

bool debuginator_save(TheDebuginator* debuginator, DebuginatorSaveItemCallback callback, void* userdata) {
	char current_full_path[1024] = { 0 };
	int path_indices[DEBUGINATOR_MAX_HIERARCHY_SIZE] = { 0 };
	int current_path_index = 0;

	DebuginatorItem* item = debuginator->root->folder.first_child;
	while (item != NULL) {
		if (item->is_folder) {
			if (item->folder.first_child != NULL) {
				int folder_title_length = (int)DEBUGINATOR_strlen(item->title);
				DEBUGINATOR_strcpy_s(current_full_path + path_indices[current_path_index], folder_title_length + 1, item->title); // HACK

				if (item->folder.is_collapsed) {
					bool saved = callback(current_full_path, DEBUGINATOR_FOLDER_COLLAPSED_STRING, userdata);
					if (!saved) {
						return false;
					}
				}

				path_indices[current_path_index + 1] = path_indices[current_path_index] + folder_title_length;
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


			if (item->leaf.active_index != item->leaf.default_index && !debuginator->edit_types[item->leaf.edit_type].forget_state) {
				bool saved = callback(current_full_path, item->leaf.value_titles[item->leaf.active_index], userdata);
				if (!saved) {
					return false;
				}
			}

			if (item->leaf.hot_key_index != DEBUGINATOR_NO_HOT_INDEX) {
				const char* hot_key_key = debuginator->hot_keys[item->leaf.hot_key_index].key;
				int hot_key_value_index = debuginator->hot_keys[item->leaf.hot_key_index].value_index;
				const char* hot_key_value_title = hot_key_value_index == DEBUGINATOR_NO_HOT_INDEX ? "" : item->leaf.value_titles[hot_key_value_index];
				char hot_path_key[DEBUGINATOR_MAX_PATH_LENGTH + 16] = {0};
				DEBUGINATOR_sprintf_s(hot_path_key, sizeof(hot_path_key), "%s__HotKey_Key", current_full_path);
				char hot_path_value[DEBUGINATOR_MAX_PATH_LENGTH + 16] = {0};
				DEBUGINATOR_sprintf_s(hot_path_value, sizeof(hot_path_value), "%s__HotKey_Value", current_full_path);
				bool saved = callback(hot_path_key, hot_key_key, userdata);
				if (!saved) {
					return false;
				}
				saved = callback(hot_path_value, hot_key_value_title, userdata);
				if (!saved) {
					return false;
				}
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

	return true;
}

void debuginator_load_item(TheDebuginator* debuginator, const char* key, const char* value) {
	// First we store path + value so that we can use it later when (re)creating the item.
	debuginator__store_item_setting(debuginator, key, value);

	// Then we check if the item already existed, and if so, update its state.
	DebuginatorItem* item = debuginator_get_item(debuginator, NULL, key, NULL);
	if (item == NULL) {
		// Pass
	}
	else if (item->is_folder) {
		bool is_folder_collapsed_title = DEBUGINATOR_strcmp(value, DEBUGINATOR_FOLDER_COLLAPSED_STRING) == 0;
		if (is_folder_collapsed_title) {
			debuginator_set_collapsed(debuginator, item, true);
		}
	}
	else {
		for (int i = 0; i < item->leaf.num_values; i++) {
			if (DEBUGINATOR_strcmp(item->leaf.value_titles[i], value) == 0) {
				item->leaf.hot_index = i;
				if (!debuginator->edit_types[item->leaf.edit_type].forget_state) {
					debuginator_activate(debuginator, item, false);
				}
				break;
			}
		}
	}
}

DebuginatorItem* debuginator_get_hot_item(TheDebuginator* debuginator, int* out_hot_item_index) {
	if (out_hot_item_index != NULL) {
		if (debuginator->hot_item->is_folder) {
			*out_hot_item_index = 0;
		}
		else if (debuginator->hot_item->leaf.is_expanded) {
			*out_hot_item_index = debuginator->hot_item->leaf.hot_index;
		}
		else {
			*out_hot_item_index = DEBUGINATOR_NO_HOT_INDEX;
		}
	}

	return debuginator->hot_item;
}

void debuginator_set_hot_item(TheDebuginator* debuginator, DebuginatorItem* item) {
	if (item == debuginator->root) {
		return;
	}

	debuginator->hot_item = item;
	item->parent->folder.hot_child = item;
}

void debuginator_set_default_value(TheDebuginator* debuginator, const char* path, const char* value_title, int value_index) {
	DebuginatorItem* item = debuginator_get_item(debuginator, NULL, path, NULL);
	if (item == NULL || item->is_folder) {
		return;
	}

	if (value_title != NULL) {
		for (int i = 0; i < item->leaf.num_values; i++) {
			if (DEBUGINATOR_strcmp(value_title, item->leaf.value_titles[i]) == 0) {
				value_index = i;
				break;
			}
		}
	}

	if (0 <= value_index && value_index < item->leaf.num_values) {
		item->leaf.default_index = value_index;
	}
}

void debuginator_modify_value(TheDebuginator* debuginator, DebuginatorItem* item, float x_axis, float y_axis, bool snap) {
	if (debuginator->edit_types[item->leaf.edit_type].modify_value != NULL) {
		debuginator->edit_types[item->leaf.edit_type].modify_value(debuginator, item, x_axis, y_axis, snap);
	}
}

void debuginator_set_edit_type(TheDebuginator* debuginator, const char* path, DebuginatorItemEditorDataType edit_type) {
	DebuginatorItem* item = debuginator_get_item(debuginator, NULL, path, NULL);
	if (item == NULL) {
		return;
	}

	item->leaf.edit_type = edit_type;
}

void debuginator_item_set_on_changed_callback(DebuginatorItem* item, DebuginatorOnItemChangedCallback callback) {
	DEBUGINATOR_assert(!item->is_folder);
	item->leaf.on_item_changed_callback = callback;
}

void debuginator_item_set_on_changed_callback_by_path(TheDebuginator* debuginator, const char* path, DebuginatorOnItemChangedCallback callback) {
	DebuginatorItem* item = debuginator_get_item(debuginator, NULL, path, NULL);
	if (item == NULL) {
		return;
	}

	DEBUGINATOR_assert(!item->is_folder);
	item->leaf.on_item_changed_callback = callback;
}

void debuginator_item_set_user_data(DebuginatorItem* item, void* user_data) {
	DEBUGINATOR_assert(!item->is_folder);
	item->user_data = user_data;
}

void debuginator_item_set_user_data_by_path(TheDebuginator* debuginator, const char* path, void* user_data) {
	DebuginatorItem* item = debuginator_get_item(debuginator, NULL, path, NULL);
	if (item == NULL) {
		return;
	}

	DEBUGINATOR_assert(!item->is_folder);
	item->user_data = user_data;
}

// Note: If you remove the last visible item, you must create a new one under the root.
void debuginator_remove_item(TheDebuginator* debuginator, DebuginatorItem* item) {
	if (item->is_folder) {
		DebuginatorItem* child = item->folder.first_child;
		while (child != NULL) {
			debuginator_remove_item(debuginator, child);
			child = item->folder.first_child;
		}
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
	}

	if (parent->folder.first_child == item) {
		parent->folder.first_child = item->next_sibling;
	}

	if (debuginator->hot_item == item) {
		debuginator->hot_item = debuginator__nearest_visible_item(item);
	}
	
	// Only update the height of the parent if we were visible
	if (item->parent->is_folder && !item->parent->folder.is_collapsed) {
		debuginator__set_total_height(item->parent, item->parent->total_height - item->total_height);
	}

	debuginator__set_total_height(item->parent, item->parent->total_height - item->total_height);

	if (!item->is_folder && !item->is_filtered) {
		// If it's a folder we've already adjusted the parent's count when we removed the item's children above.
		debuginator__adjust_num_visible_children(item->parent, -1);
	}

	debuginator__deallocate(debuginator, item->title);
	if (!item->is_folder) {
		// Deallocate things in case The Debuginator owns these things.
		debuginator__deallocate(debuginator, item->leaf.description);
		for (int i = 0; i < item->leaf.num_values; ++i) {
			debuginator__deallocate(debuginator, item->leaf.value_titles[i]);
		}
		debuginator__deallocate(debuginator, item->leaf.value_titles);
	}

	debuginator__deallocate(debuginator, item);
}

void debuginator_remove_item_by_path(TheDebuginator* debuginator, const char* path) {
	DebuginatorItem* item = debuginator_get_item(debuginator, NULL, path, NULL);
	if (item == NULL) {
		return;
	}

	debuginator_remove_item(debuginator, item);
}

static bool debuginator__distance_to_hot_item(DebuginatorItem* item, DebuginatorItem* hot_item, int item_height, int* distance) {
	if (item == hot_item) {
		if (!item->is_folder && item->leaf.is_expanded && !item->is_filtered) {
			*distance += item_height * (item->leaf.hot_index + 1);
		}
		return true;
	}

	*distance += item_height;
	if (item->is_folder && !item->is_filtered) {
		DebuginatorItem* child = debuginator__first_visible_child(item);
		while (child) {
			bool found = debuginator__distance_to_hot_item(child, hot_item, item_height, distance);
			if (found) {
				return true;
			}

			child = debuginator__next_visible_sibling(child);
		}
	} else if (item->leaf.is_expanded && !item->is_filtered) {
		*distance += item_height * item->leaf.num_values;
		*distance += item->leaf.description_line_count * item_height;
	}
	return false;
}

int debuginator_total_height(TheDebuginator* debuginator) {
	return debuginator->root->total_height;
}

bool debuginator_is_filtering_enabled(TheDebuginator* debuginator) {
	return debuginator->filter_enabled;
}

void debuginator_set_filtering_enabled(TheDebuginator* debuginator, bool enabled) {
	debuginator->filter_enabled = enabled;
	// debuginator->draw_mode = enabled ? DEBUGINATOR_DrawModeSortedFilter : DEBUGINATOR_DrawModeHierarchy;
}

const char* debuginator_get_filter(TheDebuginator* debuginator) {
	return debuginator->filter;
}

void debuginator_set_filter(TheDebuginator* debuginator, const char* wanted_filter) {
	DEBUGINATOR_strcpy_s(debuginator->filter, sizeof(debuginator->filter), wanted_filter);
}

void debuginator_update_filter(TheDebuginator* debuginator, const char* wanted_filter) {
	// See this for a description of how the fuzzy filtering works.
	// https://medium.com/@Srekel/implementing-a-fuzzy-search-algorithm-for-the-debuginator-cacc349e6c55

	const size_t filter_length = DEBUGINATOR_strlen(wanted_filter);
	bool expanding_search = false;
	if (filter_length < DEBUGINATOR_strlen(debuginator->filter)) {
		if (DEBUGINATOR_memcmp(wanted_filter, debuginator->filter, filter_length) == 0) {
			// The new filter is the same as the old one, minus 1-N characters.
			expanding_search = true;
		}
	}

	// Remove "No entries found" item - we can find new ones even with a longer filter if it
	// means scoring higher
	if (debuginator->hot_item->user_data == (void*)0x12345678) {
		debuginator_remove_item(debuginator, debuginator->hot_item);
	}

	// Exact search
	// "el eb" matches "Debuginator/Help"
	// "oo " doesn't match "lolol"
	// "aa aa" doesn't match "Cars/Saab and Volvo"
	// "aa aa" matches "Caars/Saab"
	bool exact_search = false;
	for (size_t i = 0; i < filter_length; i++) {
		if (wanted_filter[i] == ' ') {
			exact_search = true;
			break;
		}
	}

	char filter[32] = { 0 };
	for (int i = 0; i < 20; i++) {
		// TODO fix loop counter
		filter[i] = (char)DEBUGINATOR_tolower(wanted_filter[i]);
	}

	DEBUGINATOR_memset(debuginator->sorted_items, 0, sizeof(debuginator->sorted_items));

	char current_full_path[DEBUGINATOR_MAX_PATH_LENGTH] = { 0 };
	char current_full_path_lowercase[DEBUGINATOR_MAX_PATH_LENGTH] = { 0 };
	int path_indices[DEBUGINATOR_MAX_HIERARCHY_SIZE] = { 0 };
	int current_path_index = 0;

	int best_score = -1;
	int worst_score = 0;
	DebuginatorItem* best_item = NULL;
	debuginator->best_sorted_item = NULL;

	DebuginatorItem* item = debuginator->root->folder.first_child;
	while (item != NULL) {
		if (item->is_folder) {
			if (item->folder.first_child != NULL) {
				DEBUGINATOR_strcpy_s(current_full_path + path_indices[current_path_index], sizeof(current_full_path) - path_indices[current_path_index], item->title);
				DEBUGINATOR_strcpy_s(current_full_path_lowercase + path_indices[current_path_index], sizeof(current_full_path) - path_indices[current_path_index], item->title);

				path_indices[current_path_index+1] = path_indices[current_path_index] + (int)DEBUGINATOR_strlen(item->title);
				*(current_full_path + path_indices[current_path_index + 1]) = ' ';
				*(current_full_path_lowercase + path_indices[current_path_index + 1]) = ' ';
				path_indices[current_path_index + 1]++;
				DEBUGINATOR_assert(path_indices[current_path_index + 1] < sizeof(current_full_path));

				for (int i = path_indices[current_path_index]; i < path_indices[current_path_index + 1]; i++) {
					current_full_path_lowercase[i] = (char)DEBUGINATOR_tolower(current_full_path_lowercase[i]);
				}

				++current_path_index;
				DEBUGINATOR_assert(current_path_index + 1 < sizeof(path_indices) / sizeof(path_indices[0]));
				item = item->folder.first_child;

				continue;
			}
		}
		else {
			bool taken_chars[DEBUGINATOR_MAX_PATH_LENGTH] = { 0 };
			DEBUGINATOR_static_assert(sizeof(taken_chars) == sizeof(current_full_path));
			DEBUGINATOR_strcpy_s(current_full_path + path_indices[current_path_index], sizeof(current_full_path) - path_indices[current_path_index], item->title);
			DEBUGINATOR_strcpy_s(current_full_path_lowercase + path_indices[current_path_index], sizeof(current_full_path) - path_indices[current_path_index], item->title);
			path_indices[current_path_index + 1] = path_indices[current_path_index] + (int)DEBUGINATOR_strlen(item->title);
			DEBUGINATOR_assert(path_indices[current_path_index + 1] < sizeof(current_full_path));
			int current_path_length = path_indices[current_path_index + 1];

			for (int i = path_indices[current_path_index]; i < current_path_length; i++) {
				current_full_path_lowercase[i] = (char)DEBUGINATOR_tolower(current_full_path_lowercase[i]);
			}

			int score = -1;
			bool is_filtered = false;

			int filter_part = 0;
			while (filter[filter_part] != '\0') {
				if (filter[filter_part] == ' ') {
					++filter_part;
					continue;
				}

				int path_part = 0;
				int matches[8] = { 0 };
				int match_count = 0;
				while (current_full_path_lowercase[path_part] != '\0') {
					bool filter_part_found = false;
					for (int path_i = path_part; path_i < current_path_length; path_i++) {
						if (current_full_path_lowercase[path_i] == filter[filter_part] && taken_chars[path_i] == false) {
							path_part = path_i;
							filter_part_found = true;
							break;
						}
					}

					if (!filter_part_found) {
						break;
					}

					int match_length = 0;
					const char* filter_char = filter + filter_part;
					const char* path_char = current_full_path_lowercase + path_part;
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
				int best_match_score = 0;
				for (int i = 0; i < match_count; i += 2) {
					int match_index = matches[i];
					int match_length = matches[i + 1];
					int is_word_break_start = match_index == 0
						|| current_full_path[match_index - 1] == ' '
						|| (!DEBUGINATOR_isalpha(current_full_path[match_index - 1]) && DEBUGINATOR_isalpha(current_full_path[match_index]))
						|| (!DEBUGINATOR_isdigit(current_full_path[match_index - 1]) && DEBUGINATOR_isdigit(current_full_path[match_index]))
						|| (!DEBUGINATOR_isupper(current_full_path[match_index - 1]) && DEBUGINATOR_isupper(current_full_path[match_index]));
					int is_word_break_end = match_index + match_length == current_path_length
						|| current_full_path[match_index + match_length] == ' '
						|| (!DEBUGINATOR_isalpha(current_full_path[match_index + match_length - 1]) && DEBUGINATOR_isalpha(current_full_path[match_index + match_length]))
						|| (!DEBUGINATOR_isdigit(current_full_path[match_index + match_length - 1]) && DEBUGINATOR_isdigit(current_full_path[match_index + match_length]))
						|| (!DEBUGINATOR_isupper(current_full_path[match_index + match_length - 1]) && DEBUGINATOR_isupper(current_full_path[match_index + match_length]));
					int is_match_in_item_title = match_index >= path_indices[current_path_index];
					int path_segment_length = path_indices[current_path_index + 1] - path_indices[current_path_index];
					int path_segment_modifier = 0;
					while (path_segment_length >>= 1) {
						++path_segment_modifier;
					}

					// The goal here is to severely punish matching in the middle of words, especially for short matches.
					// For example, we don't want to get "detail" as a filter result to "AI", as it just clutters the
					// result list.
					// However, we do want to be able to type "pick" and match "Colorpicker", and there is no practical
					// way to detect that there is a word boundary between "Color" and "picker".
					// So we give a score of zero to filters that don't align with the start or end of a word, except
					// when the match is long enough (3+), in which case, we add anything that matches that too.
					int score_word_start = is_word_break_start * DEBUGINATOR_SCORE_WORD_BREAK_START;
					int score_word_break_end = is_word_break_end * DEBUGINATOR_SCORE_WORD_BREAK_END;
					int score_match_in_item_title = is_match_in_item_title * DEBUGINATOR_SCORE_ITEM_TITLE_MATCH;
					int score_match_length = match_length * match_length;
					int score_match_length_fallback = match_length < 3 ? 0 : match_length * match_length;
					int match_score =
								(score_word_start + score_word_break_end)
								* (score_match_length + score_match_in_item_title)
								+ score_match_length_fallback
								- path_segment_modifier;

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
					for (int match_i = 0; match_i < matches[best_match_index + 1]; match_i++) {
						taken_chars[matches[best_match_index] + match_i] = true;
					}
				}
			}

			// Ignore items in collapsed folders.
			// TODO: Make smarter.
			DebuginatorItem* parent = item->parent;
			while(parent) {
				if (parent->folder.is_collapsed) {
					// If a folder is collapsed, we want to treat this item as filtered IF
					// we actually have a filter. Otherwise, it's business as usual and we
					// clear the is_filtered flag.
					is_filtered = filter_length > 0;
					score = -1;
					break;
				}

				parent = parent->parent;
			}

			if (is_filtered && !item->is_filtered) {
				debuginator__set_total_height(item, 0);
				debuginator__adjust_num_visible_children(item->parent, -1);
				item->leaf.is_expanded = false;
			}
			else if (!is_filtered && item->is_filtered) {
				debuginator__set_total_height(item, debuginator->item_height); //Hacky
				debuginator__adjust_num_visible_children(item->parent, 1);
			}

			item->is_filtered = is_filtered;

			if (score > best_score) {
				if (item == debuginator->hot_item) {
					score++;
				}
				best_score = score;
				best_item = item;
			}

			if (debuginator->draw_mode == DEBUGINATOR_DrawModeSortedFilter && score > worst_score) {
				DebuginatorSortedItem* sorted_item = NULL;
				int worst_count = 0;
				for (int i_si = 0; i_si < DEBUGINATOR_SORTED_ITEM_COUNT; ++i_si) {
					if (debuginator->sorted_items[i_si].score == worst_score) {
						++worst_count;
						sorted_item = &debuginator->sorted_items[i_si];
					}
				}

				DEBUGINATOR_assert(worst_count > 0);
				DEBUGINATOR_assert(sorted_item);

				if (worst_count == 1) {
					// There was only one item with the worst score.
					// We're replacing it, so update the new worst score.
					worst_score = score;
				}

				// First we remove the sorted item from the linked list, and ensure that its
				// previous neighbors have the right references.
				if (sorted_item->prev) {
					sorted_item->prev->next = sorted_item->next;
				}

				if (sorted_item->next) {
					sorted_item->next->prev = sorted_item->prev;
				}

				sorted_item->score = score;
				sorted_item->item = item;
				sorted_item->prev = NULL;
				sorted_item->next = NULL;


				// Find the first item that we score higher than, and insert the new item before it.
				DebuginatorSortedItem* next = debuginator->best_sorted_item;
				while (next) {
					if (sorted_item->score > next->score) {
						sorted_item->prev = next->prev;
						sorted_item->next = next;
						if (next->prev) {
							next->prev->next = sorted_item;
						}
						next->prev = sorted_item;
						break;
					}

					next = next->next;
				}


				if (sorted_item->prev == NULL) {
					// We're the first item
					debuginator->best_sorted_item = sorted_item;
				} else if (sorted_item->next == NULL && debuginator->best_sorted_item != NULL) {
					// We're the last item (worst scoring of the ones found so far)
					next = debuginator->best_sorted_item;
					while (next->next) {
						next = next->next;
					}
					next->next = sorted_item;
					sorted_item->prev = next;
				}
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

	if (expanding_search && debuginator->hot_item != NULL && !debuginator->hot_item->is_filtered) {
	 	// We're good. Just keep the previously hot item.
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

	debuginator__set_item_total_height_recursively(debuginator->root, debuginator->item_height);

	int distance_from_root_to_hot_item = 0;
	debuginator__distance_to_hot_item(debuginator->root, debuginator->hot_item, debuginator->item_height, &distance_from_root_to_hot_item);
	float wanted_y = DEBUGINATOR_FILTER_HEIGHT + debuginator->size.y * debuginator->focus_height + (float)debuginator->scroll_current;
	float distance_to_wanted_y = wanted_y - distance_from_root_to_hot_item;
	debuginator->current_height_offset = distance_to_wanted_y;

	DEBUGINATOR_strcpy_s(debuginator->filter, sizeof(debuginator->filter), filter);
}

void debuginator_apply_scroll(TheDebuginator* debuginator, int distance) {
	debuginator->scroll_wanted += distance;

	// Make sure we don't scroll too far away from the menu's content.
	int distance_from_root_to_hot_item = 0;
	debuginator__distance_to_hot_item(debuginator->root, debuginator->hot_item, debuginator->item_height, &distance_from_root_to_hot_item);
	int max_offset = (int)(debuginator->size.y / 2);
	debuginator->scroll_wanted = DEBUGINATOR_min(debuginator->scroll_wanted, max_offset + distance_from_root_to_hot_item);
	debuginator->scroll_wanted = DEBUGINATOR_max(debuginator->scroll_wanted, -debuginator->root->total_height + distance_from_root_to_hot_item + max_offset);
}

void debuginator_reset_scrolling(TheDebuginator* debuginator) {
	debuginator->scroll_wanted = 0;
}

void debuginator_set_mouse_cursor_pos(TheDebuginator* debuginator, DebuginatorVector2* mouse_cursor_pos) {
	debuginator->mouse_cursor_pos = *mouse_cursor_pos;
}

void debuginator_activate_item_at_mouse_cursor(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator->hot_mouse_item;
	if (hot_item == NULL) {
		return;
	}

	int distance_from_root_to_hot_item_before = 0;
	debuginator__distance_to_hot_item(debuginator->root, debuginator->hot_item, debuginator->item_height, &distance_from_root_to_hot_item_before);

	if (hot_item->is_folder) {
		debuginator->hot_item = hot_item;
		bool collapse = !hot_item->folder.is_collapsed;
		debuginator_set_collapsed(debuginator, hot_item, collapse);
	}
	else if (debuginator->hot_mouse_item_index == DEBUGINATOR_NO_HOT_INDEX) {
		if (!debuginator->edit_types[hot_item->leaf.edit_type].toggle_by_default) {
			debuginator_expand_item_at_mouse_cursor(debuginator, DEBUGINATOR_Toggle);
			return;
		}

		debuginator->hot_item = hot_item;
		if (++hot_item->leaf.hot_index == hot_item->leaf.num_values) {
			hot_item->leaf.hot_index = 0;
		}
		debuginator_activate(debuginator, hot_item, true);
	}
	else {
		debuginator->hot_item = hot_item;
		hot_item->leaf.hot_index = debuginator->hot_mouse_item_index;
		debuginator_activate(debuginator, hot_item, true);
	}

	int distance_from_root_to_hot_item_after = 0;
	debuginator__distance_to_hot_item(debuginator->root, debuginator->hot_item, debuginator->item_height, &distance_from_root_to_hot_item_after);

	debuginator->scroll_wanted += (distance_from_root_to_hot_item_after - distance_from_root_to_hot_item_before);
	debuginator->scroll_current = debuginator->scroll_wanted;
}

void debuginator_expand_item_at_mouse_cursor(TheDebuginator* debuginator, DebuginatorExpand expand) {
	DebuginatorItem* hot_item = debuginator->hot_mouse_item;
	if (hot_item == NULL) {
		return;
	}

	int distance_from_root_to_hot_item_before = 0;
	debuginator__distance_to_hot_item(debuginator->root, debuginator->hot_item, debuginator->item_height, &distance_from_root_to_hot_item_before);

	if (!debuginator->hot_item->is_folder && debuginator->hot_item != hot_item) {
		debuginator->hot_item->leaf.is_expanded = false;
		debuginator__set_total_height(debuginator->hot_item, debuginator->item_height);
	}

	debuginator->hot_item = hot_item;
	if (hot_item->is_folder) {
		bool collapse = expand == DEBUGINATOR_Toggle ? !hot_item->folder.is_collapsed : !(int)expand;
		debuginator_set_collapsed(debuginator, hot_item, collapse);
	}
	else {
		bool do_expand = expand == DEBUGINATOR_Toggle ? !hot_item->leaf.is_expanded : (int)expand;
		hot_item->leaf.is_expanded = do_expand;
		if (do_expand) {
			debuginator__set_total_height(hot_item, debuginator->item_height * (hot_item->leaf.num_values + hot_item->leaf.description_line_count));
		}
		else {
			debuginator__set_total_height(hot_item, debuginator->item_height);
		}
	}

	int distance_from_root_to_hot_item_after = 0;
	debuginator__distance_to_hot_item(debuginator->root, debuginator->hot_item, debuginator->item_height, &distance_from_root_to_hot_item_after);

	debuginator->scroll_wanted += (distance_from_root_to_hot_item_after - distance_from_root_to_hot_item_before);
	debuginator->scroll_current = debuginator->scroll_wanted;
}

DebuginatorItem* debuginator_get_item_at_mouse_cursor(TheDebuginator* debuginator, int* out_hot_item_index) {
	if (out_hot_item_index != NULL) {
		*out_hot_item_index = debuginator->hot_mouse_item_index;
	}

	return debuginator->hot_mouse_item;
}

bool debuginator_is_mouse_over(TheDebuginator* debuginator, bool* out_over_quick_draw_area) {
	if (debuginator->openness == 0) {
		return false;
	}

	float right_edge_x = debuginator->top_left.x + debuginator->size.x;
	if (out_over_quick_draw_area != NULL) {
		*out_over_quick_draw_area =
			right_edge_x - debuginator->quick_draw_size < debuginator->mouse_cursor_pos.x &&
			debuginator->mouse_cursor_pos.x < right_edge_x;
	}

	return
		debuginator->top_left.x <= debuginator->mouse_cursor_pos.x &&
		debuginator->mouse_cursor_pos.x <= right_edge_x;
}

void debuginator_assign_hot_key(TheDebuginator* debuginator, const char* _key, const char* path, int value_index, const char* optional_value_title) {
#if DEBUGINATOR_DO_HOT_KEY_UPPERCASING
	char key[128] = {0};
	DEBUGINATOR_strcpy_s(key, 128, _key);
	for (size_t i = 0; i < DEBUGINATOR_strlen(key); i++) {
		key[i] = (char)DEBUGINATOR_toupper(key[i]);
	}
#else
	const char* key = _key;
#endif

	DebuginatorItem* item = debuginator_get_item(debuginator, NULL, path, NULL);
	if (optional_value_title != NULL) {
		if (!item->is_folder) {
			for (int vt_i = 0; vt_i < item->leaf.num_values; ++vt_i) {
				if (DEBUGINATOR_strcmp(optional_value_title, item->leaf.value_titles[vt_i]) == 0) {
					value_index = vt_i;
					break;
				}
			}
		}
	}

	if (debuginator->num_hot_keys == DEBUGINATOR_MAX_NUM_HOT_KEYS) {
		return;
	}

	const char* owned_key = debuginator_copy_string(debuginator, key, 0);
	const char* owned_path = debuginator_copy_string(debuginator, path, 0);

	int i = debuginator->num_hot_keys++;
	debuginator->hot_keys[i].key = owned_key;
	debuginator->hot_keys[i].path = owned_path;
	debuginator->hot_keys[i].value_index = value_index;

	if (item != NULL) {
		item->leaf.hot_key_index = i;
	}
}

void debuginator_unassign_hot_key(TheDebuginator* debuginator, const char* _key) {
#if DEBUGINATOR_DO_HOT_KEY_UPPERCASING
	char key[128] = { 0 };
	DEBUGINATOR_strcpy_s(key, 128, _key);
	for (size_t i = 0; i < DEBUGINATOR_strlen(key); i++) {
		key[i] = (char)DEBUGINATOR_toupper(key[i]);
	}
#else
	const char* key = _key;
#endif

	for (int i = 0; i < debuginator->num_hot_keys; ++i) {
		if (DEBUGINATOR_strcmp(key, debuginator->hot_keys[i].key) == 0) {
			DebuginatorItem* item = debuginator_get_item(debuginator, NULL, debuginator->hot_keys[i].path, NULL);
			if (item != NULL) {
				item->leaf.hot_key_index = DEBUGINATOR_NO_HOT_INDEX;
			}

			debuginator__deallocate(debuginator, debuginator->hot_keys[i].key);
			debuginator__deallocate(debuginator, debuginator->hot_keys[i].path);

			int last_index = --debuginator->num_hot_keys;
			DebuginatorItem* swap_item = debuginator_get_item(debuginator, NULL, debuginator->hot_keys[last_index].path, NULL);
			if (swap_item != NULL) {
				swap_item->leaf.hot_key_index = i;
			}
			debuginator->hot_keys[i] = debuginator->hot_keys[last_index];
			--i;
		}
	}
}

DebuginatorItem* debuginator_get_first_assigned_hot_key_item(TheDebuginator* debuginator, const char* _key) {
#if DEBUGINATOR_DO_HOT_KEY_UPPERCASING
	char key[128] = { 0 };
	DEBUGINATOR_strcpy_s(key, 128, _key);
	for (size_t i = 0; i < DEBUGINATOR_strlen(key); i++) {
		key[i] = (char)DEBUGINATOR_toupper(key[i]);
	}
#else
	const char* key = _key;
#endif

	for (int i = 0; i < debuginator->num_hot_keys; ++i) {
		if (DEBUGINATOR_strcmp(key, debuginator->hot_keys[i].key) == 0) {
			DebuginatorItem* item = debuginator_get_item(debuginator, NULL, debuginator->hot_keys[i].path, NULL);
			return item;
		}
	}

	return NULL;
}

bool debuginator_activate_hot_key(TheDebuginator* debuginator, const char* _key) {
#if DEBUGINATOR_DO_HOT_KEY_UPPERCASING
	char key[128] = { 0 };
	DEBUGINATOR_strcpy_s(key, 128, _key);
	for (size_t i = 0; i < DEBUGINATOR_strlen(key); i++) {
		key[i] = (char)DEBUGINATOR_toupper(key[i]);
	}
#else
	const char* key = _key;
#endif

	bool success = false;
	for (int i = 0; i < debuginator->num_hot_keys; ++i) {
		if (DEBUGINATOR_strcmp(key, debuginator->hot_keys[i].key) == 0) {
			DebuginatorItem* item = debuginator_get_item(debuginator, NULL, debuginator->hot_keys[i].path, NULL);
			if (item == NULL) {
				continue;
			}

			if (item->is_folder || item->leaf.num_values < debuginator->hot_keys[i].value_index) {
				continue;
			}

			if (debuginator->hot_keys[i].value_index == DEBUGINATOR_NO_HOT_INDEX) {
				if (++item->leaf.hot_index >= item->leaf.num_values) {
					item->leaf.hot_index = 0;
				}
				debuginator_activate(debuginator, item, true);
			}
			else {
				item->leaf.hot_index = debuginator->hot_keys[i].value_index;
				debuginator_activate(debuginator, item, true);
			}

			success = true;
		}
	}

	return success;
}

void debuginator_clear_hot_keys(TheDebuginator* debuginator) {
	for (int i = 0; i < debuginator->num_hot_keys; ++i) {
		DebuginatorItem* item = debuginator_get_item(debuginator, NULL, debuginator->hot_keys[i].path, NULL);
		if (item != NULL) {
			item->leaf.hot_key_index = DEBUGINATOR_NO_HOT_INDEX;
		}

		debuginator__deallocate(debuginator, debuginator->hot_keys[i].key);
		debuginator__deallocate(debuginator, debuginator->hot_keys[i].path);
	}

	debuginator->num_hot_keys = 0;
}

/*
DebuginatorItem* debuginator_get_hot_key_assignment(TheDebuginator* debuginator, const char* _key) {
#if DEBUGINATOR_DO_HOT_KEY_UPPERCASING
	char key[128] = { 0 };
	DEBUGINATOR_strcpy_s(key, 128, _key);
	for (size_t i = 0; i < DEBUGINATOR_strlen(key); i++) {
		key[i] = (char)DEBUGINATOR_toupper(key[i]);
	}
#else
	const char* key = _key;
#endif

	for (int i = 0; i < debuginator->num_hot_keys; ++i) {
		if (DEBUGINATOR_strcmp(key, debuginator->hot_keys[i].key) == 0) {
			DebuginatorItem* item = debuginator_get_item(debuginator, NULL, debuginator->hot_keys[i].path, NULL);
			return item;
		}
	}

	return NULL;
}

DebuginatorItem* debuginator_get_hot_key_assigned_path(TheDebuginator* debuginator, const char* _key) {
#if DEBUGINATOR_DO_HOT_KEY_UPPERCASING
	char key[128] = { 0 };
	DEBUGINATOR_strcpy_s(key, 128, _key);
	for (size_t i = 0; i < DEBUGINATOR_strlen(key); i++) {
		key[i] = (char)DEBUGINATOR_toupper(key[i]);
	}
#else
	const char* key = _key;
#endif

	for (int i = 0; i < debuginator->num_hot_keys; ++i) {
		if (DEBUGINATOR_strcmp(key, debuginator->hot_keys[i].key) == 0) {
			DebuginatorItem* item = debuginator_get_item(debuginator, NULL, debuginator->hot_keys[i].path, NULL);
			return item;
		}
	}

	return NULL;
}
*/

void debuginator_set_item_height(TheDebuginator* debuginator, int item_height) {
	debuginator->item_height = item_height;
	debuginator__set_item_total_height_recursively(debuginator->root, item_height);

	int distance_from_root_to_hot_item = 0;
	debuginator__distance_to_hot_item(debuginator->root, debuginator->hot_item, debuginator->item_height, &distance_from_root_to_hot_item);
	float wanted_y = debuginator->size.y * debuginator->focus_height;
	float distance_to_wanted_y = wanted_y - distance_from_root_to_hot_item;
	debuginator->current_height_offset = distance_to_wanted_y;
}

void debuginator_set_size(TheDebuginator* debuginator, int width, int height) {
	debuginator->size.x = (float)width;
	debuginator->size.y = (float)height;
}

void debuginator_set_screen_resolution(TheDebuginator* debuginator, int width, int height) {
	debuginator->screen_resolution.x = (float)width;
	debuginator->screen_resolution.y = (float)height;
}

void debuginator_set_left_aligned(TheDebuginator* debuginator, bool left_aligned) {
	debuginator->open_direction = left_aligned ? 1 : -1;
}

bool debuginator_is_left_aligned(TheDebuginator* debuginator) {
	return debuginator->open_direction == 1;
}

float debuginator_distance_from_edge(TheDebuginator* debuginator) {
	return debuginator->openness * debuginator->size.x;
}

void debuginator_get_default_config(TheDebuginatorConfig* config) {
	DEBUGINATOR_memset(config, 0, sizeof(*config));

	config->create_default_debuginator_items = true;
	config->open_direction = 1;
	config->sort_items = true;
	config->focus_height = 0.25f;
	config->item_height = 30;
	config->quick_draw_size = 200;

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
	themes[0].colors[DEBUGINATOR_LineHighlightMouse] = debuginator__color(100, 100, 50, 100);
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
	themes[1].colors[DEBUGINATOR_LineHighlightMouse] = debuginator__color(70, 70, 130, 100);
	themes[1].colors[DEBUGINATOR_ItemEditorOff] = debuginator__color(0, 0, 255, 200);
	themes[1].colors[DEBUGINATOR_ItemEditorOn] = debuginator__color(130, 220, 255, 200);
	themes[1].fonts[DEBUGINATOR_ItemDescription].italic = true;

	// High contrast dark theme
	themes[2].colors[DEBUGINATOR_Background] = debuginator__color(25, 25, 25, 220);
	themes[2].colors[DEBUGINATOR_FolderTitle] = debuginator__color(130, 180, 220, 255);
	themes[2].colors[DEBUGINATOR_ItemTitle] = debuginator__color(140, 140, 140, 250);
	themes[2].colors[DEBUGINATOR_ItemTitleOverridden] = debuginator__color(230, 230, 200, 255);
	themes[2].colors[DEBUGINATOR_ItemTitleHot] = debuginator__color(255, 255, 200, 255);
	themes[2].colors[DEBUGINATOR_ItemTitleActive] = debuginator__color(100, 100, 100, 255);
	themes[2].colors[DEBUGINATOR_ItemTitleActive1] = debuginator__color(140, 140, 140, 255);
	themes[2].colors[DEBUGINATOR_ItemTitleActive2] = debuginator__color(220, 220, 200, 255);
	themes[2].colors[DEBUGINATOR_ItemDescription] = debuginator__color(150, 150, 150, 255);
	themes[2].colors[DEBUGINATOR_ItemValueDefault] = debuginator__color(100, 100, 100, 200);
	themes[2].colors[DEBUGINATOR_ItemValueOverridden] = debuginator__color(230, 230, 200, 200);
	themes[2].colors[DEBUGINATOR_ItemValueHot] = debuginator__color(255, 255, 200, 200);
	themes[2].colors[DEBUGINATOR_LineHighlight] = debuginator__color(100, 100, 100, 150);
	themes[2].colors[DEBUGINATOR_LineHighlightMouse] = debuginator__color(100, 100, 100, 100);
	themes[2].colors[DEBUGINATOR_ItemEditorOff] = debuginator__color(250, 100, 100, 200);
	themes[2].colors[DEBUGINATOR_ItemEditorOn] = debuginator__color(130, 220, 255, 200);
	themes[2].fonts[DEBUGINATOR_ItemDescription].italic = true;

	// High contrast light theme
	themes[3].colors[DEBUGINATOR_Background] = debuginator__color(255, 255, 255, 240);
	themes[3].colors[DEBUGINATOR_FolderTitle] = debuginator__color(0, 50, 00, 255);
	themes[3].colors[DEBUGINATOR_ItemTitle] = debuginator__color(80, 80, 80, 250);
	themes[3].colors[DEBUGINATOR_ItemTitleOverridden] = debuginator__color(80, 80, 255, 255);
	themes[3].colors[DEBUGINATOR_ItemTitleHot] = debuginator__color(0, 130, 130, 255);
	themes[3].colors[DEBUGINATOR_ItemTitleActive] = debuginator__color(100, 100, 100, 255);
	themes[3].colors[DEBUGINATOR_ItemTitleActive1] = debuginator__color(100, 100, 100, 255);
	themes[3].colors[DEBUGINATOR_ItemTitleActive2] = debuginator__color(0, 0, 0, 255);
	themes[3].colors[DEBUGINATOR_ItemDescription] = debuginator__color(150, 150, 0, 255);
	themes[3].colors[DEBUGINATOR_ItemValueDefault] = debuginator__color(100, 100, 100, 200);
	themes[3].colors[DEBUGINATOR_ItemValueOverridden] = debuginator__color(60, 60, 255, 200);
	themes[3].colors[DEBUGINATOR_ItemValueHot] = debuginator__color(255, 255, 100, 200);
	themes[3].colors[DEBUGINATOR_LineHighlight] = debuginator__color(100, 100, 150, 150);
	themes[3].colors[DEBUGINATOR_LineHighlightMouse] = debuginator__color(100, 100, 150, 100);
	themes[3].colors[DEBUGINATOR_ItemEditorOff] = debuginator__color(200, 150, 100, 200);
	themes[3].colors[DEBUGINATOR_ItemEditorOn] = debuginator__color(250, 250, 100, 200);
	themes[3].fonts[DEBUGINATOR_ItemDescription].italic = true;

	config->edit_types[DEBUGINATOR_EditTypeArray].quick_draw = debuginator__quick_draw_default;
	config->edit_types[DEBUGINATOR_EditTypeArray].expanded_draw = debuginator__expanded_draw_default;
	config->edit_types[DEBUGINATOR_EditTypeArray].toggle_by_default = true;
	config->edit_types[DEBUGINATOR_EditTypeArrayExpand].quick_draw = debuginator__quick_draw_default;
	config->edit_types[DEBUGINATOR_EditTypeArrayExpand].expanded_draw = debuginator__expanded_draw_default;
	config->edit_types[DEBUGINATOR_EditTypeActionArray].expanded_draw = debuginator__expanded_draw_default;
	config->edit_types[DEBUGINATOR_EditTypeActionArray].forget_state = true;
	config->edit_types[DEBUGINATOR_EditTypeActionArray].toggle_by_default = true;
	config->edit_types[DEBUGINATOR_EditTypeActionArrayExpand].expanded_draw = debuginator__expanded_draw_default;
	config->edit_types[DEBUGINATOR_EditTypeActionArrayExpand].forget_state = true;
	config->edit_types[DEBUGINATOR_EditTypeBoolean].quick_draw = debuginator__quick_draw_boolean;
	config->edit_types[DEBUGINATOR_EditTypeBoolean].expanded_draw = debuginator__expanded_draw_boolean;
	config->edit_types[DEBUGINATOR_EditTypeBoolean].toggle_by_default = true;
	config->edit_types[DEBUGINATOR_EditTypePreset].quick_draw = debuginator__quick_draw_preset;
	config->edit_types[DEBUGINATOR_EditTypePreset].expanded_draw = debuginator__expanded_draw_preset;
	config->edit_types[DEBUGINATOR_EditTypePreset].toggle_by_default = true;
	config->edit_types[DEBUGINATOR_EditTypePreset].forget_state = true;
	config->edit_types[DEBUGINATOR_EditTypeColorPicker].quick_draw = debuginator__quick_draw_colorpicker;
	config->edit_types[DEBUGINATOR_EditTypeColorPicker].expanded_draw = debuginator__expanded_draw_colorpicker;
	config->edit_types[DEBUGINATOR_EditTypeNumberRange].quick_draw = debuginator__quick_draw_numberrange;
	config->edit_types[DEBUGINATOR_EditTypeNumberRange].expanded_draw = debuginator__expanded_draw_numberrange;
	config->edit_types[DEBUGINATOR_EditTypeNumberRange].modify_value = debuginator__modify_value_numberrange;
	// config->edit_types[DEBUGINATOR_EditTypeNumberRange].activate = debuginator__activate_numberrange;
}

void debuginator_create(TheDebuginatorConfig* config, TheDebuginator* debuginator) {
	DEBUGINATOR_assert(config->draw_rect != NULL);
	DEBUGINATOR_assert(config->draw_text != NULL);
	DEBUGINATOR_assert(config->app_user_data != NULL);
	DEBUGINATOR_assert(config->word_wrap != NULL);
	DEBUGINATOR_assert(config->memory_arena != NULL);
	DEBUGINATOR_assert(config->memory_arena_capacity > 0);
	DEBUGINATOR_assert(config->open_direction == -1 || config->open_direction == 1);
	DEBUGINATOR_assert(config->size.x > 0 && config->size.y > 0);
	DEBUGINATOR_assert(config->screen_resolution.x > 0 && config->screen_resolution.y > 0);

	DEBUGINATOR_memset(debuginator, 0, sizeof(*debuginator));
	debuginator->memory_arena = config->memory_arena;
	debuginator->memory_arena_capacity = config->memory_arena_capacity;

	// Allocators begin at the first block, meaning we waste memory between memory_arena and
	// the first block. That's ok.
	debuginator->allocator_data.arena_end = debuginator->memory_arena + debuginator->memory_arena_capacity;
	debuginator->allocator_data.block_capacity = DEBUGINATOR_ALLOCATOR_BLOCK_SIZE;
	debuginator->allocator_data.next_free_block = (char*)((((DEBUGINATOR_intptr)debuginator->memory_arena + DEBUGINATOR_ALLOCATOR_BLOCK_SIZE - 1) / DEBUGINATOR_ALLOCATOR_BLOCK_SIZE) * DEBUGINATOR_ALLOCATOR_BLOCK_SIZE);
	debuginator__block_allocator_init(&debuginator->allocators[0], 8, &debuginator->allocator_data);
	debuginator__block_allocator_init(&debuginator->allocators[1], 16, &debuginator->allocator_data);
	debuginator__block_allocator_init(&debuginator->allocators[2], 32, &debuginator->allocator_data);
	debuginator__block_allocator_init(&debuginator->allocators[3], 64, &debuginator->allocator_data);
	debuginator__block_allocator_init(&debuginator->allocators[4], sizeof(DebuginatorItem), &debuginator->allocator_data);
	debuginator__block_allocator_init(&debuginator->allocators[5], DEBUGINATOR_ALLOCATOR_BLOCK_SIZE - sizeof(DebuginatorBlockAllocator*), &debuginator->allocator_data);

	debuginator->draw_image = config->draw_image;
	debuginator->draw_rect = config->draw_rect;
	debuginator->draw_text = config->draw_text;
	debuginator->word_wrap = config->word_wrap;
	debuginator->text_size = config->text_size;
	debuginator->on_opened_changed = config->on_opened_changed;
	debuginator->app_user_data = config->app_user_data;

	debuginator->size = config->size;
	debuginator->open_direction = (char)config->open_direction;
	debuginator->focus_height = config->focus_height;
	debuginator->screen_resolution = config->screen_resolution;
	debuginator->item_height = config->item_height;
	debuginator->quick_draw_size = config->quick_draw_size;
	debuginator->draw_mode = DEBUGINATOR_DrawModeHierarchy;
	debuginator->sort_items = config->sort_items;

	debuginator->root_position.x = -debuginator->size.x;
	debuginator->top_left = debuginator__vector2(debuginator->root_position.x + debuginator->size.x * debuginator->openness * debuginator->open_direction, 0);

	DEBUGINATOR_memcpy(debuginator->edit_types, config->edit_types, sizeof(debuginator->edit_types));
	DEBUGINATOR_memcpy(debuginator->themes, config->themes, sizeof(debuginator->themes));
	debuginator->theme_index = 0;
	debuginator->theme = debuginator->themes[0];

	debuginator->bool_values[0] = false;
	debuginator->bool_values[1] = true;
	debuginator->bool_titles[0] = "False";
	debuginator->bool_titles[1] = "True";

	// Ensure mouse interaction starts disabled
	DebuginatorVector2 mouse_cursor_pos = debuginator__vector2(-1.f, -1.f);
	debuginator_set_mouse_cursor_pos(debuginator, &mouse_cursor_pos);

	// Create root
	DebuginatorItem* item = debuginator_new_folder_item(debuginator, NULL, "Menu Root", 0);
	debuginator->root = item;

	if (config->create_default_debuginator_items) {
		{
			debuginator_create_array_item(debuginator, NULL, "Debuginator/Help/About",
				"The Debuginator is an open source debug menu made by Anders 'Srekel' Elfgren.\nLatest version can be found here: https://github.com/Srekel/the-debuginator", NULL, NULL,
				NULL, NULL, 0, 0);

			// Not sure if this should be here or in each app/plugin that uses The Debuginator.. but I'll put it here for now.
			debuginator_create_array_item(debuginator, NULL, "Debuginator/Help/Keyboard default usage (1)",
				"Open the menu with Right Arrow. \nClose it with Left Arrow. \nUse all arrow keys to navigate. \nRight Arrow is also used to change value on a menu item.", NULL, NULL,
				NULL, NULL, 0, 0);
			debuginator_create_array_item(debuginator, NULL, "Debuginator/Help/Keyboard default usage (2)",
				"Hold CTRL for faster navigation and item toggling. \nEscape to quickly close the menu.\nBackspace to toggle search.", NULL, NULL,
				NULL, NULL, 0, 0);

			debuginator_create_array_item(debuginator, NULL, "Debuginator/Help/Gamepad default usage (1)",
				"Open the menu with Start/Options button. \nClose it with Left D-Pad. \nUse D-Pad to navigate. \nD-Pad Right is used to change value on a menu item.", NULL, NULL,
				NULL, NULL, 0, 0);
			debuginator_create_array_item(debuginator, NULL, "Debuginator/Help/Gamepad default usage (2)",
				"Use the corresponding AXBY buttons to do the same things as the D-Pad, but faster!", NULL, NULL,
				NULL, NULL, 0, 0);
		}

		{
			// TODO: Use a special callback instead of copy 1 byte in order to fix startup wonky animations.
			char* directions = (char*)debuginator__allocate(debuginator, 2); // char as in byte
			directions[0] = 1;
			directions[1] = -1;
			const char** string_titles = (const char**)debuginator__allocate(debuginator, sizeof(char*) * 2);
			string_titles[0] = "Left";
			string_titles[1] = "Right";
			debuginator_create_array_item(debuginator, NULL, "Debuginator/Alignment",
				"Right alignment is not fully tested and has some visual glitches.", debuginator_copy_1byte, &debuginator->open_direction,
				string_titles, directions, 2, sizeof(directions[0]));
		}
		{
			char* directions = (char*)debuginator__allocate(debuginator, 2); // char as in byte
			directions[0] = 1;
			directions[1] = -1;
			const char** string_titles = (const char**)debuginator__allocate(debuginator, sizeof(char*) * 2);
			string_titles[0] = "Left";
			string_titles[1] = "Right";
			debuginator_create_bool_item(debuginator, "Debuginator/Sort items", "Makes items be sorted alphabetically by default.", &debuginator->sort_items);
		}
		{
			int* theme_indices = (int*)debuginator__allocate(debuginator, 4 * sizeof(int)); //
			theme_indices[0] = 0;
			theme_indices[1] = 1;
			theme_indices[2] = 2;
			theme_indices[3] = 3;
			const char** string_titles = (const char**)debuginator__allocate(debuginator, sizeof(char*) * 4);
			string_titles[0] = "Classic";
			string_titles[1] = "Blue";
			string_titles[2] = "High Contrast Dark";
			string_titles[3] = "High Contrast Light";
			debuginator_create_array_item(debuginator, NULL, "Debuginator/Theme",
				"Change color theme of The Debuginator. \nNote that only Classic is currently polished.", debuginator__on_change_theme, debuginator,
				string_titles, (void*)theme_indices, 4, sizeof(theme_indices[0]));
		}

		// {
		// 	debuginator_create_bool_item(debuginator, "Debuginator/Settings/Enable notifications",
		// 		"Get popups... or not!", &debuginator->notifications_enabled);
		// }
	}
}

void debuginator_update(TheDebuginator* debuginator, float dt) {
	// To not lerp outside 1
	if (dt > 0.5f) {
		dt = 0.5f;
	}

	debuginator->dt = dt;
	debuginator->draw_timer += dt * 5;
	if (debuginator->is_open && debuginator->openness < 1) {
		debuginator->openness_timer += dt * 5;
		if (debuginator->openness_timer > 1) {
			debuginator->openness_timer = 1;
			if (debuginator->on_opened_changed) {
				debuginator->on_opened_changed(true, true, debuginator->app_user_data);
			}
		}

		debuginator->openness = debuginator__ease_out(debuginator->openness_timer, 0, 1, 1);
	}

	else if (!debuginator->is_open && debuginator->openness > 0) {
		debuginator->openness_timer -= dt * 5;
		if (debuginator->openness_timer < 0) {
			debuginator->openness_timer = 0;
			if (debuginator->on_opened_changed) {
				debuginator->on_opened_changed(false, true, debuginator->app_user_data);
			}
		}

		debuginator->openness = debuginator__ease_out(debuginator->openness_timer, 0, 1, 1);
	}

	// Smooth scrolling
	debuginator->scroll_current = (int)debuginator__lerp((float)debuginator->scroll_current, (float)debuginator->scroll_wanted, DEBUGINATOR_min(0.9f, dt * 10.0f));

	// Ensure hot item is smoothly placed at a nice position
	int distance_from_root_to_hot_item = 0;
	debuginator__distance_to_hot_item(debuginator->root, debuginator->hot_item, debuginator->item_height, &distance_from_root_to_hot_item);
	float wanted_y = DEBUGINATOR_FILTER_HEIGHT + debuginator->size.y * debuginator->focus_height + debuginator->scroll_current;
	float distance_to_wanted_y = wanted_y - distance_from_root_to_hot_item;
	debuginator->current_height_offset = debuginator__lerp(debuginator->current_height_offset, distance_to_wanted_y, DEBUGINATOR_min(1, dt * 10));
	if (DEBUGINATOR_fabs(debuginator->current_height_offset - distance_to_wanted_y) < 0.1f) {
		debuginator->current_height_offset = distance_to_wanted_y;
	}

	// Update left vs right
	if (debuginator->open_direction == 1) {
		debuginator->root_position.x = -debuginator->size.x;
	}
	else {
		debuginator->root_position.x = debuginator->screen_resolution.x;
	}

	// Update "origin" position
	debuginator->top_left = debuginator__vector2(debuginator->root_position.x + debuginator->size.x * debuginator->openness * debuginator->open_direction, 0);
}


float debuginator_draw_item(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2 offset, bool hot);
void debuginator__draw_hierarchy(TheDebuginator* debuginator, float dt, DebuginatorVector2 offset);
void debuginator__draw_sorted_filter(TheDebuginator* debuginator, float dt, DebuginatorVector2 offset);
void debuginator__draw_animations(TheDebuginator* debuginator, float dt);
void debuginator__draw_search_filter(TheDebuginator* debuginator, float dt);

void debuginator_draw(TheDebuginator* debuginator, float dt) {
	// Don't do anything if we're fully closed
	if (!debuginator->is_open && debuginator->openness == 0) {
		return;
	}

	// Update theme opacity
	DebuginatorTheme* source_theme = &debuginator->themes[debuginator->theme_index];
	for (int i = 0; i < DEBUGINATOR_NumDrawTypes; i++) {
		debuginator->theme.colors[i].a = (unsigned char)(source_theme->colors[i].a * debuginator->openness);
	}

	float lerp_t = (float)(DEBUGINATOR_sin(debuginator->draw_timer * 2) + 1) * 0.5f;
	debuginator->theme.colors[DEBUGINATOR_ItemTitleActive].r = (unsigned char)debuginator__lerp((float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive1].r, (float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive2].r, lerp_t);
	debuginator->theme.colors[DEBUGINATOR_ItemTitleActive].g = (unsigned char)debuginator__lerp((float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive1].g, (float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive2].g, lerp_t);
	debuginator->theme.colors[DEBUGINATOR_ItemTitleActive].b = (unsigned char)debuginator__lerp((float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive1].b, (float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive2].b, lerp_t);

	if (debuginator->hot_mouse_item == NULL) {
		debuginator->theme.colors[DEBUGINATOR_LineHighlightMouse].a = (unsigned char)debuginator__lerp((float)debuginator->theme.colors[DEBUGINATOR_LineHighlightMouse].a, 0, 0.5f);
	}
	else {
		debuginator->theme.colors[DEBUGINATOR_LineHighlightMouse].a = (unsigned char)debuginator__lerp((float)debuginator->theme.colors[DEBUGINATOR_LineHighlightMouse].a, (float)debuginator->theme.colors[DEBUGINATOR_LineHighlight].a - 50, 0.005f);
	}

	// Clear mouse hot item. It gets set appropriately in draw_item.
	debuginator->hot_mouse_item = NULL;
	debuginator->hot_mouse_item_index = DEBUGINATOR_NO_HOT_INDEX;

	// Background
	DebuginatorVector2 offset = debuginator->top_left;
	debuginator->draw_rect(&offset, &debuginator->size, &debuginator->theme.colors[DEBUGINATOR_Background], debuginator->app_user_data);

	if (debuginator->draw_mode == DEBUGINATOR_DrawModeHierarchy) {
		debuginator__draw_hierarchy(debuginator, dt, offset);
		debuginator__draw_animations(debuginator, dt);
	}
	else if (debuginator->draw_mode == DEBUGINATOR_DrawModeSortedFilter) {
		debuginator__draw_sorted_filter(debuginator, dt, offset);
	}

	debuginator__draw_search_filter(debuginator, dt);
}

void debuginator__draw_hierarchy(TheDebuginator* debuginator, float dt, DebuginatorVector2 offset){
	DEBUGINATOR_UNUSED(dt);
	offset.y = debuginator->current_height_offset;

	// Draw all items within the debuginator's draw area
	offset.x += DEBUGINATOR_LEFT_MARGIN;
	DebuginatorItem* item_to_draw = debuginator__first_visible_child(debuginator->root);
	while (item_to_draw && offset.y < -item_to_draw->total_height) {
		// We'll start to draw off-screen which we don't want.
		if (offset.y + item_to_draw->total_height < 0) {
			// Whole item is off-screen, skip to sibling
			offset.y += item_to_draw->total_height;
			item_to_draw = debuginator__next_visible_sibling(item_to_draw);
		}
		else if (item_to_draw->is_folder) {
			// Part of item is off-screen, find which child to draw
			offset.x += DEBUGINATOR_INDENT;
			offset.y += debuginator->item_height;
			item_to_draw = debuginator__first_visible_child(item_to_draw);
		}
		else {
			DEBUGINATOR_assert(false);
		}
	}

	// item_to_draw is now the first item (folder most likely) who is at least partly
	// within the rendering area.
	while (item_to_draw && offset.y < debuginator->size.y) {
		debuginator_draw_item(debuginator, item_to_draw, offset, debuginator->hot_item == item_to_draw);
		offset.y += item_to_draw->total_height;
		while (item_to_draw && debuginator__next_visible_sibling(item_to_draw) == NULL) {
			offset.x -= DEBUGINATOR_INDENT;
			item_to_draw = item_to_draw->parent;
		}

		if (item_to_draw == NULL) {
			break;
		}

		item_to_draw = debuginator__next_visible_sibling(item_to_draw);
	}
}

void debuginator__draw_animations(TheDebuginator* debuginator, float dt) {
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
			DebuginatorItem* anim_item = animation->data.item_activate.item;
			int distance_from_root_to_item = 0;
			debuginator__distance_to_hot_item(debuginator->root, anim_item, debuginator->item_height, &distance_from_root_to_item);

			DebuginatorVector2 start_position = animation->data.item_activate.start_pos;
			DebuginatorVector2 end_position;
			end_position.x = debuginator->top_left.x + debuginator->size.x - debuginator->quick_draw_size;
			end_position.y = distance_from_root_to_item + debuginator->current_height_offset - debuginator->item_height / 2.0f;
			if (anim_item->leaf.is_expanded) {
				end_position.y -= (anim_item->leaf.hot_index + anim_item->leaf.description_line_count - 1) * debuginator->item_height;
			}

			if (anim_item->leaf.num_values) {
				DebuginatorVector2 position;
				position.x = debuginator__ease_out(animation->time, start_position.x, end_position.x - start_position.x, animation->duration);
				position.y = debuginator__ease_out(animation->time, start_position.y, end_position.y - start_position.y, animation->duration);
				DebuginatorFont* font = &debuginator->theme.fonts[DEBUGINATOR_ItemTitle];
				const char* text = anim_item->leaf.value_titles[animation->data.item_activate.value_index];
				debuginator->draw_text(text, &position, &debuginator->theme.colors[DEBUGINATOR_ItemTitleActive], font, debuginator->app_user_data);
			}
		}
	}

	if (running_animations == 0) {
		debuginator->animation_count = 0;
	}
}

void debuginator__draw_search_filter(TheDebuginator* debuginator, float dt) {
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
		DebuginatorVector2 filter_pos = debuginator__vector2(debuginator->top_left.x + debuginator->size.x - 450, 25);
		DebuginatorVector2 filter_size = debuginator__vector2(150 + (debuginator->size.x - 250) * debuginator->filter_timer, DEBUGINATOR_FILTER_HEIGHT);
		DebuginatorColor filter_color = debuginator__color(50, 100, 50, (int)(200 * debuginator->filter_timer * alpha));
		debuginator->draw_rect(&filter_pos, &filter_size, &filter_color, debuginator->app_user_data);

		DebuginatorVector2 header_text_size = debuginator->text_size("Search: ", &debuginator->theme.fonts[DEBUGINATOR_ItemTitleActive], debuginator->app_user_data);
		filter_size.x = header_text_size.x + 40;
		debuginator->draw_rect(&filter_pos, &filter_size, &filter_color, debuginator->app_user_data);

		filter_pos.x += 20;
		filter_pos.y = filter_pos.y + filter_size.y / 2 - header_text_size.y / 2;

		DebuginatorColor header_color = debuginator->theme.colors[DEBUGINATOR_ItemTitleActive];
		header_color.a = (unsigned char)(header_color.a * alpha);
		debuginator->draw_text("Search: ", &filter_pos, &header_color, &debuginator->theme.fonts[DEBUGINATOR_ItemTitleActive], debuginator->app_user_data);

		filter_pos.x += 40;
		filter_pos.x += header_text_size.x;
		if (DEBUGINATOR_strchr(debuginator->filter, ' ')) {
			// Exact search mode
			DebuginatorVector2 underline_size;
			underline_size.y = 1;
			char letter[2] = { 0 };
			for (size_t i = 0; i < DEBUGINATOR_strlen(debuginator->filter); i++) {
				letter[0] = debuginator->filter[i];
				debuginator->draw_text(letter, &filter_pos, &debuginator->theme.colors[DEBUGINATOR_ItemTitleActive], &debuginator->theme.fonts[DEBUGINATOR_ItemTitleActive], debuginator->app_user_data);
				DebuginatorVector2 letter_text_size = debuginator->text_size(letter, &debuginator->theme.fonts[DEBUGINATOR_ItemTitleActive], debuginator->app_user_data);
				underline_size.x = letter_text_size.x;
				if (letter[0] != ' ') {
					DebuginatorVector2 underline_pos = debuginator__vector2(filter_pos.x, filter_pos.y);
					debuginator->draw_rect(&underline_pos, &underline_size, &debuginator->theme.colors[DEBUGINATOR_ItemValueHot], debuginator->app_user_data);
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
			filter_color.a = alpha * DEBUGINATOR_sin(debuginator->draw_timer) < 0.5 ? 220u : 50u;
			debuginator->draw_rect(&caret_pos, &caret_size, &filter_color, debuginator->app_user_data);
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
	bool mouse_over =
		debuginator->top_left.x <= debuginator->mouse_cursor_pos.x && debuginator->mouse_cursor_pos.x < debuginator->top_left.x + debuginator->size.x &&
		offset.y <= debuginator->mouse_cursor_pos.y && debuginator->mouse_cursor_pos.y < offset.y + item->total_height;
	hot = hot || mouse_over;

	// Set hot mouse item here. Yes, in draw.. it's easiest!
	debuginator->hot_mouse_item = mouse_over ? item : debuginator->hot_mouse_item;
	float half_height = debuginator->item_height / 2.0f;

	if (item->is_folder) {
		mouse_over =
			debuginator->top_left.x <= debuginator->mouse_cursor_pos.x && debuginator->mouse_cursor_pos.x < debuginator->top_left.x + debuginator->size.x &&
			offset.y <= debuginator->mouse_cursor_pos.y && debuginator->mouse_cursor_pos.y < offset.y + debuginator->item_height;
		hot = hot || mouse_over;

		if (mouse_over || debuginator->hot_item == item) {
			DebuginatorVector2 highlight_pos = debuginator__vector2(debuginator->top_left.x, offset.y);
			DebuginatorVector2 highlight_size = debuginator__vector2(debuginator->size.x, (float)debuginator->item_height);
			debuginator->draw_rect(&highlight_pos, &highlight_size, &debuginator->theme.colors[mouse_over ? DEBUGINATOR_LineHighlightMouse : DEBUGINATOR_LineHighlight], debuginator->app_user_data);
		}

		unsigned color_index = item == debuginator->hot_item ? DEBUGINATOR_ItemTitleActive : (hot ? DEBUGINATOR_ItemTitleHot : DEBUGINATOR_FolderTitle);
		DebuginatorVector2 text_pos = debuginator__vector2(offset.x, offset.y + half_height);
		debuginator->draw_text(item->title, &text_pos, &debuginator->theme.colors[color_index], &debuginator->theme.fonts[DEBUGINATOR_ItemTitle], debuginator->app_user_data);
		if (item->folder.is_collapsed) {
			DebuginatorVector2 collapsed_box_pos1 = debuginator__vector2(offset.x - debuginator->item_height / 2.0f , offset.y + debuginator->item_height / 4.0f);
			DebuginatorVector2 collapsed_box_size1 = debuginator__vector2(debuginator->item_height / 2.0f, debuginator->item_height / 4.0f);
			DebuginatorVector2 collapsed_box_pos2 = debuginator__vector2(offset.x - debuginator->item_height / 4.0f, offset.y + debuginator->item_height / 8.0f);
			DebuginatorVector2 collapsed_box_size2 = debuginator__vector2(debuginator->item_height / 4.0f, debuginator->item_height / 8.0f);
			debuginator->draw_rect(&collapsed_box_pos1, &collapsed_box_size1, &debuginator->theme.colors[color_index], debuginator->app_user_data);
			debuginator->draw_rect(&collapsed_box_pos2, &collapsed_box_size2, &debuginator->theme.colors[color_index], debuginator->app_user_data);
		}

		offset.x += DEBUGINATOR_INDENT;
		DebuginatorItem* child = debuginator__first_visible_child(item);
		offset.y += debuginator->item_height;
		while (child) {
			if (offset.y > debuginator->size.y) {
				break;
			}

			if (offset.y + child->total_height > 0) {
				debuginator_draw_item(debuginator, child, offset, debuginator->hot_item == child);
			}
			offset.y += child->total_height;
			child = debuginator__next_visible_sibling(child);
		}
	}
	else {
		if (item->leaf.is_expanded && mouse_over && debuginator->hot_mouse_item_index == DEBUGINATOR_NO_HOT_INDEX) {
			DebuginatorVector2 highlight_pos = debuginator__vector2(debuginator->top_left.x, offset.y);
			DebuginatorVector2 highlight_size = debuginator__vector2(debuginator->size.x, (float)debuginator->item_height * (1 + item->leaf.description_line_count));
			debuginator->draw_rect(&highlight_pos, &highlight_size, &debuginator->theme.colors[DEBUGINATOR_LineHighlightMouse], debuginator->app_user_data);
		}

		if (hot && (!item->leaf.is_expanded || item->leaf.num_values == 0)) {
			DebuginatorVector2 highlight_pos = debuginator__vector2(debuginator->top_left.x, offset.y);
			DebuginatorVector2 highlight_size = debuginator__vector2(debuginator->size.x, (float)debuginator->item_height);
			debuginator->draw_rect(&highlight_pos, &highlight_size, &debuginator->theme.colors[mouse_over ? DEBUGINATOR_LineHighlightMouse : DEBUGINATOR_LineHighlight], debuginator->app_user_data);
		}

		bool is_overriden = item->leaf.active_index != item->leaf.default_index && !debuginator->edit_types[item->leaf.edit_type].forget_state;
		unsigned default_color_index = is_overriden ? DEBUGINATOR_ItemTitleOverridden : DEBUGINATOR_ItemTitle;
		unsigned color_index = hot ? DEBUGINATOR_ItemTitleActive : default_color_index;
		DebuginatorFont* font = &debuginator->theme.fonts[DEBUGINATOR_ItemTitle];
		DebuginatorVector2 text_pos = debuginator__vector2(offset.x, offset.y + half_height);
		debuginator->draw_text(item->title, &text_pos, &debuginator->theme.colors[color_index], font, debuginator->app_user_data);

		if (debuginator->edit_types[item->leaf.edit_type].quick_draw != NULL) {
			debuginator->edit_types[item->leaf.edit_type].quick_draw(debuginator, item, &offset);
		}

		if (item->leaf.hot_key_index != DEBUGINATOR_NO_HOT_INDEX
			&& (debuginator->hot_keys[item->leaf.hot_key_index].value_index == DEBUGINATOR_NO_HOT_INDEX || !item->leaf.is_expanded)) {
			const char* key = debuginator->hot_keys[item->leaf.hot_key_index].key;
			int hot_key_value_index = debuginator->hot_keys[item->leaf.hot_key_index].value_index;
			char hot_key_text[DEBUGINATOR_MAX_PATH_LENGTH];
			DEBUGINATOR_sprintf_s(hot_key_text, DEBUGINATOR_MAX_PATH_LENGTH, "[%s] %s", key, hot_key_value_index == DEBUGINATOR_NO_HOT_INDEX ? "Toggle" : item->leaf.value_titles[hot_key_value_index]);

			if (debuginator->open_direction == 1) {
				DebuginatorVector2 key_rect_pos = debuginator__vector2(debuginator->top_left.x + debuginator->size.x, offset.y);
				DebuginatorVector2 key_rect_size = debuginator__vector2(debuginator->text_size(hot_key_text, font, debuginator->app_user_data).x + 20, (float)debuginator->item_height);
				debuginator->draw_rect(&key_rect_pos, &key_rect_size, &debuginator->theme.colors[DEBUGINATOR_Background], debuginator->app_user_data);

				DebuginatorVector2 key_text_pos = debuginator__vector2(debuginator->top_left.x + debuginator->size.x + 10, offset.y + half_height);
				debuginator->draw_text(hot_key_text, &key_text_pos, &debuginator->theme.colors[DEBUGINATOR_ItemTitleOverridden], font, debuginator->app_user_data);
			}
			else {
				DebuginatorVector2 key_rect_size = debuginator__vector2(debuginator->text_size(hot_key_text, font, debuginator->app_user_data).x + 20, (float)debuginator->item_height);
				DebuginatorVector2 key_rect_pos = debuginator__vector2(debuginator->top_left.x - key_rect_size.x, offset.y);
				debuginator->draw_rect(&key_rect_pos, &key_rect_size, &debuginator->theme.colors[DEBUGINATOR_Background], debuginator->app_user_data);

				DebuginatorVector2 key_text_pos = debuginator__vector2(key_rect_pos.x + 10, offset.y + half_height);
				debuginator->draw_text(hot_key_text, &key_text_pos, &debuginator->theme.colors[DEBUGINATOR_ItemTitleOverridden], font, debuginator->app_user_data);
			}
		}

		if (item->leaf.is_expanded) {
			offset.x += DEBUGINATOR_INDENT;

			const char* description = item->leaf.description;
			float description_width = debuginator->size.x - 50 + debuginator->top_left.x - offset.x;
			char description_line_to_draw[64];
			int description_height = 0;
			int row_lengths[32];
			int row_count = 0;
			debuginator->word_wrap(description, debuginator->theme.fonts[DEBUGINATOR_ItemDescription], description_width, &row_count, row_lengths, 32, debuginator->app_user_data);
			int row_index = 0;
			for (int i = 0; i < row_count; i++) {
				int row_index_end = row_index + row_lengths[i];
				const char* description_line = description + row_index;
				DEBUGINATOR_strncpy_s(description_line_to_draw, 64u, description_line, DEBUGINATOR_min(row_index_end - row_index, 64u));
				row_index = row_index_end;
				while (description[row_index] == '\n') {
					++row_index;
				}
				offset.y += debuginator->item_height;
				description_height += debuginator->item_height;
				DebuginatorVector2 description_pos = debuginator__vector2(offset.x, offset.y + half_height);
				debuginator->draw_text(description_line_to_draw, &description_pos, &debuginator->theme.colors[DEBUGINATOR_ItemDescription], &debuginator->theme.fonts[DEBUGINATOR_ItemDescription], debuginator->app_user_data);
			}

			// Feels kinda ugly to do this here but... works for now.
			debuginator__set_total_height(item, debuginator->item_height + description_height + debuginator->item_height * (item->leaf.num_values));

			debuginator->edit_types[item->leaf.edit_type].expanded_draw(debuginator, item, &offset);
		}
	}

	return offset.y;
}

void debuginator__draw_sorted_filter(TheDebuginator* debuginator, float dt, DebuginatorVector2 offset) {
	DEBUGINATOR_UNUSED(dt);
	DebuginatorSortedItem* sorted_item = debuginator->best_sorted_item;
	while (sorted_item && sorted_item->score > 0) {
		DebuginatorItem* item = sorted_item->item;
		DebuginatorVector2 text_pos = debuginator__vector2(offset.x, offset.y + debuginator->item_height / 2.0f);
		debuginator->draw_text(item->title, &text_pos, &debuginator->theme.colors[DEBUGINATOR_ItemDescription], &debuginator->theme.fonts[DEBUGINATOR_ItemDescription], debuginator->app_user_data);
		offset.y += debuginator->item_height;
		sorted_item = sorted_item->next;
	}
}

bool debuginator_is_open(TheDebuginator* debuginator) {
	return debuginator->is_open;
}

void debuginator_set_open(TheDebuginator* debuginator, bool is_open) {
	debuginator->is_open = is_open;
	if (debuginator->on_opened_changed) {
		debuginator->on_opened_changed(is_open, false, debuginator->app_user_data);
	}
}

void debuginator_activate(TheDebuginator* debuginator, DebuginatorItem* item, bool animate) {
	item->leaf.draw_t = 0;
	if (item->leaf.num_values <= 0) {
		// "Action" items doesn't have a list of values, they just get triggered
		if (item->leaf.on_item_changed_callback != NULL) {
			item->leaf.on_item_changed_callback(item, item->leaf.values, NULL, debuginator->app_user_data);
		}
		return;
	}

	int hot_index = item->leaf.hot_index;
	item->leaf.active_index = hot_index;

	if (animate) {
		DebuginatorAnimation* animation = debuginator__get_free_animation(debuginator);
		if (animation != NULL) {
			animation->type = DEBUGINATOR_ItemActivate;
			animation->data.item_activate.item = item;
			animation->data.item_activate.value_index = hot_index;
			animation->data.item_activate.start_pos.x = debuginator->top_left.x;
			animation->duration = 0.5f;

			DebuginatorItem* parent = item->parent;
			while (parent) {
				animation->data.item_activate.start_pos.x += DEBUGINATOR_INDENT;
				parent = parent->parent;
			}

			int y_dist_to_root = 0;
			debuginator__distance_to_hot_item(debuginator->root, item, debuginator->item_height, &y_dist_to_root);
			animation->data.item_activate.start_pos.y = y_dist_to_root + debuginator->current_height_offset - debuginator->item_height / 2;
			if (item->leaf.is_expanded) {
				animation->data.item_activate.start_pos.y += debuginator->item_height * item->leaf.description_line_count;
			}
		}
	}

	if (item->leaf.on_item_changed_callback == NULL) {
		return;
	}

	void* value = ((char*)item->leaf.values) + hot_index * item->leaf.array_element_size;
	item->leaf.on_item_changed_callback(item, value, item->leaf.value_titles[hot_index], debuginator->app_user_data);
}

bool debuginator_is_collapsed(DebuginatorItem* item) {
	return item->is_folder && item->folder.is_collapsed;
}

void debuginator_set_collapsed(TheDebuginator* debuginator, DebuginatorItem* item, bool collapsed) {
	if (!item->is_folder) {
		return;
	}

	if (item == debuginator->root) {
		return;
	}

	item->folder.is_collapsed = collapsed;
	if (collapsed) {
		debuginator__set_total_height(item, debuginator->item_height);
		if (item->folder.hot_child == debuginator->hot_item) {
			DebuginatorItem* temp_item = item;
			while (temp_item->folder.is_collapsed) {
				temp_item = temp_item->parent;
			}
			if (temp_item == debuginator->root) {
				debuginator_set_hot_item(debuginator, debuginator__first_visible_child(temp_item));
			}
			else {
				debuginator_set_hot_item(debuginator, temp_item);
			}
		}
	}
	else {
		int height = debuginator->item_height;
		DebuginatorItem* child = debuginator__first_visible_child(item);
		while (child != NULL) {
			height += child->total_height;
			child = debuginator__next_visible_sibling(child);
		}
		debuginator__set_total_height(item, height);
	}
}

void debuginator_move_sibling_previous(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator->hot_item;

	if (!hot_item->is_folder && hot_item->leaf.is_expanded) {
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

	if (!hot_item->is_folder && hot_item->leaf.is_expanded) {
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

void debuginator_move_to_next_leaf(TheDebuginator* debuginator, bool long_move) {
	DebuginatorItem* hot_item = debuginator->hot_item;
	if (!hot_item->is_folder && hot_item->leaf.is_expanded) {
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

	DebuginatorItem* hot_item_new = debuginator__next_visible_item(hot_item);
	if (long_move && hot_item_new != NULL) {
		while (hot_item_new != NULL && hot_item_new->parent == hot_item->parent && (hot_item_new->is_folder || hot_item_new->leaf.active_index == hot_item_new->leaf.default_index)) {
			hot_item_new = debuginator__next_visible_item(hot_item_new);
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
	if (!hot_item->is_folder && hot_item->leaf.is_expanded) {
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

	DebuginatorItem* hot_item_new = debuginator__prev_visible_item(hot_item);
	if (long_move && hot_item_new != NULL) {
		while (hot_item_new != NULL && hot_item_new->parent == hot_item->parent && (hot_item_new->is_folder || hot_item_new->leaf.active_index == hot_item_new->leaf.default_index)) {
			hot_item_new = debuginator__prev_visible_item(hot_item_new);
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
		bool toggle_by_default = debuginator->edit_types[hot_item->leaf.edit_type].toggle_by_default;
		if (toggle_and_activate && !toggle_by_default || !toggle_and_activate && toggle_by_default && !hot_item->leaf.is_expanded) {
			if (++hot_item->leaf.hot_index == hot_item->leaf.num_values) {
				hot_item->leaf.hot_index = 0;
			}
			debuginator_activate(debuginator, debuginator->hot_item, true);
		}
		else if (hot_item->leaf.is_expanded) {
			debuginator_activate(debuginator, debuginator->hot_item, true);
		}
		else {
			hot_item->leaf.is_expanded = true;
			debuginator__set_total_height(hot_item, debuginator->item_height * (hot_item->leaf.num_values) + debuginator->item_height); // for description, HACK! :(
		}
	}
	else {
		if (hot_item->folder.is_collapsed) {
			debuginator_set_collapsed(debuginator, hot_item, false);
		}

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
	if (!hot_item->is_folder && hot_item->leaf.is_expanded) {
		hot_item->leaf.is_expanded = false;
		debuginator__set_total_height(hot_item, debuginator->item_height);
	}
	else if (hot_item->parent != debuginator->root) {
		hot_item_new = debuginator->hot_item->parent;
	}

	if (hot_item != hot_item_new) {
		hot_item_new->parent->folder.hot_child = hot_item_new;
		debuginator->hot_item = hot_item_new;
	}
}

void debuginator_move_to_root(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator->hot_item;
	if (!hot_item->is_folder && hot_item->leaf.is_expanded) {
		hot_item->leaf.is_expanded = false;
		debuginator__set_total_height(hot_item, debuginator->item_height);
	}

	// We're now in the root, select the first child
	debuginator->hot_item = debuginator__first_visible_child(debuginator->root);
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
	DEBUGINATOR_memcpy(item->user_data, value, 1);
}

DebuginatorItem* debuginator_create_bool_item(TheDebuginator* debuginator, const char* path, const char* description, void* user_data) {
	bool value_before_creation = *(bool*)user_data;
	DEBUGINATOR_static_assert(sizeof(debuginator->bool_values[0]) == 1);
	DebuginatorItem* item = debuginator_create_array_item(debuginator, NULL, path,
		description, debuginator_copy_1byte, user_data,
		debuginator->bool_titles, debuginator->bool_values, 2, sizeof(debuginator->bool_values[0]));
	item->leaf.edit_type = DEBUGINATOR_EditTypeBoolean;

	if (value_before_creation == true) {
		item->leaf.default_index = 1;

		const char* item_setting = debuginator__get_item_setting(debuginator, path);
		if (DEBUGINATOR_strcmp(item_setting, "") == 0) {
			item->leaf.hot_index = 1;
			item->leaf.active_index = 1;
		}
	}

	return item;
}

DebuginatorItem* debuginator_create_bool_item_with_callback(TheDebuginator* debuginator, const char* path, const char* description, void* user_data, DebuginatorOnItemChangedCallback callback) {
	bool value_before_creation = *(bool*)user_data;
	DEBUGINATOR_static_assert(sizeof(debuginator->bool_values[0]) == 1);
	DebuginatorItem* item = debuginator_create_array_item(debuginator, NULL, path,
		description, callback, user_data,
		debuginator->bool_titles, debuginator->bool_values, 2, sizeof(debuginator->bool_values[0]));
	item->leaf.edit_type = DEBUGINATOR_EditTypeBoolean;

	if (value_before_creation == true) {
		item->leaf.default_index = 1;

		const char* item_setting = debuginator__get_item_setting(debuginator, path);
		if (DEBUGINATOR_strcmp(item_setting, "") == 0) {
			item->leaf.hot_index = 1;
			item->leaf.active_index = 1;
		}
	}

	return item;
}

static void debuginator__activate_preset(DebuginatorItem* item, void* value, const char* value_title, void* app_userdata) {
	(void)value_title;
	(void)app_userdata;
	(void)value;
	(void)item;
	const char** preset_value_titles = (const char**)item->leaf.values;
	TheDebuginator* debuginator = (TheDebuginator*)item->user_data;
	for (int i = 0; i < item->leaf.num_values; i++) {
		const char* path = item->leaf.value_titles[i];
		DebuginatorItem* item_to_activate = debuginator_get_item(debuginator, NULL, path, NULL);
		if (item_to_activate == NULL) {
			continue;
		}

		const char* preset_value_title = preset_value_titles[i];
		for (int value_i = 0; value_i < item_to_activate->leaf.num_values; value_i++) {
			if (DEBUGINATOR_strcmp(item_to_activate->leaf.value_titles[value_i], preset_value_title) == 0) {
				item_to_activate->leaf.hot_index = value_i;
				debuginator_activate(debuginator, item_to_activate, true);
				break;
			}
		}
	}

	item->leaf.active_index = 0;
}

DebuginatorItem* debuginator_create_preset_item(TheDebuginator* debuginator, const char* path, const char** paths, const char** value_titles, int** value_indices, int num_paths) {
	(void)value_indices; // TODO

	char description[2048] = { 0 };
	DEBUGINATOR_strcpy_s(description, 16, "Preset: \n");
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

		if ((int)(description_end - description) > sizeof(description) - DEBUGINATOR_MAX_PATH_LENGTH) {
			DEBUGINATOR_strcpy_s(description_end, 32, "[Preset description cropped]\n");
			description_end += sizeof("[Preset description cropped]\n");
			break;
		}
	}

	DEBUGINATOR_assert(description_end < description + sizeof(description));
	const char* owned_description = debuginator_copy_string(debuginator, description, (int)(description_end - description));
	DebuginatorItem* item = debuginator_create_array_item(debuginator, NULL, path,
		owned_description, debuginator__activate_preset, debuginator,
		paths, (void*)value_titles, num_paths, 0);

	item->leaf.edit_type = DEBUGINATOR_EditTypePreset;

	return item;
}

DebuginatorItem* debuginator_create_colorpicker_item(TheDebuginator* debuginator, const char* path, const char* description, DebuginatorOnItemChangedCallback on_item_changed_callback, void* user_data, DebuginatorColor* start_color) {
	DebuginatorColor* value_before_creation = start_color;
	DebuginatorColor* state = (DebuginatorColor*)debuginator__allocate(debuginator, sizeof(DebuginatorColor));
	*state = *start_color;
	DebuginatorItem* item = debuginator_create_array_item(debuginator, NULL, path,
		description, on_item_changed_callback, user_data,
		NULL, state, 0, 0);
	item->leaf.edit_type = DEBUGINATOR_EditTypeColorPicker;

	if (value_before_creation != NULL) {
		// TODO
	}

	return item;
}

static void debuginator_copy_float(DebuginatorItem* item, void* value, const char* value_title, void* app_userdata) {
	(void)value_title;
	(void)app_userdata;
	float* state = (float*)value;
	float src = state[3];
	float* dest = (float*)item->user_data;
	*dest = src;
}

DebuginatorItem* debuginator_create_numberrange_float_item(TheDebuginator* debuginator, const char* path, const char* description, float* user_data, float range_min, float range_max) {
	float value_before_creation = *user_data;
	float* state = (float*)debuginator__allocate(debuginator, sizeof(float) * 4);
	state[0] = range_min;
	state[1] = range_max;
	state[2] = value_before_creation; // default value
	state[3] = value_before_creation; // 'active' edited value
	// DEBUGINATOR_memcpy(state, range, sizeof(float) * range_count);
	DebuginatorItem* item = debuginator_create_array_item(debuginator, NULL, path,
		description, debuginator_copy_float, user_data,
		NULL, state, DEBUGINATOR_CUSTOM_VALUE_STATE_COUNT, 0);
	item->leaf.edit_type = DEBUGINATOR_EditTypeNumberRange;
	return item;
}

#endif // DEBUGINATOR_IMPLEMENTATION

#ifndef DEBUGINATOR_ENABLE_WARNINGS
#ifdef _MSC_VER
#pragma warning( pop )
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif
#endif // DEBUGINATOR_ENABLE_WARNINGS


#endif // INCLUDE_THE_DEBUGINATOR_H


/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Anders Elfgren
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
// clang-format on
