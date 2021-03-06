
#include <assert.h>

static const int WIDTH = 500;

static void unittest_debuginator_assert(bool test) {
	if (!test) {
	}
	assert(test);
	//DebugBreak();
}

#define DEBUGINATOR_assert unittest_debuginator_assert
#define DEBUGINATOR_static_assert unittest_debuginator_assert
#define ASSERT unittest_debuginator_assert
#define DEBUGINATOR_IMPLEMENTATION
#include "../../the_debuginator.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <string.h>
#include <stdio.h>

#include "gui.h"
#include "game.h"
#include "demo.h"

enum FontTemplate {
	FONT_DemoHeader,
	FONT_ItemTitle,
	FONT_ItemDescription,
	FONT_Count
};

static FontTemplateHandle s_fonts[16];

static bool theme_setup(GuiHandle gui) {
	memset(s_fonts, 0, sizeof(*s_fonts));
	s_fonts[FONT_DemoHeader] = gui_register_font_template(gui, "LiberationMono-Bold.ttf", 72);
	s_fonts[FONT_ItemTitle] = gui_register_font_template(gui, "LiberationMono-Regular.ttf", 18);
	s_fonts[FONT_ItemDescription] = gui_register_font_template(gui, "LiberationSerif-Regular.ttf", 18);

	for (size_t i = 0; i < FONT_Count; i++) {
		if (s_fonts[i] == 0) {
			return false;
		}
	}

	return true;
}

void draw_text(const char* text, DebuginatorVector2* position, DebuginatorColor* color, DebuginatorFont* font, void* userdata) {
	GuiHandle gui = (GuiHandle)userdata;
	int color_index = font->draw_type == DEBUGINATOR_ItemDescription ? (int)FONT_ItemDescription : (int)FONT_ItemTitle;
	gui_draw_text(gui, text, *(Vector2*)position, s_fonts[color_index], *(Color*)color);
}

void draw_rect(DebuginatorVector2* position, DebuginatorVector2* size, DebuginatorColor* color, void* userdata) {
	gui_draw_rect_filled((GuiHandle)userdata, *(Vector2*)position, *(Vector2*)size, *(Color*)color);
}

void draw_image(DebuginatorVector2* position, DebuginatorVector2* size, DebuginatorImageHandle handle, void* userdata) {
	gui_draw_texture((GuiHandle)userdata, handle.h.ull_value, *(Vector2*)position, *(Vector2*)size);
}

void word_wrap(const char* text, DebuginatorFont* font, float max_width, int* row_count, int* row_lengths, int row_lengths_buffer_size, void* app_userdata) {
	int color_index = font->draw_type == DEBUGINATOR_ItemDescription ? (int)FONT_ItemDescription : (int)FONT_ItemTitle;
	gui_word_wrap((GuiHandle)app_userdata, text, s_fonts[color_index], max_width, row_count, row_lengths, row_lengths_buffer_size);
}

DebuginatorVector2 text_size(const char* text, DebuginatorFont* font, void* userdata) {
	int color_index = font->draw_type == DEBUGINATOR_ItemDescription ? (int)FONT_ItemDescription : (int)FONT_ItemTitle;
	Vector2 text_size = gui_text_size((GuiHandle)userdata, text, s_fonts[color_index]);
	return *(DebuginatorVector2*)&text_size;
}

void log(const char* text, void* userdata) {
	(void)userdata;
	printf("[Debuginator] %s\n", text);
}

void play_sound(DebuginatorSoundEvent event, void* userdata) {
	(void)userdata;
	gui_play_sound(event);
}

struct SaveData {
	char* buffer;
	int buffer_size;
	int buffer_capacity;
};

bool save_item(const char* key, const char* value, void* userdata) {
	SaveData* save_data = (SaveData*)userdata;
	if (save_data->buffer_capacity - save_data->buffer_size < 512) {
		return false;
	}

	int chars = sprintf_s(save_data->buffer + save_data->buffer_size, save_data->buffer_capacity - save_data->buffer_size, "%s=%s\n", key, value);
	save_data->buffer_size += chars;
	return true;
}

