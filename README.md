# The Debuginator

A juicy feature-packed debug menu intended for games.

It's conceptually based on the debug menu I wrote at Fatshark for **Warhammer: End Times – Vermintide**, where it was - and still is! - used extensively by everyone. Quote by one of the game designers during the Christmas party: *"The debug menu saved man-years for us"*. He was drunk. But not wrong.

The API is designed to have what I think is just the right amount of responsibilities. The application reads input and uses that to control the menu, and the menu sends back what it wants the application to draw.

It's nearly feature complete now - unless someone has any cool requests - and as far as I know, bug free.

I'd really like to add lots of nice GIFs but they get quite large and I'm not keen on forcing people to download megabytes just to view a GitHub project's README. So here's just one with a low resolution and framerate:

<a href="https://www.youtube.com/watch?v=QoVHe_0quQ4" target="_blank"><img src="debuginator.gif"
alt="GIF demo. Click to go to youtube demo!" /></a>

There's a fancier demo on Youtube: https://youtu.be/8lA5HQik2wo

## Features

These are subject to change and in various level of implementedness (including currently not at all).

- :heavy_check_mark: Done.
- :factory: Work in progress
- :small_blue_diamond: Some parts done, other parts on hold.
- :red_circle: Not started.

### :heavy_check_mark: STB-style single header library

Though tests, examples and plugins are in separate files/folders/projects.

### :heavy_check_mark: Batteries included

Think of the_debuginator.h less as a library and more as a *thing* you plug in to get a nice, juicy, feature-packed debug menu.

Where it makes sense, I've added functionality for extending the menu with your own custom features.

### :heavy_check_mark: Politely coded

It's written in **C99**, to make it easy to add to any project on any platform. In one case I use an anonymous union (I pragma away the warning), so if your compiler doesn't support that, I recommend forking and fixing. I just like the convenience.

**No globals or static variables**, in case you want to have it in a plugin and reload (or instantiate more than one!).

**Built using maximum warning levels and warnings as errors**. I use a few #pragmas to ignore warnings I don't think is problematic. Define `DEBUGINATOR_ENABLE_WARNINGS` if you want to handle that manually.

**Statically analyzed with Cppcheck.** Again, I suppress a few issues that I think aren't problematic. You can check `the-debuginator_solution_suppressions.cfg` if you're interested.

**It doesn't allocate any memory.** You pass in a buffer at creation and that's what it'll use internally.

**The library has no dependencies.** It will include some standard headers but only if you don't provide overrides. (For the SDL reference demo, a font is included, and to build  you need SDL 2 and SDL TTF.)

