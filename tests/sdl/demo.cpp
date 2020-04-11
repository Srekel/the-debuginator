
#include "../../the_debuginator.h"

//#include <time.h>
#include <stdio.h>
#include <string.h>

#include "demo.h"

void demo_trigger_next(DemoData* data) {
	if (data->current_slide == -1) {
		data->current_slide = 0;

		for (int i=0; i < data->num_slides; ++i) {
			DemoSlide& slide = data->slides[i];
			slide.num_texts_visible = 0;
			memset(slide.anim_times, 0, sizeof(slide.anim_times));
		}

		DemoSlide& slide = data->slides[data->current_slide];
		slide.num_texts_visible++;

		return;
	}

	DemoSlide& slide = data->slides[data->current_slide];
	if (slide.num_texts_visible == slide.num_texts) {
		if (data->current_slide != data->num_slides - 1) {
			data->current_slide++;
			data->slides[data->current_slide].num_texts_visible++;
		}
		else {
			data->current_slide = -1;
		}
	}
	else {
		slide.num_texts_visible++;
	}
}
static void on_demo_action(DebuginatorItem* item, void* value, const char* value_title, void* app_userdata) {
	(void)app_userdata;
	(void)value;
	DemoData* data = (DemoData*)item->user_data;
	if (strcmp(value_title, "Start") == 0) {
		demo_trigger_next(data);
	}
	if (strcmp(value_title, "Stop") == 0) {
		data->current_slide = -1;
	}
	if (strcmp(value_title, "Next") == 0) {
		demo_trigger_next(data);
	}
	if (strcmp(value_title, "Prev") == 0) {
		if (data->current_slide >= 0) {
			data->current_slide -= 1;
		}
	}
}

static void debug_menu_setup(TheDebuginator* debuginator, DemoData* data) {
	(void)data;
	{
		static const char* string_titles[4] = { "Start", "Stop", "Prev", "Next" };
		debuginator_create_array_item(debuginator, NULL, "SDL Demo/Slide show",
			"Presentation demo", on_demo_action, data,
			string_titles, NULL, 4, 0);
		debuginator_set_edit_type(debuginator, "SDL Demo/Slide show", DEBUGINATOR_EditTypeActionArray);
	}
}

static DemoData s_demo_data;

static void presentation_setup(DemoData* data) {
	{
		DemoSlide& slide = data->slides[data->num_slides++];
		slide.texts[slide.num_texts++] = "Click to start slideshow!";
	}
	{
		DemoSlide& slide = data->slides[data->num_slides++];
		slide.texts[slide.num_texts++] = "What is it?";
		slide.texts[slide.num_texts++] = "- Open source C library";
		slide.texts[slide.num_texts++] = "- Debug menu for games";
		slide.texts[slide.num_texts++] = "- Feature rich";
	}
	{
		DemoSlide& slide = data->slides[data->num_slides++];
		slide.texts[slide.num_texts++] = "So what does it look like?";
		slide.texts[slide.num_texts++] = "- It's pretty sweet!";
		slide.texts[slide.num_texts++] = "- Press Right Arrow to open it!";
	}
	{
		DemoSlide& slide = data->slides[data->num_slides++];
		slide.texts[slide.num_texts++] = "Core UI features";
		slide.texts[slide.num_texts++] = "- Hierarchical";
		slide.texts[slide.num_texts++] = "  - Folders can be nested";
		slide.texts[slide.num_texts++] = "  - Folders can be collapsed";
		slide.texts[slide.num_texts++] = "- Scrollable (no limit on items)";
		slide.texts[slide.num_texts++] = "- Dynamic (add/remove/modify items live)";
		slide.texts[slide.num_texts++] = "- Mouse/Keyboard/Gamepad support";
	}
	{
		DemoSlide& slide = data->slides[data->num_slides++];
		slide.texts[slide.num_texts++] = "Anything else?";
		slide.texts[slide.num_texts++] = "...";
		slide.texts[slide.num_texts++] = "You betcha!";
	}
	{
		DemoSlide& slide = data->slides[data->num_slides++];
		slide.texts[slide.num_texts++] = "Themes!";
		slide.texts[slide.num_texts++] = "- Only Classic looks decent";
		slide.texts[slide.num_texts++] = "- Please help me!";
	}
	{
		DemoSlide& slide = data->slides[data->num_slides++];
		slide.texts[slide.num_texts++] = "Alignment!";
		slide.texts[slide.num_texts++] = "- Left or right";
	}
	{
		DemoSlide& slide = data->slides[data->num_slides++];
		slide.texts[slide.num_texts++] = "Search!";
		slide.texts[slide.num_texts++] = "- Press backspace to start";
		slide.texts[slide.num_texts++] = "- ... and to stop";
		slide.texts[slide.num_texts++] = "- Fuzzy matching!";
	}
	{
		DemoSlide& slide = data->slides[data->num_slides++];
		slide.texts[slide.num_texts++] = "Item editors!";
		slide.texts[slide.num_texts++] = "- Bool items";
		slide.texts[slide.num_texts++] = "- 'Action array' items";
		slide.texts[slide.num_texts++] = "- Support for custom ones";
	}
	{
		DemoSlide& slide = data->slides[data->num_slides++];
		slide.texts[slide.num_texts++] = "Presets!";
		slide.texts[slide.num_texts++] = "- Activate one item...";
		slide.texts[slide.num_texts++] = "- ... and affect multiple!";
	}
	{
		DemoSlide& slide = data->slides[data->num_slides++];
		slide.texts[slide.num_texts++] = "Hotkeys!";
		slide.texts[slide.num_texts++] = "- Bind input to specific items";
		slide.texts[slide.num_texts++] = "- Activate when menu is closed";
	}
	{
		DemoSlide& slide = data->slides[data->num_slides++];
		slide.texts[slide.num_texts++] = "Default and overriden values!";
		slide.texts[slide.num_texts++] = "- See highlighted colors.";
	}
	{
		DemoSlide& slide = data->slides[data->num_slides++];
		slide.texts[slide.num_texts++] = "Save & Load!";
		slide.texts[slide.num_texts++] = "- Remembers state.";
		slide.texts[slide.num_texts++] = "- Items & Folders.";
	}
	{
		DemoSlide& slide = data->slides[data->num_slides++];
		slide.texts[slide.num_texts++] = "That's it!";
		slide.texts[slide.num_texts++] = "- I'm open to...";
		slide.texts[slide.num_texts++] = "- ...suggestions.";
		slide.texts[slide.num_texts++] = "- ...feature requests.";
		slide.texts[slide.num_texts++] = "- ...bug reports.";
		slide.texts[slide.num_texts++] = "- @srekel";
		slide.texts[slide.num_texts++] = "- srekel@gmail.com";
		slide.texts[slide.num_texts++] = "- https://github.com/Srekel/the-debuginator/";
	}
}