void save(TheDebuginator* debuginator) {
	SaveData save_data;
	save_data.buffer = NULL;
	save_data.buffer_size = 0;
	save_data.buffer_capacity = 1024;

	while (true) {
		save_data.buffer = (char*)malloc(save_data.buffer_capacity);
		memset(save_data.buffer, 0, save_data.buffer_capacity);
		bool saved = debuginator_save(debuginator, save_item, &save_data);
		if (saved) {
			break;
		}

		save_data.buffer_size = 0;
		save_data.buffer_capacity *= 16;
		free(save_data.buffer);
	}

	FILE* file = NULL;
	int error = fopen_s(&file, "DebuginatorConfig.txt", "w");
	if (file == NULL || error < 0) {
		return;
	}

	fputs(save_data.buffer, file);
	fclose(file);
	free(save_data.buffer);
}

bool load(TheDebuginator* debuginator, char* loaded_data_buffer, int loaded_buffer_size) {
	FILE* file = NULL;
	int error = fopen_s(&file, "DebuginatorConfig.txt", "r");
	if (file == NULL || error < 0) {
		return true;
	}

	bool result = true;
	char load_buffer[1024] = { 0 };
	fread_s(load_buffer, 1024, 1, 1023, file);
	const char* data = load_buffer;
	while (*data != '\0') {
		if (loaded_buffer_size < 512) {
			result = false;
			break;
		}

		const char* key = data;
		const char* value = NULL;
		const char* loaded_data_buffer_key = NULL;
		const char* loaded_data_buffer_value = NULL;
		while (*data++ != '\0') {
			if (*data == '=') {
				memcpy(loaded_data_buffer, key, data - key);
				loaded_data_buffer[data - key] = 0;
				loaded_data_buffer_key = loaded_data_buffer;
				loaded_data_buffer += data - key + 1;
				loaded_buffer_size -= (int)(data - key + 1);
				value = (++data);
			}

			if (*data == '\n' && value != NULL) {
				memcpy(loaded_data_buffer, value, data - value);
				loaded_data_buffer[data - value] = 0;
				loaded_data_buffer_value = loaded_data_buffer;
				loaded_data_buffer += data - value + 1;
				loaded_buffer_size -= (int)(data - value + 1);
				// Debuginator needs to own this
				const char* value_owned = debuginator_copy_string(debuginator, loaded_data_buffer_value, 0);
				debuginator_load_item(debuginator, loaded_data_buffer_key, value_owned);
				++data;
				break;
			}
		}
	}

	fclose(file);
	return result;
}

