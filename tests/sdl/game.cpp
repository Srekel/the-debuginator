
#include "../../the_debuginator.h"

//#include <time.h>
#include <stdio.h>
#include <string.h>

#include "game.h"

static void on_box_action(DebuginatorItem* item, void* value, const char* value_title, void* app_userdata) {
	(void)app_userdata;
	(void)value;
	GameBox* box = (GameBox*)item->user_data;
	if (strcmp(value_title, "Hide") == 0) {
		box->color.a = 0;
	}
	else if (strcmp(value_title, "Show") == 0) {
		box->color.a = box->alpha;
	}
	else if (strcmp(value_title, "Speed up") == 0) {
		box->velocity.x *= 2;
		box->velocity.y *= 2;
	}
	else if (strcmp(value_title, "Slow down") == 0) {
		box->velocity.x *= 0.5f;
		box->velocity.y *= 0.5f;
	}
}

#pragma warning(suppress: 4505) // unreferenced local function
static void on_boxes_activated(DebuginatorItem* item, void* value, const char* value_title, void* app_userdata) {
	(void)app_userdata;
	(void)value;
	GameData* data = (GameData*)item->user_data;
	if (strcmp(value_title, "Add box") == 0 && data->boxes_n < 256) {
		GameBox* box = &data->boxes[data->boxes_n++];
		box->randomize();

		// Add item for box
		char folder[64] = { 0 };
		sprintf_s(folder, 64, "Game/Boxes/Box %02d", data->boxes_n);
		static const char* box_action_titles[5] = { "Hide", "Show", "Speed up", "Slow down" };
		debuginator_create_array_item(data->debuginator, NULL, folder,
			"Change box properties.", on_box_action, box,
			box_action_titles, NULL, 4, 0);
		debuginator_set_edit_type(data->debuginator, folder, DEBUGINATOR_EditTypeActionArray);
	}
	else if (strcmp(value_title, "Clear all boxes") == 0 && data->boxes_n < 256) {
		data->boxes_n = 0;

		debuginator_remove_item_by_path(data->debuginator, "Game/Boxes");
	}

	sprintf_s(data->box_string, "Box count: %d", data->boxes_n);
}

static void debug_menu_setup(TheDebuginator* debuginator, GameData* data) {
	(void)data;
	{
		static const char* string_titles[5] = { "String A", "String B", "String C", "String D", "String E" };
		debuginator_create_array_item(debuginator, NULL, "SDL Demo/String Test",
			"Multiple strings.", NULL, NULL,
			string_titles, NULL, 5, 0);
	}

	debuginator_create_bool_item(debuginator, "SDL Demo/Draw boxes", "Whether to draw the animated boxes or not.", &data->draw_boxes);

	{
		static const char* string_titles[3] = { "Clear all boxes", "Add box", data->box_string };
		debuginator_create_array_item(debuginator, NULL, "SDL Demo/Boxes",
			"Box things", on_boxes_activated, data,
			string_titles, NULL, 3, 0);
		debuginator_set_edit_type(debuginator, "SDL Demo/Boxes", DEBUGINATOR_EditTypeActionArray);
	}

	debuginator_create_bool_item(debuginator, "Test/LOL/XXZZ", "Change a bool.", &data->mybool);
	debuginator_create_bool_item(debuginator, "Test/LOL2/YY", "Change a bool.", &data->mybool);
	debuginator_create_bool_item(debuginator, "YYY", NULL, &data->mybool);


	// debuginator_create_bool_item(debuginator, "SimpleBool 1", "Change a bool.", &data->mybool);
	// debuginator_create_bool_item(debuginator, "Folder/SimpleBool 2", "Change a bool.", &data->mybool);
	// debuginator_create_bool_item(debuginator, "Folder/SimpleBool 3", "Change a bool.", &data->mybool);
	debuginator_create_bool_item(debuginator, "Folder/Subfolder with a long name ololololol/SimpleBool 4 with a really long long title", "Change a bool.", &data->mybool);
	// debuginator_create_bool_item(debuginator, "SDL Demo/Load test", "Change a bool.", &data->load_test);

	debuginator_create_folder_item(debuginator, NULL, "Folder 2");
	char folder[64] = { 0 };
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			sprintf_s(folder, 64, "Game/Test%02d/GameBool%02d", i, j);
			debuginator_create_bool_item(debuginator, folder, "Change a bool.", &data->gamebool);
		}
	}

	for (int i = 0; i < 1; i++) {
		for (int j = 0; j < 1; j++) {
			sprintf_s(folder, 64, "Game/Test%02d/GameBool%02d", i, j);
			DebuginatorItem* item = debuginator_get_item(debuginator, NULL, folder, false);
			debuginator_remove_item(debuginator, item);
		}
	}

	//debuginator_create_bool_item(debuginator, "Game/Test10/GameBool00", "Change a bool.", &data->gamebool);
}

static GameData s_game_data;

GameData* game_init(GuiHandle gui, TheDebuginator* debuginator) {
	memset(&s_game_data, 0, sizeof(s_game_data));
	s_game_data.gui = gui;
	s_game_data.window_size.x = 1280; // TODO fix
	s_game_data.window_size.y = 720;
	s_game_data.debuginator = debuginator;
	debug_menu_setup(debuginator, &s_game_data);

	return &s_game_data;
}

void game_update(GameData* game_data, float dt) {

	// bouncing boxes
	if (game_data->draw_boxes) {
		for (size_t box_i = 0; box_i < game_data->boxes_n; box_i++) {
			GameBox* box = &game_data->boxes[box_i];
			box->pos.x += box->velocity.x * (float)dt;
			box->pos.y += box->velocity.y * (float)dt;
			if (box->pos.x < 0 && box->velocity.x < 0 || box->pos.x + box->size.x > game_data->window_size.x && box->velocity.x > 0) {
				box->velocity.x *= -1;
			}
			if (box->pos.y - box->size.y < 0 && box->velocity.y < 0 || box->pos.y + box->size.y > game_data->window_size.y && box->velocity.y > 0) {
				box->velocity.y *= -1;
			}
			gui_draw_rect_filled(game_data->gui, box->pos, box->size, box->color);
		}
	}
}
