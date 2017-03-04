#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <time.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "gui.h"

static const int WIDTH = 500;

static void unittest_debuginator_assert(bool test) {
	if (!test) {
	}
	assert(test);
	//DebugBreak();
}

#define DEBUGINATOR_assert unittest_debuginator_assert
#define ASSERT unittest_debuginator_assert

struct DebuginatorItem;

struct DrawData {
	int color;
};
//
//void debuginator__default_quick_draw(DebuginatorItem* item, void* data) {
//	if (item->leaf.num_values > 0) {
//		//Vector2 value_offset = *(Vector2*)position;
//		//value_offset.x = 300 + debuginator->openness * 500 - 500;
//		draw_text(gui, item->leaf.value_titles[item->leaf.active_index], *(Vector2*)position, s_fonts[FONT_ItemTitle], s_theme[color]);
//	}
//}


#include "../../the_debuginator.h"
//typedef DebuginatorVector2 Vector2;


struct GameBox {
	GameBox() {
	}
	void randomize() {
		pos.x = pos.y = 0;
		pos.x = pos.y = 0;
		size.x = 50 + 100 * ((float)rand()) / RAND_MAX;
		size.y = 50 + 100 * ((float)rand()) / RAND_MAX;
		velocity.x = 5 + 50 * ((float)rand()) / RAND_MAX;
		velocity.y = 5 + 50 * ((float)rand()) / RAND_MAX;
		color.r = 50 + 100 * ((float)rand()) / RAND_MAX;
		color.g = 50 + 100 * ((float)rand()) / RAND_MAX;
		color.b = 50 + 100 * ((float)rand()) / RAND_MAX;
		color.a = 150 + 100 * ((float)rand()) / RAND_MAX;
	}
	Vector2 pos;
	Vector2 size;
	Vector2 velocity;
	Color color;
};

struct GameData {
	bool mybool;
	bool draw_boxes;
	char mystring[256];
	GameBox boxes[256];
	int boxes_n;
	char box_string[32];
};

static FontTemplateHandle s_fonts[16];
enum FontTemplates {
	FONT_ItemTitle,
	FONT_ItemDescription,
	FONT_count
};

static int s_theme_index = 0;
static Color s_theme[16];
static Color s_themes[3][16];
enum ColorTemplates {
	COLOR_Background,
	COLOR_FolderTitle,
	COLOR_ItemTitle,
	COLOR_ItemTitleOverridden,
	COLOR_ItemTitleHot,
	COLOR_ItemTitleActive,
	COLOR_ItemDescription,
	COLOR_LineHighlight,
	COLOR_ItemValueDefault,
	COLOR_ItemValueOverridden,
	COLOR_ItemValueHot,
	COLOR_NumColors
};

static bool theme_setup(GuiHandle gui) {
	memset(s_fonts, 0, sizeof(*s_fonts));
	s_fonts[FONT_ItemTitle] = gui_register_font_template(gui, "LiberationMono-Regular.ttf", 18);
	s_fonts[FONT_ItemDescription] = gui_register_font_template(gui, "LiberationSerif-Italic.ttf", 18);

	for (size_t i = 0; i < FONT_count; i++) {
		if (s_fonts[i] == 0) {
			return false;
		}
	}

	return true;
}

void draw_text(const char* text, DebuginatorVector2* position, DebuginatorColor* color, DebuginatorFont* font, void* userdata) {
	GuiHandle gui = (GuiHandle)userdata;
	int color_index = font->italic ? (int)FONT_ItemDescription : (int)FONT_ItemTitle;
	gui_draw_text(gui, text, *(Vector2*)position, s_fonts[color_index], *(Color*)color);
}

void draw_rect(DebuginatorVector2 position, DebuginatorVector2 size, DebuginatorColor color, void* userdata) {
	gui_draw_rect_filled((GuiHandle)userdata, *(Vector2*)&position, *(Vector2*)&size, *(Color*)&color);
}

const char* word_wrap(const char* text, DebuginatorFont font, float max_width, char* buffer, int buffer_size, void* userdata) {
	return gui_word_wrap((GuiHandle)userdata, s_fonts[font.italic ? FONT_ItemDescription : FONT_ItemTitle], text, (int)max_width, buffer, buffer_size);
}


static void on_change_theme(DebuginatorItem* item, void* value, const char* value_title) {
	s_theme_index = *(int*)value;
	memcpy(s_theme, s_themes[s_theme_index], sizeof(s_theme));
}