bool handle_debuginator_keyboard_input_event(SDL_Event* event, TheDebuginator* debuginator, DemoData* demodata) {
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		DebuginatorItem* hot_mouse_item = debuginator_get_item_at_mouse_cursor(debuginator, NULL);
		if (hot_mouse_item == NULL) {
			demo_trigger_next(demodata);
		}
	}

	if (!debuginator_is_open(debuginator)) {
		if (event->type == SDL_TEXTINPUT) {
			int text_length = (int)strlen(event->text.text);
			if (text_length > 0) {
				char key[] = { event->text.text[0], 0 };
				debuginator_activate_hot_key(debuginator, key);
			}

			return true;
		}

		if (event->type == SDL_KEYDOWN) {
			if (event->key.keysym.sym == SDLK_RIGHT || event->key.keysym.scancode == SDL_SCANCODE_GRAVE) {
				debuginator_set_open(debuginator, true);
				return true;
			}
		}

		return false;
	}

	int hot_item_index;
	DebuginatorItem* hot_item = debuginator_get_hot_item(debuginator, &hot_item_index);
	switch (event->type) {
		case SDL_KEYDOWN:
		{
			if (event->key.keysym.scancode == SDL_SCANCODE_LCTRL || event->key.keysym.scancode == SDL_SCANCODE_RCTRL) {
				return true;
			}

			if (event->key.keysym.sym == SDLK_UP) {
				debuginator_reset_scrolling(debuginator);
				bool long_move = (event->key.keysym.mod & SDLK_LCTRL) > 0;
				debuginator_move_to_prev_leaf(debuginator, long_move);
				return true;
			}
			else if (event->key.keysym.sym == SDLK_DOWN) {
				debuginator_reset_scrolling(debuginator);
				bool long_move = (event->key.keysym.mod & SDLK_LCTRL) > 0;
				debuginator_move_to_next_leaf(debuginator, long_move);
				return true;
			}
			else if (event->key.keysym.sym == SDLK_LEFT || event->key.keysym.sym == SDLK_ESCAPE || event->key.keysym.scancode == SDL_SCANCODE_GRAVE) {
				debuginator_reset_scrolling(debuginator);
				if (debuginator->is_open && (debuginator_is_folder(hot_item) || !hot_item->leaf.is_expanded)) {
					debuginator_set_open(debuginator, false);
					save(debuginator);
					return true;
				}
				else if (!debuginator_is_folder(hot_item) && hot_item->leaf.is_expanded) {
					debuginator_move_to_parent(debuginator);
					return true;
				}
			}
			else if (event->key.keysym.sym == SDLK_RIGHT) {
				debuginator_reset_scrolling(debuginator);
				if (debuginator_is_folder(hot_item)) {
					debuginator_set_collapsed(debuginator, hot_item, !debuginator_is_collapsed(hot_item));
					return true;
				}
				else {
					bool direct_activate = (event->key.keysym.mod & SDLK_LCTRL) > 0;
					debuginator_move_to_child(debuginator, direct_activate);
					return true;
				}
			}
			else if (event->key.keysym.sym == SDLK_BACKSPACE) {
				if (debuginator->filter_length > 0) {
					char filter[64] = { 0 };
					strcpy_s(filter, 64, debuginator->filter);
					filter[--debuginator->filter_length] = '\0';
					debuginator_update_filter(debuginator, filter);
					//debuginator->filter[--debuginator->filter_length] = '\0';
				}
				else if (debuginator->filter_enabled) {
					debuginator_set_filtering_enabled(debuginator, false);
				}
				else {
					debuginator_set_filtering_enabled(debuginator, true);
				}
			}
			else if (event->key.keysym.sym >= 'a' && event->key.keysym.sym <= 'z') {
				int hot_mouse_item_index;
				DebuginatorItem* hot_mouse_item = debuginator_get_item_at_mouse_cursor(debuginator, &hot_mouse_item_index);
				if (hot_mouse_item != NULL) {
					hot_item = hot_mouse_item;
					hot_item_index = hot_mouse_item_index;
				}

				int buffer_size = 256;
				char path[256];
				debuginator_get_path(debuginator, hot_item, path, &buffer_size);
				ASSERT(buffer_size == 256);
				char key[] = { (char)event->key.keysym.sym, 0 };

      			DebuginatorItem* prev_hot_key_item = debuginator_get_first_assigned_hot_key_item(debuginator, key);
      			if ( prev_hot_key_item == hot_item ) {
      				// Key was already assigned to this item, toggle it off
      				debuginator_unassign_hot_key(debuginator, key);
					break;
      			}

				bool multi_add_key_held = SDL_GetModState() & (KMOD_LCTRL | KMOD_RCTRL);
				if (prev_hot_key_item && !multi_add_key_held) {
					// Item was already assigned to something and user just wants the new one assigned,
					// so unassign the old one first.
			        debuginator_unassign_hot_key(debuginator, key);
				}

				debuginator_assign_hot_key(debuginator, key, path, hot_item_index, NULL);
			}
			break;
		}
		case SDL_TEXTINPUT:
		{
			int new_text_length = (int)strlen(event->text.text);
			if (debuginator->filter_enabled) {
				if (debuginator->filter_length + new_text_length >= sizeof(debuginator->filter)) {
					break;
				}

				char filter[64] = { 0 };
				ASSERT(sizeof(filter) >= sizeof(debuginator->filter));
				strcpy_s(filter, sizeof(debuginator->filter), debuginator->filter);
				strcat_s(filter, sizeof(debuginator->filter), event->text.text);
				// if (SDL_GetModState() & (KMOD_LCTRL | KMOD_RCTRL);)
				debuginator->filter_length = (int)strlen(filter);
				debuginator_update_filter(debuginator, filter);
			}
			break;
		}
		case SDL_MOUSEMOTION: {
			DebuginatorVector2 mouse_cursor_pos = { (float)event->motion.x, (float)event->motion.y };
			debuginator_set_mouse_cursor_pos(debuginator, &mouse_cursor_pos);

			break;
		}
		case SDL_MOUSEWHEEL: {
			debuginator_apply_scroll(debuginator, event->wheel.y * debuginator->item_height * 1);

			break;
		}
		case SDL_MOUSEBUTTONDOWN: {
			DebuginatorVector2 mouse_cursor_pos = { (float)event->button.x, (float)event->button.y };
			debuginator_set_mouse_cursor_pos(debuginator, &mouse_cursor_pos);
			if (event->button.button == SDL_BUTTON_LEFT && event->button.state == SDL_PRESSED) {
				debuginator_activate_item_at_mouse_cursor(debuginator);
			}
			else if (event->button.button == SDL_BUTTON_RIGHT && event->button.state == SDL_PRESSED) {
				debuginator_expand_item_at_mouse_cursor(debuginator, DEBUGINATOR_Toggle);
			}

			break;
		}
		case SDL_WINDOWEVENT: {
			if (event->window.event == SDL_WINDOWEVENT_LEAVE) {
				DebuginatorVector2 mouse_cursor_pos = { -1.f, -1.f };
				debuginator_set_mouse_cursor_pos(debuginator, &mouse_cursor_pos);
			}

			break;
		}
	}


	return false;
}

