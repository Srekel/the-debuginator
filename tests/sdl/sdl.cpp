#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <SDL.h>
#include <SDL_ttf.h>

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

void draw_item(DebuginatorItem* item, SDL_Point offset, SDL_Renderer* renderer) {
	SDL_Rect rect;
	rect.x = offset.x;
	rect.y = offset.y;
	rect.w = 100;
	rect.h = 30;
	SDL_RenderFillRect(renderer, &rect);

	if (item->is_folder) {
		offset.x += 20;
		item = item->folder.first_child;
		while (item) {
			offset.y += 35;
			draw_item(item, offset, renderer);
			item = item->next_sibling;
		}
	}
}

int main(int argc, char **argv)
{
	(void)(argc, argv);

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Quit();
		return 1;
	}

	if (TTF_Init() != 0) {
		SDL_Quit();
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Debuginator SDL example", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	if (window == NULL) {
		SDL_Quit();
		return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		SDL_Quit();
		return 1;
	}

	TTF_Font* font = TTF_OpenFont("LiberationMono-Regular.ttf", 22);
	if (font == NULL) {
		SDL_Quit();
		return 1;
	}

	SDL_Color text_color = { 150, 150, 0, 0 };
	SDL_Surface* text_surface = TTF_RenderText_Solid(font, "LOLOL", text_color);
	if (text_surface == NULL) {
		SDL_Quit();
		return 1;
	}

	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	if (text_texture == NULL) {
		SDL_Quit();
		return 1;
	}

	SDL_FreeSurface(text_surface);
	text_surface = NULL;
	

	DebuginatorItem item_buffer[256];
	TheDebuginator debuginator = debuginator_create(item_buffer, 256);

	GameData data = { 0 };
	debug_menu_setup(&debuginator, &data);

	SDL_Rect bgrectangle;
	bgrectangle.x = 000;
	bgrectangle.y = 000;
	bgrectangle.w = 200;
	bgrectangle.h = 500;
	SDL_Rect rectangle;
	rectangle.x = 100;
	rectangle.y = 200;
	rectangle.w = 250;
	rectangle.h = 50;
	/*SDL_RenderFillRect(renderer, &rectangle);
	SDL_RenderDrawRect(renderer, &rectangle);
	SDL_RenderPresent(renderer);
*/

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_Event event;
	for (size_t i = 0; i < 500; i++) {
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				i = 10000;
			}
		}

		SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 150, 255, 0, 50);
		SDL_RenderFillRect(renderer, &bgrectangle);

		SDL_SetRenderDrawColor(renderer, 150, 255, 0, 150);
		SDL_Point item_offset;
		item_offset.x = 0;
		item_offset.y = 0;
		draw_item(debuginator.root, item_offset, renderer);
		
		SDL_RenderCopy(renderer, text_texture, NULL, &rectangle);
		SDL_RenderPresent(renderer);
		SDL_Delay(10);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
