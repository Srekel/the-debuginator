#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "gui.h"

static void unittest_debuginator_assert(bool test) {
	if (!test) {
	}
	assert(test);
	//DebugBreak();
}

#define DEBUGINATOR_assert unittest_debuginator_assert
#define ASSERT unittest_debuginator_assert

#include "../../the_debuginator.h"

struct GameData {
	bool mybool;
	char mystring[256];
};

static void debug_menu_setup(TheDebuginator* debuginator, GameData* data) {
	debuginator_create_bool_item(debuginator, "SimpleBool 1", "Change a bool.", &data->mybool);
	debuginator_create_bool_item(debuginator, "Folder/SimpleBool 2", "Change a bool.", &data->mybool);
	debuginator_create_bool_item(debuginator, "Folder/SimpleBool 3", "Change a bool.", &data->mybool);
	debuginator_create_bool_item(debuginator, "Folder/SimpleBool 4 with a really long long title", "Change a bool.", &data->mybool);

	debuginator_new_folder_item(debuginator, NULL, "Folder 2", 0);
/*
	static const char* string_values[3] = { "gamestring 1", "gamestring 2", "gamestring 3" };
	static const char* string_titles[3] = { "First value", "Second one", "This is the third." };
	debuginator_create_array_item(debuginator, NULL, "Folder 2/String item",
		"Do it", unittest_on_item_changed_stringtest, &g_testdata,
		string_titles, (void*)string_values, 3, sizeof(string_values[0]));
*/
}

void draw_item(DebuginatorItem* item, Vector2 offset, GuiHandle gui) {
	draw_rect_filled(gui, offset, Vector2(100, 30), Color(200, 100, 50, 200));

	if (item->is_folder) {
		offset.x += 20;
		item = item->folder.first_child;
		while (item) {
			offset.y += 35;
			draw_item(item, offset, gui);
			item = item->next_sibling;
		}
	}
}

int main(int argc, char **argv)
{
	(void)(argc, argv);

	GuiHandle gui = create_gui(800, 600, "Debuginator SDL demo");
	if (gui == 0) {
		return 1;
	}

	FontTemplateHandle font = register_font_template(gui, "LiberationMono-Regular.ttf", 22);
	if (font == 0) {
		destroy_gui(gui);
		return 1;
	}

	DebuginatorItem item_buffer[256];
	TheDebuginator debuginator = debuginator_create(item_buffer, 256);

	GameData data = { 0 };
	debug_menu_setup(&debuginator, &data);

	SDL_Rect bgrectangle;
	bgrectangle.x = 000;
	bgrectangle.y = 000;
	bgrectangle.w = 200;
	bgrectangle.h = 500;


	SDL_Event event;
	for (size_t i = 0; i < 500; i++) {
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				i = 10000;
			}
		}

		frame_begin(gui);

		Vector2 item_offset(0, 0);
		draw_item(debuginator.root, item_offset, gui);
		draw_text(gui, "LOLOL", Vector2(100, 100), font, Color(255, 100, 0, 255));
		//SDL_RenderCopy(renderer, text_texture, NULL, &rectangle);
		frame_end(gui);
		SDL_Delay(10);
	}

	destroy_gui(gui);
	return 0;
}