bool handle_debuginator_gamepad_input_event(SDL_Event* event, TheDebuginator* debuginator, SDL_GameControllerButton& current_button, double& time_since_button_pressed, float& scroll_speed) {
	switch (event->type) {
		case SDL_CONTROLLERBUTTONDOWN:
		{
			SDL_ControllerButtonEvent& button_ev = event->cbutton;
			current_button = (SDL_GameControllerButton)button_ev.button;
			time_since_button_pressed = 0;

			debuginator_reset_scrolling(debuginator);

			if (button_ev.button == SDL_CONTROLLER_BUTTON_DPAD_UP) {
				bool long_move = (event->key.keysym.mod & SDLK_LCTRL) > 0;
				debuginator_move_to_prev_leaf(debuginator, long_move);
				return true;
			}
			else if (button_ev.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) {
				bool long_move = (event->key.keysym.mod & SDLK_LCTRL) > 0;
				debuginator_move_to_next_leaf(debuginator, long_move);
				return true;
			}
			else if (current_button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER) {
				bool long_move = true;
				debuginator_move_to_prev_leaf(debuginator, long_move);
				return true;
			}
			else if (current_button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) {
				bool long_move = true;
				debuginator_move_to_next_leaf(debuginator, long_move);
				return true;
			}
			else if (button_ev.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT || button_ev.button == SDL_CONTROLLER_BUTTON_BACK) {
				if (debuginator->is_open && !debuginator->hot_item->leaf.is_expanded) {
					debuginator_set_open(debuginator, false);
					save(debuginator);
					return true;
				}
				else if (!debuginator->hot_item->is_folder && debuginator->hot_item->leaf.is_expanded) {
					debuginator_move_to_parent(debuginator);
					return true;
				}
			}
			else if (button_ev.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) {
				if (!debuginator->is_open) {
					debuginator_set_open(debuginator, true);
					return true;
				}
				else {
					bool direct_activate = (event->key.keysym.mod & SDLK_LCTRL) > 0;
					debuginator_move_to_child(debuginator, direct_activate);
					return true;
				}
			}
			else if (button_ev.button == SDL_CONTROLLER_BUTTON_A) {
				bool direct_activate = true;
				debuginator_move_to_child(debuginator, direct_activate);
				return true;
			}

		}
		break;
		case SDL_CONTROLLERBUTTONUP:
		{
			current_button = SDL_CONTROLLER_BUTTON_INVALID;
			//in_repeat_mode = false;
		}
		break;
		case SDL_CONTROLLERAXISMOTION:
		{
			SDL_ControllerAxisEvent& motion_ev = event->caxis;
			if (motion_ev.axis == SDL_CONTROLLER_AXIS_LEFTY) {
				scroll_speed = 0;
				if (motion_ev.value < -5000 || motion_ev.value > 5000) {
					scroll_speed = (float)(-3000 * (motion_ev.value / 32767.0) * (motion_ev.value / 32767.0) * (motion_ev.value / 32767.0));
				}
			}
		}
		break;
	}

	return false;
}

