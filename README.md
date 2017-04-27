# The Debuginator

A juicy feature-packed debug menu intended for games.

It's conceptually based on the debug menu I wrote at Fatshark for **Warhammer: End Times – Vermintide**, where it was - and still is! - used extensively by everyone. Quote by one of the game designers during the Christmas party: *"The debug menu saved man-years for us"*. He was drunk. But not wrong.

Gifs will be added once project is slightly more complete.

## Current status

- **Highly work in progress.**
- Simple unit testing. Currently a bunch of them fails due to heavy refactoring.
- Reference implementation in SDL kept up to par with all features.
- Performs well enough in terms of CPU speed. 10k - 200k items is a breeze on my work station.
- Memory wise it's a bit worse. It's not designed to be cheap on memory (about 110 bytes + item title length per item), but I believe there's a bug in my allocator (design) that makes it fairly wasteful, so... I'll need to look into that.
- Only built and tested with VS2015 & VS2017.
- A small amount of technical debt stacked up. Will do a pass once things are more final.

## Features

These are subject to change and in various level of implementedness (including currently not at all).

- :heavy_check_mark: Done.
- :factory: Work in progress
- :small_blue_diamond: Some parts done, other parts on hold.
- :red_circle: Not started.

### :heavy_check_mark: Easy to use

See documentation further down.

Or check out the SDL demo for information on how to initialize the debuginator and for more advanced uses. 

### :heavy_check_mark: C99 compatible

So it's easy to add to any project. In one case I use an anonymous union (I pragma away the warning), so if your compiler doesn't support that, I recommend forking and fixing. I just like the convenience. 

### :heavy_check_mark: Built using maximum warning levels and warnings as errors, tested with Cppcheck

Because that's just polite to anyone who wants to use it.

I do use #pragma to ignore a silly warning (see above).

### :heavy_check_mark: Allocates nothing

Only the memory buffer passed in at creation.

### :heavy_check_mark: No dependencies

For the library that is.

A font is included, and to build the SDL demo you need SDL 2 and SDL TTF.

### :heavy_check_mark: STB-style single header library

Though tests and examples are in separate files/folders/projects.

### :heavy_check_mark: Scrollable

Can handle any number of items, the "hot" one will be centered (ish) smoothly.

### :heavy_check_mark: Performant

Tested with 10000 menu items with no noticable hit on my laptop running a debug build. That's good enough for me! Hey, it's a debug menu.

Though I will likely do some proper profiling and optimization later on.

### :heavy_check_mark: Search filter

Quickly and easily filter the items to find the one you want. Uses a fuzzy search mechanism to allow a user who isn't entirely sure what something is called to find it quickly. Adding a space to the search makes the filter run in "exact" mode; each part of the filter must be matched as-is.

Check my post on this for a bit of details about it: https://medium.com/@Srekel/implementing-a-fuzzy-search-algorithm-for-the-debuginator-cacc349e6c55

### :small_blue_diamond: Hierarchical

Put things in folders in folders. Support for expanding and collapsing folders.

### :small_blue_diamond: Save/Load of settings

So you start up with the settings you had when you exited. Simple interface, application needs to do most of the work.

### :small_blue_diamond: Input

Agnostic regarding Keyboard/Gamepad. It's handled at application layer, though guidelines for how to bind keys exist.

Mouse input coming later.

### :small_blue_diamond: Custom item editors

Different editors for different types of items, and support for users adding their own.

### :factory: Nice look & feel

Nice default color scheme, multiple themes to choose from, unique "editors" for different types of data, smooth animations, juicy feedback.

Yes, it's important.

### :factory: Reference implementation

Written in near-C C++ and SDL.

### :factory: Presets

Activate one item to activate a number of other ones.

### :red_circle: Accordiony

No matter how far down or deep you scroll, you can always see the folders above the current item.

### :red_circle: Favorites

Set your most used ones. They will show up in a special folder near the top.

### :red_circle: Hotkeys

Press a key when debug menu is active to assign the current item/value. Press it again when menu is closed to toggle/set the value.

## Can I help?

Sure. This is my first public single-header C library. Any helpful comments appreciated. PRs are not welcome yet as it's still early in development.

## License

Similarly to STB's single header libraries: The debuginator and other source files in this repository are in the public domain. You can do anything you want with them. You have no legal obligation to do anything else, although I appreciate attribution.

They are also licensed under the MIT open source license, if you have lawyers who are unhappy with public domain. Every source file includes an explicit dual-license for you to choose from.

Note that this does **NOT** include any folders that has LICENSE or README files that specifies their own license. They retain their own licenses.

# How to use the_debuginator.h

## Installation
Put the_debuginator.h somewhere in your project. It's an STB-style single header library and as such usage is a bit special.

Add this to *one* cpp file:

```C
#define DEBUGINATOR_IMPLEMENTATION
// #define DEBUGINATOR_OPTIONAL_SETTING_OR_OVERRIDE_X
// #define DEBUGINATOR_OPTIONAL_SETTING_OR_OVERRIDE_Y
// #define DEBUGINATOR_OPTIONAL_SETTING_OR_OVERRIDE_ETC
#include "path/to/the_debuginator.h"
```

## Setup

In the same cpp file:

```C
// Set up debuginator callbacks
void draw_text(const char* text, DebuginatorVector2* position, DebuginatorColor* color, DebuginatorFont* font, void* userdata) {
    // Your code goes here
}

void draw_rect(DebuginatorVector2* position, DebuginatorVector2* size, DebuginatorColor* color, void* userdata) {
    // Your code goes here
}

void word_wrap(const char* text, DebuginatorFont font, float max_width, unsigned* row_count, unsigned* row_lengths, int row_lengths_buffer_size, void* app_userdata) {
    // Your code goes here
}

void word_wrap2(const char* text, DebuginatorFont font, float max_width, char** buffer, int buffer_size, void* userdata) {
    // Your code goes here
}

DebuginatorVector2 text_size(const char* text, DebuginatorFont* font, void* userdata) {
    // Your code goes here
}

int main(...) {
    TheDebuginatorConfig config;
    debuginator_get_default_config(&config);
    
    config.memory_arena_capacity = 1024 * 512;
    config.memory_arena = (char*)malloc(config.memory_arena_capacity);

    config.draw_rect = draw_rect;
    config.draw_text = draw_text;
    config.word_wrap = word_wrap;
    config.text_size = text_size;
    
    config.size.x = 500;
    config.size.y = GetMyScreenResolution().y; // You need to calculate this.
    
    config.screen_resolution.x = config.size.x; // Yes these are temporary and will be removed.
    config.screen_resolution.y = config.size.y;
    
    // There's a bunch of other things you CAN change in the config, but these things
    // are the necessary stuff.
    
    config.create_default_debuginator_items = true; // I recommend this
    
    TheDebuginator debuginator;
    debuginator_create(&config, &debuginator);
    
    // debuginator is now the thing that you pass into the API functions to update/change/draw The Debuginator.
}
```

Actual usage, such as adding items or modifying settings, can be done from any c file.

## How to use

### The gist of it 
This is the core for creating an item:

```C
DebuginatorItem* debuginator_create_array_item(TheDebuginator* debuginator,
	DebuginatorItem* parent, const char* path, const char* description,
	DebuginatorOnItemChangedCallback on_item_changed_callback, void* user_data,
	const char** value_titles, void* values, int num_values, int value_size) 
```

Each *item* is defined by its path. If you create two items with the same path, only one will actually exist - with the data from the last call. 

An *leaf item* has a list of *values*. A value has a title and a... value. The Debuginator doesn't care about what the values are, it just sees them as an array; a pointer and an element size. When you activate a value on an item, you'll get a callback with the value pointing to the correct place in that array. For bools, you don't really need to care about that stuff, it's handled by the wrapper function.

The leaf item also has a userdata field that you will use in your callbacks.

In addition to leaf items, there are folder items which currently doesn't really do anything in particular except be there. You don't need to create folder items before items, they'll be created implicitly if they don't already exist. You can pass NULL to the *parent* parameter, in fact, it's the most common use case. It's mainly there as an optimization. 

### Saving and loading

I recommend looking at the SDL demo for a good example of how to do this. But here's how it works.

Saving is fairly straightforward. Call debuginator_save and pass in a callback that gets called for each item who's value is different from the default. I recommend storing it to a single key-value map, like "MyGame/MySetting = True".

Loading uses a bit of a trick inside The Debuginator. Remember how I said you can use the same path twice when creating an item, and it'll just overwrite it the second time? Here's the trick: Call debuginator_load_item. It takes a path and a value (which should match the title of one of the values of the item). If the item doesn't exist, it'll create one with no data, so it won't be visible.. except it will store the value inside the *description* field. Once the item is created *for reals*, it'll look to see if there's something already there, stored in the description field, and if so, use that to set the hot/active index.

### Examples

Here's how to add a boolean item that toggles god mode for the player:

```C
    struct Player {
        int health;
        bool godmode;
    }
    
    Player my_player;
    my_player.health = 100;
    my_player.godmode = false;
    
    debuginator_create_bool_item(
      &debuginator,
      "Player Mechanics/God Mode",
      "Player is invincible if enabled",
      &my_player.godmode);
```

Here's how to create a preset item - it'll toggle multiple things.

```C
    const char* preset_paths[2] = { "Workflow/Skip intro", "Player Mechanics/God Mode" };
    const char* preset_value_titles[2] = { "True", "True" };
    debuginator_create_preset_item(&debuginator, 
    		"My Presets/Good workflow", 
		preset_paths, preset_value_titles, NULL, 2);
```

