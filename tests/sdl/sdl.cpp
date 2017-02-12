#include <stdio.h>
#include <assert.h>
#include <string.h>

struct UnitTestData
{
	char errors[256][256];
	unsigned error_index;
	unsigned num_tests;

	bool simplebool_target;
	bool simplebool_on_change;
	int simple_bool_counter;
};

static UnitTestData g_testdata = {};

static void unittest_debuginator_assert(bool test) {
	if (g_testdata.error_index == 256) {
		assert(false);
	}
	if (!test) {
		memcpy(g_testdata.errors[g_testdata.error_index++], "LOL", 4);
	}
	//DebugBreak();
}

#define DEBUGINATOR_assert unittest_debuginator_assert
#define ASSERT unittest_debuginator_assert

#include "../../the_debuginator.h"


//
//DebuginatorItemDefinition debug_menu_define(DebuginatorItemType type,
//	const char** value_titles, const char** value_descriptions, void** values, unsigned num_values) {
//
//	DebuginatorItemDefinition item = {};
//	item.type = type;
//	item.value_titles = value_titles;
//	item.value_descriptions = value_descriptions;
//	item.array_values = *values;
//	item.num_values = num_values;
//	return item;
//}
//
//DebuginatorItemDefinition debug_menu_define_bool(const char** value_descriptions = 0x0) {
//	static unsigned bool_values[2] = { 1, 0 };
//	static const char* bool_titles[2] = { "True, False" };
//	return debug_menu_define(ItemType_Array, bool_titles, value_descriptions, 0x0, bool_values, 2);
//}



static void on_item_changed_simplebool(DebuginatorItemDefinition* item, void* value, const char* /*value_title*/) {
	UnitTestData* testdata_userdata = ((UnitTestData*)item->user_data);
	testdata_userdata->simplebool_on_change = *((bool*)value);
	testdata_userdata->simple_bool_counter++;
}

static void debug_menu_setup(TheDebuginator* debuginator, UnitTestData* testdata) {

}


#include <SDL.h>
#include <SDL_ttf.h>

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


	DebuginatorItemDefinition item_buffer[4];
	DebuginatorItemDefinition* child_buffer[4];
	TheDebuginator debuginator = debuginator_create(item_buffer, 4, child_buffer, 4);
	debug_menu_setup(&debuginator, &g_testdata);
	debuginator.hot_item = debuginator.root;
	debuginator_validate(&debuginator);

	SDL_Rect rectangle;
	rectangle.x = 0;
	rectangle.y = 0;
	rectangle.w = 50;
	rectangle.h = 50;
	SDL_RenderFillRect(renderer, &rectangle);
	SDL_RenderDrawRect(renderer, &rectangle);
	SDL_RenderPresent(renderer);

	SDL_SetRenderDrawColor(renderer, 0, 0, 50, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	SDL_Event event;
	for (size_t i = 0; i < 500; i++) {
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				i = 10000;
			}
		}
		//SDL_RenderFillRect(renderer, &rectangle);
		//SDL_RenderDrawRect(renderer, &rectangle);
		SDL_RenderCopy(renderer, text_texture, NULL, &rectangle);
		SDL_RenderPresent(renderer);
		SDL_Delay(10);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