void handle_debuginator_gamepad_input(TheDebuginator* debuginator, SDL_GameControllerButton current_button, double& time_since_button_pressed) {
	if (current_button == SDL_CONTROLLER_BUTTON_INVALID) {
		return;
	}

	if (time_since_button_pressed < 0.3) {
		return;
	}

	time_since_button_pressed = 0.25;
	if (current_button == SDL_CONTROLLER_BUTTON_DPAD_UP) {
		bool long_move = false;
		debuginator_move_to_prev_leaf(debuginator, long_move);
		return;
	}
	else if (current_button == SDL_CONTROLLER_BUTTON_DPAD_DOWN) {
		bool long_move = false;
		debuginator_move_to_next_leaf(debuginator, long_move);
		return;
	}
	else if (current_button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER) {
		bool long_move = true;
		debuginator_move_to_prev_leaf(debuginator, long_move);
		return;
	}
	else if (current_button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) {
		bool long_move = true;
		debuginator_move_to_next_leaf(debuginator, long_move);
		return;
	}
}
void handle_debuginator_gamepad_input_immediate(TheDebuginator* debuginator, GuiHandle gui) {
	SDL_GameController** controller =	gui_get_controllers(gui);
	while(*controller) {
		int axis_value = SDL_GameControllerGetAxis(*controller,  SDL_CONTROLLER_AXIS_RIGHTY);
		if (axis_value * axis_value > 8000 * 8000) {
			float change = 0.1f * (axis_value / 32767.0f) * (axis_value / 32767.0f) * (axis_value / 32767.0f);
			DebuginatorItem* hot_item = debuginator_get_hot_item(debuginator, NULL);
			debuginator_modify_value(debuginator, hot_item, 0, change, false);
		}

		++controller;
	}
}

