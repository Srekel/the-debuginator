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
	s_fonts[FONT_ItemTitle] = register_font_template(gui, "LiberationMono-Regular.ttf", 18);
	s_fonts[FONT_ItemDescription] = register_font_template(gui, "LiberationSerif-Italic.ttf", 18);

	s_themes[0][COLOR_Background]          = Color(25, 50, 25, 220);
	s_themes[0][COLOR_FolderTitle]         = Color(255, 255, 255, 255);
	s_themes[0][COLOR_ItemTitle]           = Color(120, 120, 0, 250);
	s_themes[0][COLOR_ItemTitleOverridden] = Color(200, 200, 0, 255);
	s_themes[0][COLOR_ItemTitleHot]        = Color(230, 230, 200, 255);
	s_themes[0][COLOR_ItemTitleActive]     = Color(100, 255, 100, 255);
	s_themes[0][COLOR_ItemDescription]     = Color(150, 150, 150, 255);
	s_themes[0][COLOR_ItemValueDefault]    = Color(50,  150, 50,  200);
	s_themes[0][COLOR_ItemValueOverridden] = Color(100, 255, 100, 200);
	s_themes[0][COLOR_ItemValueHot]        = Color(100, 255, 100, 200);
	s_themes[0][COLOR_LineHighlight]       = Color(100, 100, 50, 150);
	
	/*
	background						   Color(offset_lerp*220,25,50,25)
	white                            = Vector4Box(255, 255, 255, 250, ),
	text_color                       = Vector4Box(120, 120, 0  , 250, ),
	text_color_overridden            = Vector4Box(200, 200, 0  , 255, ),
	text_color_hot                   = Vector4Box(230, 230, 200, 255, ),
	text_color_active                = Vector4Box(100, 255, 100, 255, ),
	text_color_description           = Vector4Box(150, 150, 150, 255, ),
	text_color_option_default_value  = Vector4Box(50,  150, 50 , 255, ),
	text_color_option_overridden     = Vector4Box(100, 255, 100, 255, ),
	text_color_option_hot            = Vector4Box(200, 255, 200, 255, ),
	row_highlight_color              = Vector4Box(100, 100, 50 , 150, ),
	dotdotdot                        = Vector4Box(75,  75,  0  , 255, ),
	*/

	s_themes[1][COLOR_Background]      = Color(50, 50, 150, 200);
	s_themes[1][COLOR_ItemTitle]       = Color(120, 120, 120, 255);
	s_themes[1][COLOR_ItemTitleHot]    = Color(200, 200, 200, 255);
	s_themes[1][COLOR_ItemTitleActive] = Color(200, 200, 255, 255);
	
	s_themes[2][COLOR_Background]      = Color(100, 100, 100, 200);
	s_themes[2][COLOR_ItemTitle]       = Color(150, 150, 150, 255);
	s_themes[2][COLOR_ItemTitleHot]    = Color(200, 200, 200, 255);
	s_themes[2][COLOR_ItemTitleActive] = Color(255, 255, 220, 255);

	s_theme_index = 0;
	memcpy(s_theme, s_themes[s_theme_index], sizeof(s_theme));

	for (size_t i = 0; i < FONT_count; i++) {
		if (s_fonts[i] == 0) {
			return false;
		}
	}

	return true;
}

static void on_change_theme(DebuginatorItem* item, void* value, const char* value_title) {
	s_theme_index = *(int*)value;
	memcpy(s_theme, s_themes[s_theme_index], sizeof(s_theme));
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
		static int string_indices[3] = { 0, 1, 2 };
		static const char* string_titles[5] = { "String A", "String B", "String C", "String D", "String E" };
		debuginator_create_array_item(debuginator, NULL, "Debuginator/Help",
			"The Debuginator is a debug menu. With a keyboard, you open it with Right Arrow and close it with Left Arrow. You use those keys, plus Up/Down arrows to navigate. Right Arrow is also used to change value on a menu item.", NULL, NULL,
			NULL, NULL, 0, 0);
		debuginator_create_array_item(debuginator, NULL, "Debuginator/String Test",
			"Change color theme of The Debuginator.", NULL, NULL,
			string_titles, NULL, 5, 0);
	}
	debuginator_create_bool_item(debuginator, "SimpleBool 1", "Change a bool.", &data->mybool);
	debuginator_create_bool_item(debuginator, "Folder/SimpleBool 2", "Change a bool.", &data->mybool);
	//debuginator_create_bool_item(debuginator, "Folder/SimpleBool 3", "Change a bool.", &data->mybool);
	debuginator_create_bool_item(debuginator, "Folder/Subfolder/SimpleBool 4 with a really long long title", "Change a bool.", &data->mybool);

	debuginator_new_folder_item(debuginator, NULL, "Folder 2", 0);

	debuginator_initialize(debuginator);
}

