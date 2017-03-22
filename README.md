# The Debuginator

A juicy feature-packed debug menu intended for games.

It's conceptually based on the debug menu I wrote at Fatshark for **Warhammer: End Times – Vermintide**, where it was - and still is! - used extensively by everyone. Quote by one of the game designers during the Christmas party: *"The debug menu saved man-years for us"*. He was drunk. But not wrong.

Gifs will be added once project is slightly more complete.

## Current status

- **Highly work in progress.**
- Simple unit testing. Currently a bunch of them fails due to heavy refactoring.
- Reference implementation in SDL.
- Working implementation with all core features.
- Basic animation support.
- Not performance tested, but trivially optimized (only draws what's on screen).
- Only built and tested with VS2015.

## Features

These are subject to change and in various level of implementedness (including currently not at all).

- :heavy_check_mark: Done.
- :factory: Work in progress
- :small_blue_diamond: Some parts done, other parts on hold.
- :red_circle: Not started.

### :factory: Easy to use

```C
    bool player_godmode = false;
    debuginator_create_bool_item(
      &debuginator,
      "Player Mechanics/God Mode",
      "Player is invincible if enabled",
      &player_godmode);
```

See SDL demo for information on how to initialize the debuginator and for more advanced uses. (Proper documentation to be written)

### :heavy_check_mark: C99 compatible

So it's easy to add to any project.

### :heavy_check_mark: Built using maximum warning levels and warnings as errors, tested with Cppcheck

Because that's just polite to anyone who wants to use it.

I do occasionally use pragma to ignore silly warnings.

### :heavy_check_mark: Allocates nothing

Only the memory buffer passed in at creation.

### :heavy_check_mark: No dependencies

For the demo and unit test projects I have a dependency and may add more in the future if it seems like a good idea. I'm less concerned about keeping those "clean" as they aren't a part of the library itself.

### :heavy_check_mark: STB-style single header library

Though tests and examples are in separate files/folders/projects.

### :heavy_check_mark: Scrollable

Can handle any number of items, the "hot" one will be centered (ish) smoothly.

### :heavy_check_mark: Performant

Tested with 250000 menu items with no noticable hit. That's good enough for me! Hey, it's a debug menu.

Though I will likely do some proper profiling and optimization later on.

### :small_blue_diamond: Hierarchical

Put things in folders in folders. Support for expanding and collapsing folders.

### :small_blue_diamond: Save/Load of settings

So you start up with the settings you had when you exited. Simple interface, application needs to do most of the work.

### :factory: Nice look & feel

Nice default color scheme, multiple themes to choose from, unique "editors" for different types of data, smooth animations, juicy feedback.

Yes, it's important.

### :factory: Custom item editors

Different editors for different types of items, and support for users adding their own.

### :small_blue_diamond: Filter

Quickly and easily filter the items to find the one you want.

### :small_blue_diamond: Input

Agnostic regarding Keyboard/Gamepad. It's handled at application layer, though guidelines for how to bind keys exist.

Mouse input coming later.

### :factory: Reference implementation

Written in near-C C++ and SDL.

### :red_circle: Accordiony

No matter how far down or deep you scroll, you can always see the folders above the current item.

### :red_circle: Presets

Activate one item to activate a number of other ones.

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