static void on_boxes_activated(DebuginatorItem* item, void* value, const char* value_title) {
	GameData* data = (GameData*)item->user_data;
	if (strcmp(value_title, "Add box") == 0 && data->boxes_n < 256) {
		GameBox* box = &data->boxes[data->boxes_n++];
		box->randomize();
	}
	else if (strcmp(value_title, "Clear all boxes") == 0 && data->boxes_n < 256) {
		data->boxes_n = 0;
	}

	sprintf_s(data->box_string, "Box count: %d", data->boxes_n);
}

static void debug_menu_setup(TheDebuginator* debuginator, GameData* data) {

	{
		static int theme_indices[3] = { 0, 1, 2 };
		static const char* string_titles[3] = { "Classic", "Blue", "Gray" };
		debuginator_create_array_item(debuginator, NULL, "Debuginator/Help",
			"The Debuginator is a debug menu. With a keyboard, you open it with Right Arrow and close it with Left Arrow. You use those keys, plus Up/Down arrows to navigate. Right Arrow is also used to change value on a menu item.", NULL, NULL,
			NULL, NULL, 0, 0);
		debuginator_create_array_item(debuginator, NULL, "Debuginator/Theme",
			"Change color theme of The Debuginator.", on_change_theme, NULL,
			string_titles, (void*)theme_indices, 3, sizeof(theme_indices[0]));
	}
	{
		static const char* string_titles[5] = { "String A", "String B", "String C", "String D", "String E" };
		debuginator_create_array_item(debuginator, NULL, "Debuginator/Help",
			"The Debuginator is a debug menu. With a keyboard, you open it with Right Arrow and close it with Left Arrow. You use those keys, plus Up/Down arrows to navigate. Right Arrow is also used to change value on a menu item.", NULL, NULL,
			NULL, NULL, 0, 0);
		debuginator_create_array_item(debuginator, NULL, "Debuginator/String Test",
			"Change color theme of The Debuginator.", NULL, NULL,
			string_titles, NULL, 5, 0);
	}
	debuginator_create_bool_item(debuginator, "SDL Demo/Draw boxes", "Whether to draw the animated boxes or not.", &data->draw_boxes);

	{
		static const char* string_titles[3] = { "Clear all boxes", "Add box", data->box_string };
		debuginator_create_array_item(debuginator, NULL, "SDL Demo/Boxes",
			"Box things", on_boxes_activated, data,
			string_titles, NULL, 3, 0);	}

	debuginator_create_bool_item(debuginator, "SimpleBool 1", "Change a bool.", &data->mybool);
	debuginator_create_bool_item(debuginator, "Folder/SimpleBool 2", "Change a bool.", &data->mybool);
	debuginator_create_bool_item(debuginator, "Folder/SimpleBool 3", "Change a bool.", &data->mybool);
	debuginator_create_bool_item(debuginator, "Folder/Subfolder/SimpleBool 4 with a really long long title", "Change a bool.", &data->mybool);

	debuginator_new_folder_item(debuginator, NULL, "Folder 2", 0);

	debuginator_initialize(debuginator);
}

struct QuickDrawDefaultData {
	QuickDrawDefaultData(int font, int color) : font_index(font), color_index(color) {}
	int font_index;
	int color_index;
};

bool distance_to_hot_item(DebuginatorItem* item, DebuginatorItem* hot_item, float* distance) {
	if (item == hot_item) {
		return true;
	}

	*distance += 30;
	if (item->is_folder) {
		DebuginatorItem* child = item->folder.first_child;
		while (child) {
			bool found = distance_to_hot_item(child, hot_item, distance);
			if (found) {
				return true;
			}

			child = child->next_sibling;
		}
	}

	return false;
}