**Multi-platform.** Tested on MSVC 2017/Windows 10 and on the PS4 (so I've been told), and builds on Clang/Linux.

### :heavy_check_mark: Scrollable

Can handle any number of items, the "hot" one will be centered (ish) smoothly.

Also supports scrolling without changing the hot item, for example using the mouse wheel, touch, or a gamepad stick.

### :heavy_check_mark: Performant

Tested with 10000 menu items with no noticable hit on my laptop running a debug build. That's good enough for me! Hey, it's a debug menu.

Memory-wise I think you can estimate about 200 bytes, maybe 300, for a typical item in the menu. I've opted to add features, improve speed, and make the code easier to read, at the cost of memory usage. It wouldn't be too hard to optimize it a bit in this regard if you're strained for memory, but it's left as an exercise for the reader.

Btw, 200 bytes doesn't sound like much, but if you do have 10000 items - perhaps a few for each enemy and item spawned in your level - that means you'll need to spare 2mb of memory, which could potentially be an issue on consoles, phones, Raspberries, and so on. Just bear it in mind.

### :heavy_check_mark: Search filter

Quickly and easily filter the items to find the one you want. Uses a fuzzy search mechanism to allow a user who isn't entirely sure what something is called to find it quickly. Adding a space to the search makes the filter run in "exact" mode; each part of the filter must be matched as-is.

Check my post on this for a bit of details about it: https://medium.com/@Srekel/implementing-a-fuzzy-search-algorithm-for-the-debuginator-cacc349e6c55

### :heavy_check_mark: Save/Load of settings

So you start up with the settings you had when you exited. Simple interface, application needs to handle the actual I/O.

### :heavy_check_mark: Dynamic add and remove of items

So you could, for example, have items that are only available when you are in the game's main menu, or have one item for each currently spawned enemy in the game.

### :heavy_check_mark: Left or right aligned

Because some games already have other important stuff on the left side of the screen.

Hey, you could even have one instance of The Debuginator on the left and another on the right.

### :heavy_check_mark: Hierarchical

Put things in folders in folders in folders. They can be expanded and collapsed.

Folder states are persisted automatically if you implement the save interface.
This is useful when you have a lot of items but different users are only interested in different subsets of them.

### :heavy_check_mark: Custom item editors

Different editors for different types of items, and support for users adding their own.

### :heavy_check_mark: Input

The API for manouvering the menu is agnostic regarding keyboard/gamepad. It's handled at application layer, though guidelines for how to bind keys exist - see the demo.

Also supports mouse and touch input.

### :heavy_check_mark: Presets

Activate one item to activate a number of other ones. Useful for if a user tends to reset her debug settings but has a lot of them she often wants to enable. Or if your QA should play under certain circumstances. Or if you have a few settings that is just really nice to always have enabled, for new devs.

### :heavy_check_mark: Hotkeys

Press a key when debug menu is active to assign the current item/value. Press it again when the menu is closed to toggle/set the value.

Hot keys are persisted automatically if you implement the save interface.

### :heavy_check_mark: Default values

Different colors for when the current value of an item is different from it's defined *default value* makes it clearer to the developer that she is or isn't  working under the game's standard conditions.

### :heavy_check_mark: Unit tests

I used these extensively in the beginning of the project but the fun and usefulness of keeping them up to date is gone. They are surpassed by the SDL reference demo. I'm conflicted as whether to mark it as a done feature but yeah, for all intents and purposes, they are done.

### :heavy_check_mark: Nice look & feel

Nice default color scheme, multiple themes to choose from, unique "editors" for different types of data, smooth animations, juicy feedback.

Yes, it's important.

### :heavy_check_mark: Reference implementation

Written in near-C C++ and SDL.

There's also a plugin for Autodesk Stingray. Since Stingray is basically dead I'm not likely to work further on the plugin, but it's useful as another reference implementation.

### :factory: Thread safe

Not really - I don't want to add multithreading constructs to the_debuginator - it's up to the application to ensure that no thread writes or reads to it while another thread is writing. While I don't intend to change that, I **am** working on a way to make it easier to use in a multithreaded environment (that's how our own game at Warpzone Studios is engineered) but it's not done yet.

### :red_circle: Accordiony

The idea is that, no matter how far down or deep you scroll, you can always see the folders above the current item.

### :red_circle: Favorites

Set your most used ones. They will show up in a special folder near the top.

## Can I help?

Sure. Any helpful comments, bug reports, or PRs appreciated.

Even though I think The Debuginator has most of the features I envision for it, have probably missed something, so I would love feature requests!

## License

Similarly to STB's single header libraries: The Debuginator and other source files in this repository are in the public domain. You can do anything you want with them. You have no legal obligation to do anything else, although I appreciate attribution.

They are also licensed under the MIT open source license, if you have lawyers who are unhappy with public domain. Every source file includes an explicit dual-license for you to choose from.

Note that this does **NOT** include any folders that has LICENSE or README files that specifies their own license. They retain their own licenses.

# How to use the_debuginator.h