Here's how to create a generic item with a few strings and no callback (not sure what use case there is for that, but hey)... If you look at the definition of the other create_item functions, you'll see that they simply wrap this one.

```C
	{
		static const char* string_titles[5] = { "String A", "String B", "String C", "String D", "String E" };
		debuginator_create_array_item(&debuginator, 
			NULL, "My Game/String Test",
			"Multiple strings.", NULL, NULL,
			string_titles, NULL, 5, 0);
	}
```

Here's how to create a generic item with multiple things WITH a callback.

```C
	void on_change_ui_size(DebuginatorItem* item, void* value, const char* value_title, void* app_userdata) {
		(void)value_title;
		TheDebuginatorWrapper* wrapper = (TheDebuginatorWrapper*)app_userdata;
		int size_category = *(int*)value;
		if (size_category == 0) {
			set_ui_size(&wrapper->debuginator, 14, 22);
		}
		else if (size_category == 1) {
			set_ui_size(&wrapper->debuginator, 20, 30);
		}
		else if (size_category == 2) {
			set_ui_size(&wrapper->debuginator, 32, 40);
		}
		else if (size_category == 3) {
			set_ui_size(&wrapper->debuginator, 64, 70);
		}
	}

// Later...

		static const char* uisize_titles[4] = { "Small", "Medium", "Large", "ULTRA LARGE" };
		static int uisizes[4] = { 0, 1, 2, 3 };
		DebuginatorItem* uisize_item = debuginator_create_array_item(debuginator, 
			NULL, "Debuginator/UI size",
			"Change font and item size.", on_change_ui_size, wrapper,
			uisize_titles, uisizes, 4, sizeof(uisizes[0]));
```

## How to configure

You know, it's best to just look in the header file and see which functions are exposed, but... here's the API such as it is currently.

```C
bool debuginator_is_open(TheDebuginator* debuginator);
void debuginator_set_open(TheDebuginator* debuginator, bool open);

DebuginatorItem* debuginator_create_array_item(TheDebuginator* debuginator,
	DebuginatorItem* parent, const char* path, const char* description,
	DebuginatorOnItemChangedCallback on_item_changed_callback, void* user_data,
	const char** value_titles, void* values, int num_values, int value_size);

DebuginatorItem* debuginator_create_bool_item(TheDebuginator* debuginator, const char* path, const char* description, void* user_data);
DebuginatorItem* debuginator_create_preset_item(TheDebuginator* debuginator, const char* path, const char** paths, const char** value_titles, int** value_indices, int num_paths);

DebuginatorItem* debuginator_new_folder_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* title, int title_length);
DebuginatorItem* debuginator_get_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* path, bool create_if_not_exist);
void debuginator_set_hot_item(TheDebuginator* debuginator, const char* path);
DebuginatorItem* debuginator_get_hot_item(TheDebuginator* debuginator);
void debuginator_remove_item(TheDebuginator* debuginator, DebuginatorItem* item);
void debuginator_remove_item_by_path(TheDebuginator* debuginator, const char* path);

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

void debuginator_set_item_height(TheDebuginator* debuginator, int item_height);
void debuginator_set_size(TheDebuginator* debuginator, int width, int height);

```


# How to use Stingray Plugin

## Installation

Add it to your project and build it. Currently it just builds as static library, sorry about that.

## Setup

Somewhere in your project's code:

```C
	PluginManagerApi *plugin_manager_api = (PluginManagerApi*)get_engine_api(PLUGIN_MANAGER_API_ID);
	TheDebuginatorApi* debuginator_api = (TheDebuginatorApi*)plugin_manager_api->get_next_plugin_api(THE_DEBUGINATOR_API_ID, NULL);
	ScriptApi* script_api = (ScriptApi*)get_engine_api(C_API_ID);
	
	TheDebuginatorConfig config;
	ASSERT(debuginator_api->get_default_config != NULL, "Debugintor plugin not loaded.");
	debuginator_api->get_default_config(&config);
	
	config.memory_arena_capacity = 1024 * 32;
	config.size.x = 500;

	DebuginatorPluginCreateContext create_context = { 0 };
	create_context.config = config;
	create_context.gui = gui;
	create_context.font = hash64("core/performance_hud/debug");
	create_context.font_material = script_api->Gui->material(my_gui, create_context.font);

	debuginator_api->create_debuginator(NULL, &create_context);
```

## Update

```C
	void update(float dt) {
		TheDebuginator* debuginator = debuginator_api->get_debuginator(NULL);
		int devices = Debuginator_Keyboard | Debuginator_Gamepad;
		debuginator_api->handle_default_input(debuginator, devices);
	}
```
## That's it?

That's it. Basically. The plugin automatically handles a few things for you. You don't have to call handle_default_input if you want to implement your own control scheme, and you can edit the config and create context however you wish.