int main(int argc, char **argv)
{
	(void)(argc, argv);

	int res_x = 1280;
	int res_y = 720;
	bool vsync_on = true;
	GuiHandle gui = gui_create_gui(res_x, res_y, "The Debuginator - SDL Demo", vsync_on);
	if (gui == 0) {
		return 1;
	}

	if (!theme_setup(gui)) {
		gui_destroy_gui(gui);
		return 1;
	}

	TextureHandle colorpicker_image = gui_load_texture(gui, "color_picker.png");

	int memory_arena_capacity = 1024 * 1024 * 1;
	char* memory_arena = (char*)malloc(memory_arena_capacity);
	TheDebuginatorConfig config;
	debuginator_get_default_config(&config);
	config.memory_arena = memory_arena;
	config.memory_arena_capacity = memory_arena_capacity;
	config.draw_image = draw_image;
	config.draw_rect = draw_rect;
	config.draw_text = draw_text;
	config.word_wrap = word_wrap;
	config.text_size = text_size;
	config.log = log;
	config.play_sound = play_sound;
	config.app_user_data = (void*)gui;
	config.size.x = 500;
	config.size.y = (float)res_y;
	config.screen_resolution.x = (float)res_x;
	config.screen_resolution.y = (float)res_y;
	//config.open_direction = -1; // To show it on the right side of the screen
	config.create_default_debuginator_items = true;
	config.colorpicker_image.h.ull_value = colorpicker_image;

	TheDebuginator debuginator;
	debuginator_create(&config, &debuginator);

	char* loaded_data_buffer = (char*)malloc(10 * 1024 * 1024);
	bool load_result = load(&debuginator, loaded_data_buffer, 10 * 1024 * 1024);
	if (!load_result) {
		return 1;
	}
	free(loaded_data_buffer);

	GameData* gamedata = game_init(gui, &debuginator);
	DemoData* demodata = demo_init(gui, &debuginator);

	bool limit_framerate = false;
	debuginator_create_bool_item(&debuginator, "SDL Demo/Throttle framerate", "Disables sleeping between frames.", &limit_framerate);

	bool show_framerate = false;
	debuginator_create_bool_item(&debuginator, "SDL Demo/Show framerate", "Shows framerate and frame time in ms.", &show_framerate);

	const char* preset_paths[2] = { "SDL Demo/Throttle framerate", "SDL Demo/Show framerate" };
	const char* preset_value_titles[2] = { "True", "False" };
	debuginator_create_preset_item(&debuginator, "SDL Demo/Edit types/Preset", preset_paths, preset_value_titles, NULL, 2);

	float demo_y_offset = 0;
	debuginator_create_numberrange_float_item(&debuginator, "SDL Demo/Edit types/Number slider", "Example of the NumberRange edit type", &demo_y_offset, -100, 200);

	bool show_background = true;
	debuginator_create_bool_item(&debuginator, "SDL Demo/Show background image", "", &show_background);
	debuginator_create_array_item(&debuginator, NULL, "SDL Demo/Attributions", "Background image: sk5.jpg. See sk5.txt.", NULL, NULL, NULL, NULL, 0, 0);

	TextureHandle bg_texture = gui_load_texture(gui, "sk5.jpg");

	Uint64 START = SDL_GetPerformanceCounter();
	Uint64 NOW = START;
	Uint64 LAST = 0;

	SDL_Event event;
	bool quit = false;
	double time_since_button_pressed = 0;
	SDL_GameControllerButton current_button = SDL_CONTROLLER_BUTTON_INVALID;
	float gamepad_scroll_speed = 0;
	double time_now = 0;
	double bg_height = 0;
	while (!quit) {
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();
		Uint64 freq = SDL_GetPerformanceFrequency();
		double dt = (double)((NOW - LAST) * 1.0 / freq);
		time_now += dt;
		time_since_button_pressed += dt;

		while (SDL_PollEvent(&event) != 0)
		{
			if (handle_debuginator_keyboard_input_event(&event, &debuginator, demodata)) {
				continue;
			}

			if (handle_debuginator_gamepad_input_event(&event, &debuginator, current_button, time_since_button_pressed, gamepad_scroll_speed)) {
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
				case SDL_WINDOWEVENT : {
					switch (event.window.event) {
						case SDL_WINDOWEVENT_RESIZED: {
							debuginator_set_size(&debuginator, (int)debuginator.size.x, event.window.data2 );
							debuginator_set_screen_resolution(&debuginator, event.window.data1, event.window.data2 );
							res_x = event.window.data1;
							res_y = event.window.data2;
						} break;
					}

					break;
				}
			}
		}

		handle_debuginator_gamepad_input(&debuginator, current_button, time_since_button_pressed);
		handle_debuginator_gamepad_input_immediate(&debuginator, gui);
		if (gamepad_scroll_speed != 0) {
			debuginator_apply_scroll(&debuginator, (int)(gamepad_scroll_speed * dt));
		}

		debuginator_update(&debuginator, (float)dt);

		gui_frame_begin(gui);

		if (show_background) {
			bg_height = bg_height * (1 - 0.02) - 200 * 0.02;
			Vector2 bg_texture_pos = {0, (float)bg_height};
			Vector2 bg_texture_size = {(float)res_x, 0};
			gui_draw_texture(gui, bg_texture, bg_texture_pos, bg_texture_size);
		}

		Vector2 main_text_size = gui_text_size(gui, "The Debuginator", s_fonts[FONT_DemoHeader]);
		Vector2 main_text_pos(res_x / 2 - main_text_size.x / 2, res_y / 4 - main_text_size.y / 2 + demo_y_offset);
		float main_text_width = res_x - debuginator.openness * debuginator.size.x;
		float main_text_offset = debuginator.open_direction == 1 ? debuginator.openness * debuginator.size.x : 0;
		main_text_pos.x = main_text_offset + main_text_width / 2 - main_text_size.x / 2;
		unsigned char main_text_brightness = 80 + (unsigned char)(50*sin((double)(NOW-START) * 1 / freq));
		Color main_text_color(30 + main_text_brightness, 30 + main_text_brightness, main_text_brightness, 255);
		gui_draw_text(gui, "The Debuginator", main_text_pos, s_fonts[FONT_DemoHeader], main_text_color);

		game_update(gamedata, (float)dt);

		Vector2 demo_pos = main_text_pos;
		demo_pos.y += 100;
		demo_update(demodata, (float)dt, demo_pos);

		debuginator_draw(&debuginator, (float)dt);

		// Not a good way to enforce a framerate due to delay being inprecise but
		// its purpose is to save some battery, not to get exactly X fps.
		float fps = 30;
		float frame_time = 1 / fps;
		if (limit_framerate && frame_time > dt) {
			SDL_Delay((Uint32)(1000 * (frame_time - dt)));
		}

		if (show_framerate && dt > 0) {
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

	free(memory_arena);
	gui_destroy_gui(gui);
	return 0;
}
