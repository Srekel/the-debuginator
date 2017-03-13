#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <SDL.h>
#include <SDL_ttf.h>


static const int WIDTH = 500;

static void unittest_debuginator_assert(bool test) {
	if (!test) {
	}
	assert(test);
	//DebugBreak();
}

#define DEBUGINATOR_assert unittest_debuginator_assert
#define ASSERT unittest_debuginator_assert
#define DEBUGINATOR_IMPLEMENTATION
#include "../../the_debuginator.h"

#include "gui.h"
#include "game.h"
//
//void debuginator__default_quick_draw(DebuginatorItem* item, void* data) {
//	if (item->leaf.num_values > 0) {
//		//Vector2 value_offset = *(Vector2*)position;
//		//value_offset.x = 300 + debuginator->openness * 500 - 500;
//		draw_text(gui, item->leaf.value_titles[item->leaf.active_index], *(Vector2*)position, s_fonts[FONT_ItemTitle], s_theme[color]);
//	}
//}





static FontTemplateHandle s_fonts[16];
enum FontTemplates {
	FONT_ItemTitle,
	FONT_ItemDescription,
	FONT_count
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

//struct QuickDrawDefaultData {
//	QuickDrawDefaultData(int font, int color) : font_index(font), color_index(color) {}
//	int font_index;
//	int color_index;
//};

bool handle_debuginator_input(SDL_Event* event, TheDebuginator* debuginator) {
	switch (event->type) {
		case SDL_KEYDOWN:
		{
			if (event->key.keysym.sym == SDLK_UP) {
				debuginator_move_to_prev_leaf(debuginator);
				return true;
			}
			else if (event->key.keysym.sym == SDLK_DOWN) {
				debuginator_move_to_next_leaf(debuginator);
				return true;
			}
			else if (event->key.keysym.sym == SDLK_LEFT || event->key.keysym.sym == SDLK_ESCAPE) {
				if (debuginator->is_open && !debuginator->hot_item->leaf.is_active) {
					debuginator_set_open(debuginator, false);
					return true;
				}
				else if (!debuginator->hot_item->is_folder && debuginator->hot_item->leaf.is_active) {
					debuginator_move_to_parent(debuginator);
					return true;
				}
			}
			else if (event->key.keysym.sym == SDLK_RIGHT) {
				if (event->key.keysym.mod == SDLK_LCTRL) {
					if (debuginator) {

					}
				}
				if (!debuginator->is_open) {
					debuginator_set_open(debuginator, true);
					return true;
				}
				else {
					debuginator_move_to_child(debuginator);
					return true;
				}
			}

			break;
		}
	}

	return false;
}

int main(int argc, char **argv)
{
	(void)(argc, argv);

	int res_x = 800;
	int res_y = 600;
	bool vsync_on = true;
	GuiHandle gui = gui_create_gui(res_x, res_y, "Debuginator SDL demo", vsync_on);
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
	config.app_user_data = (void*)gui;
	config.word_wrap = word_wrap;
	config.size.x = 500;
	config.size.y = (float)res_y;
	config.screen_resolution.x = (float)res_x;
	config.screen_resolution.y = (float)res_y;
	config.focus_height = 0.3f;
	config.left_aligned = true;

	TheDebuginator debuginator;
	debuginator_create(&config, &debuginator);

	GameData* gamedata = game_init(gui, &debuginator);

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	double dt = 0;

	SDL_Event event;
	bool quit = false;
	while (!quit) {
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();
		Uint64 freq = SDL_GetPerformanceFrequency();
		dt = (double)((NOW - LAST) * 1.0 / freq);

		while (SDL_PollEvent(&event) != 0)
		{
			if (handle_debuginator_input(&event, &debuginator)) {
				continue;
			}

			switch (event.type) {
				case SDL_KEYDOWN:
				{
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						quit = true;
					}
					break;
				}
				case SDL_QUIT:
				{
					quit = true;
					break;
				}
			}
		}

		debuginator_update(&debuginator, (float)dt * 5);

		gui_frame_begin(gui);

		game_update(gamedata, (float)dt);
		debuginator_draw(&debuginator, (float)dt * 1.f);

		// Not a good way to enforce a framerate due to delay being inprecise but
		// its purpose is to save some battery, not to get exactly X fps.
		float fps = 30;
		float frame_time = 1 / fps;
		if (frame_time > dt) {
			SDL_Delay((Uint32)(1000 * (frame_time - dt)));
		}

		if (dt > 0) {
			char fpsstr[64] = { 0 };
			sprintf_s(fpsstr, 64, "FPS: %.2lf / ms: %.15lf", 1/dt, dt * 1000);
			gui_draw_text(gui, fpsstr, Vector2(res_x * 0.5f, 20.f), s_fonts[FONT_ItemDescription], Color(255, 255, 0, 255));
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
