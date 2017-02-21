# The Debuginator

A super sweet hierarchical scrollable accordion debug menu intended for games.

It's onceptually based on the one I wrote at Fatshark for Warhammer End Times: Vermintide, where it was used extensively by everyone. Quote by one of the designers during the Christmas party: "The debug menu saved man-years for us".

## Current status

- Highly work in progress.
- Simple unit testing.
  - 53 "tests", all passes.
- Beginning of reference implementation in SDL.

## Features

These are subject to change and in various level of implementedness (including currently not at all).

### Easy to use

    bool player_godmode = false;
    DebuginatorItem item_buffer[16];
    TheDebuginator debuginator = debuginator_create(item_buffer, sizeof(item_buffer) / sizeof(item_buffer[0]));
    debuginator_create_bool_item(&debuginator, "Player Mechanics/Godmode", "Player is invincible if enabled", &player_godmode);
    debuginator_initialize(&debuginator); // I'd optimally like to get rid of this.

    DebuginatorInput input = {0};
    input.go_sibling_down = true;
    debug_menu_handle_input(&debuginator, &input);

### C99 compatible

For maximum compatibility (except for C89 I guess but I can't be arsed).

### Built using maximum error levels and warnings as errors, tested with Cppcheck

Because that's just polite to anyone who wants to use it.

I do occasionally use pragma to ignore silly warnings.

### Allocates nothing

Only the memory buffer passed in at creation. May change in the future to accept an allocator.

### No dependencies

Will possibly use @incrediblejr handle vector if needed in future.

### Single header

Though tests and examples are in separate projects.

### Scrollable

Can handle any number of items, the "hot" one will be centered (ish) smoothly.

### Hierarchical

Put things in folders in folders. Support for expanding and collapsing folders and items coming soon.

### Accordiony

No matter how far down or deep you scroll, you can always see the folders above the current item.

### Looks sweet

Nice default color scheme, smooth animations, juicy feedback.

Yes, it's important.

### Filter

Quickly and easily filter the items to find the one you want.

### Presets

Activate one item to activate a number of other ones.

### Favorites

Set your most used ones. They will show up in a special folder near the top.

### Hotkeys

Press a key when debug menu is active to assign the current item/value. Press it again when menu is closed to toggle/set the value.

### Reference implementation

Written in SDL, roughly based on Stingray's Gui interface.

### Save/Load of settings

So you start up with the settings you had when you exited.