int main(int argc, char **argv)
{
	(void)(argc, argv);

	int res_x = 800;
	int res_y = 600;
	GuiHandle gui = gui_create_gui(res_x, res_y, "Debuginator SDL demo");
	if (gui == 0) {
		return 1;
	}

	if (!theme_setup(gui)) {
		gui_destroy_gui(gui);
		return 1;
	}

	DebuginatorItem item_buffer[256];
	TheDebuginatorConfig config;
	debuginator_get_default_config(&config);
	config.item_buffer = item_buffer;
	config.item_buffer_capacity = sizeof(item_buffer) / sizeof(item_buffer[0]);
	config.draw_rect = draw_rect;
	config.draw_text = draw_text;
	config.draw_user_data = (void*)gui;
	config.word_wrap = word_wrap;
	config.size.x = 500;
	config.size.y = res_y;
	config.focus_height = 0.3f;

	TheDebuginator debuginator;
	debuginator_create(&config, &debuginator);

	GameData data = { 0 };
	debug_menu_setup(&debuginator, &data);

	float current_y = 0;

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	double dt = 0;

	SDL_Event event;
	for (size_t i = 0; i < 400000; i++) {
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();
		Uint64 freq = SDL_GetPerformanceFrequency();
		dt = (double)((NOW - LAST) * 1.0 / freq);

		while (SDL_PollEvent(&event) != 0)
		{
			switch (event.type) {
			case SDL_QUIT:
			{
				i = 10000000;
				break;
			}
			case SDL_KEYDOWN:
			{
				if (event.key.keysym.sym == SDLK_UP) {
					debuginator_move_to_prev_leaf(&debuginator);
				}
				else if (event.key.keysym.sym == SDLK_DOWN) {
					debuginator_move_to_next_leaf(&debuginator);
					//debuginator_move_to_next(&debuginator);
				}
				else if (event.key.keysym.sym == SDLK_LEFT) {
					if (debuginator.is_open && !debuginator.hot_item->leaf.is_active) {
						debuginator_set_open(&debuginator, false);
					}
					else {
						debuginator_move_to_parent(&debuginator);
					}
				}
				else if (event.key.keysym.sym == SDLK_RIGHT) {
					if (!debuginator.is_open) {
						debuginator_set_open(&debuginator, true);
					}
					else {
						debuginator_move_to_child(&debuginator);
					}
				}
				else if (event.key.keysym.sym == SDLK_ESCAPE) {
					i = 10000000;
				}
			}
			//default:
				//break;
			}
		}

		debuginator_update(&debuginator, dt * 10);

		// update theme opacity
		for (size_t i = 0; i < COLOR_NumColors; i++) {
			s_theme[i].a = s_themes[s_theme_index][i].a * debuginator.openness;
		}

		gui_frame_begin(gui, i);

		// bouncing boxes
		if (data.draw_boxes) {
			gui_draw_rect_filled(gui, Vector2(i % 500, i % 500), Vector2(300, 300), Color(255, 255, 255, 255));
			gui_draw_rect_filled(gui, Vector2((i / 2) % 350, i % 700), Vector2(200, 200), Color(0, 0, 0, 255));
		}

		for (size_t i = 0; i < data.boxes_n; i++) {
			GameBox* box = &data.boxes[i];
			box->pos.x += box->velocity.x * dt;
			box->pos.y += box->velocity.y * dt;
			if (box->pos.x < 0 && box->velocity.x < 0 || box->pos.x + box->size.x > res_x && box->velocity.x > 0) {
				box->velocity.x *= -1;
			}
			if (box->pos.y  - box->size.y < 0 && box->velocity.y < 0 || box->pos.y + box->size.y > res_x && box->velocity.y > 0) {
				box->velocity.y *= -1;
			}
			gui_draw_rect_filled(gui, box->pos, box->size, box->color);
		}

		float width = 400;
		Vector2 offset(-width * (1 - debuginator.openness), 0);

		// Background
		gui_draw_rect_filled(gui, offset, Vector2(500, res_y), s_theme[COLOR_Background]);

		// Ensure hot item is smoothly placed at a nice position
		//distance_to_hot_item(debuginator.root, debuginator.hot_item, &offset.y);
		//float wanted_y = res_y * 0.4f;
		//float distance_to_wanted_y = wanted_y - offset.y;
		//offset.y = lerp(current_y, distance_to_wanted_y, 0.05f);
		//current_y = offset.y;

		// Draw all items
		//debuginator_draw_item(&debuginator, debuginator.root, *(DebuginatorVector2*)&offset, true);
		debuginator_draw(&debuginator);


		// Sleep a bit for my battery!
		float fps = 60;
		float frame_time = 1 / fps;
		if (frame_time > dt) {
			SDL_Delay(1000 * (frame_time - dt));
		}

		if (dt > 0) {
			char fpsstr[64] = { 0 };
			sprintf_s(fpsstr, 64, "FPS: %.2lf / s: %.15lf", 1/dt, dt);
			gui_draw_text(gui, fpsstr, Vector2(res_x / 2, 20), s_fonts[FONT_ItemDescription], Color(255, 255, 0, 255));
		}

		gui_frame_end(gui);
	}

	for (size_t i = 0; i < 16; i++) { // TODO unhardcode
		if (s_fonts[i] != 0x0) { // TODO invalid handle
			gui_unregister_font_template(gui, s_fonts[i]);
		}
	}

	gui_destroy_gui(gui);
	return 0;
}