float draw_item(TheDebuginator* debuginator, DebuginatorItem* item, Vector2 offset, bool hot, GuiHandle gui) {
	//draw_rect_filled(gui, offset, Vector2(100, 30), Color(200, 100, 50, 200));
/*
	if (!debuginator->hot_item->is_folder && debuginator->hot_item->leaf.is_active) {
		if (debuginator->hot_item == item) {
			for (size_t i = 0; i < item->leaf.num_values; i++) {
				draw_rect_filled(gui, Vector2(0, offset.y + (i+1) * 30), Vector2(3, 20), Color(0, 255, 0, 255));
			}
		}
	}
	else if (debuginator->hot_item->parent == item->parent) {
		draw_rect_filled(gui, Vector2(0, offset.y), Vector2(3, 20), Color(0, 255, 0, 255));
	}*/

	if (item->is_folder) {
		if (debuginator->hot_item == item) {
			draw_rect_filled(gui, Vector2(0, offset.y - 5), Vector2(400, 30), s_theme[COLOR_LineHighlight]);
		}

		unsigned color_index = item == debuginator->hot_item ? COLOR_ItemTitleActive : (hot ? COLOR_ItemTitleHot : COLOR_FolderTitle);
		draw_text(gui, item->title, offset, s_fonts[FONT_ItemTitle], s_theme[color_index]);
		offset.x += 20;
		DebuginatorItem* child = item->folder.first_child;
		while (child) {
			offset.y += 30;
			offset.y = draw_item(debuginator, child, offset, debuginator->hot_item == child, gui);
			child = child->next_sibling;
		}
	}
	else {
		if (debuginator->hot_item == item && (!item->leaf.is_active || item->leaf.num_values == 0)) {
			draw_rect_filled(gui, Vector2(debuginator->openness * 400 - 400, offset.y - 5), Vector2(400, 30), s_theme[COLOR_LineHighlight]);
		}

		bool is_overriden = item->leaf.active_index != 0;
		unsigned default_color_index = is_overriden ? COLOR_ItemTitleOverridden : COLOR_ItemTitle;
		unsigned color_index = item == debuginator->hot_item && !item->leaf.is_active ? COLOR_ItemTitleActive : (hot ? COLOR_ItemTitleHot : default_color_index);
		draw_text(gui, item->title, offset, s_fonts[FONT_ItemTitle], s_theme[color_index]);

		// Draw quick representation of value
		if (item->leaf.num_values > 0) {
			Vector2 value_offset = offset;
			value_offset.x = 300 + debuginator->openness * 400 - 400;
			draw_text(gui, item->leaf.value_titles[item->leaf.active_index], value_offset, s_fonts[FONT_ItemTitle], s_theme[default_color_index]);
		}
		
		if (item->leaf.is_active) {
			offset.x += 20;

			char description_line[64];
			const char* description = item->leaf.description;
			while (description) {
				description = word_wrap(gui, s_fonts[FONT_ItemDescription], description, 350 - offset.x, description_line, 64);

				offset.y += 30;
				draw_text(gui, description_line, offset, s_fonts[FONT_ItemDescription], s_theme[COLOR_ItemDescription]);
			}

			for (size_t i = 0; i < item->leaf.num_values; i++) {
				offset.y += 30;

				if (debuginator->hot_item == item && item->leaf.hot_index == i) {
					draw_rect_filled(gui, Vector2(0, offset.y - 5), Vector2(400, 30), s_theme[COLOR_LineHighlight]);
				}

				const char* value_title = item->leaf.value_titles[i];
				bool value_hot = i == item->leaf.hot_index;
				bool value_overridden = i == item->leaf.active_index;
				unsigned value_color_index = value_hot ? COLOR_ItemValueHot : (value_overridden ? COLOR_ItemTitleOverridden : COLOR_ItemValueDefault);
				draw_text(gui, value_title, offset, s_fonts[value_hot ? FONT_ItemTitle : FONT_ItemTitle], s_theme[value_color_index]);
			}
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
	for (size_t i = 0; i < 400000; i++) {
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

		float dt = 0.1;
		debuginator_update(&debuginator, dt * 0.5f);

		// update theme opacity
		for (size_t i = 0; i < COLOR_NumColors; i++) {
			s_theme[i].a = s_themes[s_theme_index][i].a * debuginator.openness;
		}

		frame_begin(gui, i);

		// bouncing boxes
		draw_rect_filled(gui, Vector2(i % 500, i % 500), Vector2(300, 300), Color(255, 255, 255, 255));
		draw_rect_filled(gui, Vector2((i / 2) % 350, i % 700), Vector2(200, 200), Color(0, 0, 0, 255));

		float width = 400;
		Vector2 offset(-width * (1 - debuginator.openness), 0);

		// Background
		draw_rect_filled(gui, offset, Vector2(400, res_y), s_theme[COLOR_Background]);

		draw_item(&debuginator, debuginator.root, offset, true, gui);

		frame_end(gui);
		SDL_Delay(1 / dt);
	}

	for (size_t i = 0; i < 16; i++) { // TODO unhardcode
		if (s_fonts[i] != 0x0) { // TODO invalid handle
			unregister_font_template(gui, s_fonts[i]);
		}
	}

	destroy_gui(gui);
	return 0;
}