I really recommend looking at the SDL demo for a real use case on how to set it up and use The Debuginator. It also includes basic save/load which is something you *really* want for your debug menu! :)

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

    config.screen_resolution.x = config.size.x;
    config.screen_resolution.y = config.size.y;

    // There's a bunch of other things you CAN change in the config, but these things
    // are the necessary stuff.

    config.create_default_debuginator_items = true; // I recommend this

    TheDebuginator debuginator;
    debuginator_create(&config, &debuginator);

    // debuginator is now the thing that you pass into the API functions to update/change/draw The Debuginator.
}
```

Actual usage, such as adding items or modifying settings, can be done from any C or C++ file.

## How to use

### The gist of it
This is the core for creating an item:

```C
DebuginatorItem* debuginator_create_array_item(TheDebuginator* debuginator,
	DebuginatorItem* parent, const char* path, const char* description,
	DebuginatorOnItemChangedCallback on_item_changed_callback, void* user_data,
	const char** value_titles, void* values, int num_values, int value_size)
```

Don't be alarmed - there are utility functions that make it easier to use! :) See below.

Each *item* is defined by its path. If you create two items with the same path, only one will actually exist - with the data from the last call.

An *leaf item* has a list of *values*. A value has a title and a... value. The Debuginator doesn't care about what the values are, it just sees them as an array; a pointer and an element size. When you activate a value on an item, you'll get a callback with the value pointing to the correct place in that array. For bools, you don't really need to care about that stuff, it's handled by the wrapper function.

The leaf item also has a userdata field that you will use in your callbacks.

In addition to leaf items, there are folder items which currently doesn't really do anything in particular except be there. You don't need to create folder items before items, they'll be created implicitly if they don't already exist. You can pass NULL to the *parent* parameter, in fact, it's the most common use case. It's mainly there as an optimization.

### Saving and loading

I recommend looking at the SDL demo for a good example of how to do this. But here's how it works.

Saving is fairly straightforward. Call debuginator_save and pass in a callback that gets called for each item who's value is different from the default. I recommend storing it to a single key-value map, like "MyGame/MySetting = True".

Folders will also save their state if they are collapsed. This is so that they remain collapsed if you for example close the game down and open it up again.

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

## A note on memory

The Debuginator uses (what I call) a block allocator. It's slightly wasteful in terms of memory but should be pretty efficient for allocating and deallocating.

You provide a buffer for The Debuginator to use, and it'll use that. When there's no more memory.. it'll probably crash or something.

If you want to give The Debuginator a string for it to own (and deallocate), you can do that. Look at:
```C
char* debuginator_copy_string(TheDebuginator* debuginator, const char* string, int length);
```

## API

You know, it's best to just look in the header file and see which functions are exposed, but... here's the API such as it is currently. There's additional information in the code.

```C
bool debuginator_is_open(TheDebuginator* debuginator);
void debuginator_set_open(TheDebuginator* debuginator, bool open);

DebuginatorItem* debuginator_create_array_item(TheDebuginator* debuginator,
	DebuginatorItem* parent, const char* path, const char* description,
	DebuginatorOnItemChangedCallback on_item_changed_callback, void* user_data,
	const char** value_titles, void* values, int num_values, int value_size);

DebuginatorItem* debuginator_create_bool_item(TheDebuginator* debuginator, const char* path, const char* description, void* user_data);
DebuginatorItem* debuginator_create_preset_item(TheDebuginator* debuginator, const char* path, const char** paths, const char** value_titles, int** value_indices, int num_paths);

DebuginatorItem* debuginator_create_folder_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* title, int title_length);
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

# How to run the Unit Test

Open the Visual Studio solution. Build it. Set the *unittest* project as the StartUp project. Run.

Note: The SDL demo project won't build until you fix the dependencies so you might want to unload it.
Note: The unit tests haven't been updated in a long while so you probably don't want to do this.

# How to run the SDL demo

Open the Visual Studio solution. Build it. Set the *sdl* project as the StartUp project. Run.

You'll need dependencies to build it. Get SDL 2 and SDLTTF 2. I'll update this with better instructions at some point. :)

# How to use Stingray Plugin

Not supported anymore (I mean, technically it should work with little problems but Stingray isn't available to the public any more so you are almost certainly not using it).