DemoData* demo_init(GuiHandle gui, TheDebuginator* debuginator) {
	memset(&s_demo_data, 0, sizeof(s_demo_data));
	s_demo_data.gui = gui;
	s_demo_data.debuginator = debuginator;
	s_demo_data.current_slide = -1;
	s_demo_data.font_handle = gui_register_font_template(gui, "LiberationMono-Regular.ttf", 24);
	debug_menu_setup(debuginator, &s_demo_data);
	presentation_setup(&s_demo_data);
	demo_trigger_next(&s_demo_data);

	return &s_demo_data;
}

void demo_update(DemoData* data, float dt, Vector2 offset) {
	if (data->current_slide == -1) {
		return;
	}

	Vector2 pos = offset;
	pos.x += 50;
	DemoSlide& slide = data->slides[data->current_slide];
	for (int i=0; i < slide.num_texts_visible; ++i) {
		float& anim_time = slide.anim_times[i];
		anim_time += dt * 3;
		if (anim_time > 1.f) {
			anim_time = 1.f;
		}

		pos.x = 150.f + (-150.f + pos.x) * anim_time;
		Color color(150, 50, 150, 255);
		// unsigned char base_color = slide.num_texts_visible == slide.num_texts ? 100 : 50;
		unsigned char base_color = 50;
		color.a = (unsigned char)(anim_time * color.a);
		color.r = (unsigned char)(base_color + anim_time * color.r);
		color.g = (unsigned char)(base_color + anim_time * color.g);
		color.b = (unsigned char)(base_color + anim_time * color.b);
		gui_draw_text(data->gui, slide.texts[i], pos, data->font_handle, color);
		pos.y += 50;
	}

	unsigned char progress_bar_brightness = slide.num_texts_visible == slide.num_texts ? 250 : 150;
	Color progress_bar_background_color(100, 0, 100, 200);
	Color progress_bar_color(progress_bar_brightness, 0, progress_bar_brightness, progress_bar_brightness);
	Vector2 progress_bar_pos(offset.x + 50, offset.y + 20);
	Vector2 progress_bar_background_size(350, 5);
	Vector2 progress_bar_size(350.f * slide.num_texts_visible / slide.num_texts, 5);
	gui_draw_rect_filled(data->gui, progress_bar_pos, progress_bar_background_size, progress_bar_background_color);
	gui_draw_rect_filled(data->gui, progress_bar_pos, progress_bar_size, progress_bar_color);
}
