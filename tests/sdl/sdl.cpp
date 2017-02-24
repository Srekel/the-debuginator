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

static FontTemplateHandle s_fonts[16];
enum FontTemplates {
	FONT_ItemTitle,
	FONT_count
};

static Color* s_theme;
static Color s_colors[3][16];
enum ColorTemplates {
	COLOR_Background,
	COLOR_ItemTitle,
	COLOR_ItemTitleHot,
	COLOR_ItemTitleActive
};

static bool theme_setup(GuiHandle gui) {
	s_fonts[FONT_ItemTitle] = register_font_template(gui, "LiberationMono-Regular.ttf", 22);
	s_colors[0][COLOR_Background]      = Color(50, 150, 50, 200);
	s_colors[0][COLOR_ItemTitle]       = Color(160, 160, 160, 255);
	s_colors[0][COLOR_ItemTitleHot]    = Color(200, 200, 200, 255);
	s_colors[0][COLOR_ItemTitleActive] = Color(255, 255, 220, 255);
	
	s_colors[1][COLOR_Background]      = Color(50, 50, 150, 200);
	s_colors[1][COLOR_ItemTitle]       = Color(120, 120, 120, 255);
	s_colors[1][COLOR_ItemTitleHot]    = Color(200, 200, 200, 255);
	s_colors[1][COLOR_ItemTitleActive] = Color(220, 220, 255, 255);
	
	s_colors[2][COLOR_Background]      = Color(100, 100, 100, 200);
	s_colors[2][COLOR_ItemTitle]       = Color(150, 150, 150, 255);
	s_colors[2][COLOR_ItemTitleHot]    = Color(200, 200, 200, 255);
	s_colors[2][COLOR_ItemTitleActive] = Color(255, 255, 255, 255);

	s_theme = s_colors[0];

	for (size_t i = 0; i < FONT_count; i++) {
		if (s_fonts[i] == 0) {
			return false;
		}
	}

	return true;
}

static void on_change_theme(DebuginatorItem* item, void* value, const char* value_title) {
	int theme_index = *(int*)value;
	s_theme = s_colors[theme_index];
}

static void debug_menu_setup(TheDebuginator* debuginator, GameData* data) {

	{
		static int theme_indices[3] = { 0, 1, 2 };
		static const char* string_titles[3] = { "Green", "Blue", "Gray" };
		debuginator_create_array_item(debuginator, NULL, "Debuginator/Theme",
			"Change the color theme", on_change_theme, NULL,
			string_titles, (void*)theme_indices, 3, sizeof(theme_indices[0]));
	}
	debuginator_create_bool_item(debuginator, "SimpleBool 1", "Change a bool.", &data->mybool);
	debuginator_create_bool_item(debuginator, "Folder/SimpleBool 2", "Change a bool.", &data->mybool);
	//debuginator_create_bool_item(debuginator, "Folder/SimpleBool 3", "Change a bool.", &data->mybool);
	debuginator_create_bool_item(debuginator, "Folder/SimpleBool 4 with a really long long title", "Change a bool.", &data->mybool);

	debuginator_new_folder_item(debuginator, NULL, "Folder 2", 0);

	debuginator_initialize(debuginator);
}

float draw_item(TheDebuginator* debuginator, DebuginatorItem* item, Vector2 offset, bool hot, GuiHandle gui) {
	//draw_rect_filled(gui, offset, Vector2(100, 30), Color(200, 100, 50, 200));

	if (item->is_folder) {
		unsigned color_index = item == debuginator->hot_item ? COLOR_ItemTitleActive : (hot ? COLOR_ItemTitleHot : COLOR_ItemTitle);
		draw_text(gui, item->title, offset, s_fonts[FONT_ItemTitle], s_theme[color_index]);
		offset.x += 20;
		DebuginatorItem* child = item->folder.first_child;
		while (child) {
			offset.y += 35;
			offset.y = draw_item(debuginator, child, offset, hot && child == item->folder.hot_child, gui);
			child = child->next_sibling;
		}
	}
	else {
		unsigned color_index = item == debuginator->hot_item && !item->leaf.is_active ? COLOR_ItemTitleActive : (hot ? COLOR_ItemTitleHot : COLOR_ItemTitle);
		draw_text(gui, item->title, offset, s_fonts[FONT_ItemTitle], s_theme[color_index]);
		
		offset.x += 20;
		for (size_t i = 0; i < item->leaf.num_values; i++) {
			offset.y += 35;
			const char* value_title = item->leaf.value_titles[i];
			bool value_hot = hot && i == item->leaf.hot_index;
			bool value_active = value_hot && item->leaf.is_active;
			unsigned value_color_index = value_active ? COLOR_ItemTitleActive : (value_hot ? COLOR_ItemTitleHot : COLOR_ItemTitle);
			draw_text(gui, value_title, offset, s_fonts[value_hot ? FONT_ItemTitle : FONT_ItemTitle], s_theme[value_color_index]);
		}
	}

	return offset.y;
}

int main(int argc, char **argv)
{
	(void)(argc, argv);

	int res_x = 800;
	int res_y = 600;
	GuiHandle gui = create_gui(res_x, res_y, "Debuginator SDL demo");
	if (gui == 0) {
		return 1;
	}

	if (!theme_setup(gui)) {
		destroy_gui(gui);
		return 1;
	}

	DebuginatorItem item_buffer[256];
	TheDebuginator debuginator = debuginator_create(item_buffer, 256);

	GameData data = { 0 };
	debug_menu_setup(&debuginator, &data);

	SDL_Event event;
	for (size_t i = 0; i < 4000; i++) {
		while (SDL_PollEvent(&event) != 0)
		{
			switch (event.type) {
			case SDL_QUIT:
			{
				i = 10000;
				break;
			}
			case SDL_KEYDOWN:
			{
				if (event.key.keysym.sym == SDLK_DOWN) {
					debuginator_move_sibling_next(&debuginator);
				}
				else if (event.key.keysym.sym == SDLK_LEFT) {
					debuginator_move_to_parent(&debuginator);
				}
				else if (event.key.keysym.sym == SDLK_RIGHT) {
					debuginator_move_to_child(&debuginator);
				}
				else if (event.key.keysym.sym == SDLK_ESCAPE) {
					i = 10000;
				}
			}
			//default:
				//break;
			}
		}

		frame_begin(gui, i);

		draw_rect_filled(gui, Vector2(0, 0), Vector2(200, res_y), s_theme[COLOR_Background]);

		Vector2 item_offset(0, 0);
		draw_item(&debuginator, debuginator.root, item_offset, true, gui);
		//SDL_RenderCopy(renderer, text_texture, NULL, &rectangle);
		frame_end(gui);
		SDL_Delay(10);
	}

	destroy_gui(gui);
	return 0;
}
