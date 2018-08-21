#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <string.h>

#include "gui.h"

struct FontTemplate {
	TTF_Font* font;
	//const char* font;
	//int size;
};

struct Gui {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_GameController* game_controllers[32];

	FontTemplate font_templates[8];
	int font_template_count;

	TextureHandle texture_handles[8];
	SDL_Texture* textures[8];
	int num_textures;
};

static Gui guis[1];
static int gui_count = 0;


GuiHandle gui_create_gui(int resx, int resy, const char* window_title, bool vsync_on) {
	if (gui_count > 0) {
		return (GuiHandle)nullptr;
	}

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
		SDL_Quit();
		return (GuiHandle)nullptr;
	}

	if (TTF_Init() != 0) {
		SDL_Quit();
		return (GuiHandle)nullptr;
	}

	SDL_Window* window = SDL_CreateWindow(window_title,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resx, resy, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		SDL_Quit();
		return (GuiHandle)nullptr;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | (SDL_RENDERER_PRESENTVSYNC & (vsync_on ? 0xFFFFFFFF : 0)));
	if (renderer == NULL) {
		SDL_Quit();
		return (GuiHandle)nullptr;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	Gui* gui = &guis[gui_count++];
	SDL_memset(gui, 0, sizeof(*gui));
	gui->renderer = renderer;
	gui->window = window;

	// Set up gamepads. Note: Doesn't handle adding while running.
	// If you don't have this file, make sure it gets copied from 3rdparty\SDL_GameControllerDB-master
	SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
    SDL_JoystickEventState(SDL_ENABLE);
	int num_joysticks = SDL_NumJoysticks();
	for( int i=0; i < num_joysticks; i++ ) {
		if (SDL_IsGameController(i)) {
			gui->game_controllers[i] = SDL_GameControllerOpen(i);
		}
	}

	return (GuiHandle)gui;
}

void gui_destroy_gui(GuiHandle gui_handle) {
	Gui* gui = (Gui*)gui_handle;

	SDL_DestroyWindow(gui->window);
	SDL_Quit();
}

void gui_frame_begin(GuiHandle gui_handle) {
	Gui* gui = (Gui*)gui_handle;
	SDL_SetRenderDrawColor(gui->renderer, 30, 30, 30, 255);
	SDL_RenderClear(gui->renderer);
}

void gui_frame_end(GuiHandle gui_handle){
	Gui* gui = (Gui*)gui_handle;
	SDL_RenderPresent(gui->renderer);
}

TextureHandle gui_load_texture(GuiHandle gui_handle, const char* texture_filename) {
	Gui* gui = (Gui*)gui_handle;
	SDL_Texture* texture = IMG_LoadTexture(gui->renderer, texture_filename);
	gui->textures[gui->num_textures] = texture;
	return (TextureHandle)gui->num_textures++;
}

void gui_draw_texture(GuiHandle gui_handle, TextureHandle handle, Vector2 position, Vector2 size) {
	Gui* gui = (Gui*)gui_handle;
	SDL_Texture* texture = gui->textures[handle];
	SDL_Rect texture_rect;
	texture_rect.x = (int)position.x;
	texture_rect.y = (int)position.y;
	texture_rect.w = (int)size.x;
	texture_rect.h = (int)size.y;
	SDL_RenderCopy(gui->renderer, texture, NULL, &texture_rect);
}

FontTemplateHandle gui_register_font_template(GuiHandle gui_handle, const char* font, int size) {
	TTF_Font* ttf_font = TTF_OpenFont(font, size);
	if (ttf_font == NULL) {
		return 0;
	}

	Gui* gui = (Gui*)gui_handle;
	FontTemplate* font_template = &gui->font_templates[gui->font_template_count++];
	font_template->font = ttf_font;

	return (FontTemplateHandle)font_template;
}

void gui_unregister_font_template(GuiHandle gui_handle, FontTemplateHandle font_handle) {
	(void)gui_handle;
	FontTemplate* font_template = (FontTemplate*)font_handle;
	TTF_CloseFont(font_template->font);
	font_template->font = 0;
}

void gui_draw_text(GuiHandle gui_handle, const char* text, Vector2 position, FontTemplateHandle font_handle, Color color) {
	Gui* gui = (Gui*)gui_handle;
	SDL_Color* text_color = (SDL_Color*)&color;
	FontTemplate* font_template = (FontTemplate*)font_handle;
	SDL_Surface* text_surface = TTF_RenderText_Blended(font_template->font, text, *text_color);
	if (text_surface == NULL) {
		// TODO Assert
		return;
	}

	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(gui->renderer, text_surface);
	if (text_texture == NULL) {
		// TODO Assert
		return;
	}

	SDL_FreeSurface(text_surface);

	SDL_Rect rectangle;
	rectangle.x = (int)position.x;
	rectangle.y = (int)position.y;
	TTF_SizeText(font_template->font, text, &rectangle.w, &rectangle.h);

	SDL_SetRenderDrawColor(gui->renderer, 255, 255, 255, 255);
	SDL_RenderCopy(gui->renderer, text_texture, NULL, &rectangle);

	SDL_DestroyTexture(text_texture);
}

void gui_draw_rect_filled(GuiHandle gui_handle, Vector2 position, Vector2 size, Color color) {
	Gui* gui = (Gui*)gui_handle;
	SDL_Rect rect;
	rect.x = (int)position.x;
	rect.y = (int)position.y;
	rect.w = (int)size.x;
	rect.h = (int)size.y;

	SDL_SetRenderDrawColor(gui->renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(gui->renderer, &rect);
}

void gui_word_wrap(GuiHandle gui_handle, const char* text, FontTemplateHandle font_handle, float max_width, int* row_count, int* row_lengths, int row_lengths_buffer_size) {
	(void)gui_handle;
	FontTemplate* font_template = (FontTemplate*)font_handle;

	const char* current_line = text;
	const char* current_word = text;
	const char* current_char = text;
	char line[256];
	while (*current_char != '\0') {
		bool found_newline = false;
		while (*current_char != '\0') {
			if (*current_char == ' ') {
				// We found a word end.
				// Include all trailing spaces in this word
				while (*current_char == ' ') {
					++current_char;
				}
				break;
			}

			if (*current_char == '\n') {
				// We found a new line
				++current_char;
				found_newline = true;
				break;
			}

			++current_char;
		}

		memcpy(line, current_line, current_char - current_line);
		line[current_char - current_line] = '\0';
		int width;
		if (TTF_SizeText(font_template->font, line, &width, NULL) != 0) {
			break;
		}

		bool line_too_long = width >= max_width;
		if (line_too_long || found_newline) {
			bool word_longer_than_line = current_word == current_line;
			if (found_newline) {
				row_lengths[*row_count] = (int)(current_char - current_line - 1);
				++*row_count;
				current_word = current_char;
				current_line = current_char;
			}
			else if (word_longer_than_line) {
				row_lengths[*row_count] = (int)(current_char - current_line);
				++*row_count;
				current_word = current_char;
				current_line = current_char;
			}
			else {
				// Move current word to next line.
				row_lengths[*row_count] = (int)(current_word - current_line);
				++*row_count;

				//current_word = current_char;
				current_line = current_word;
			}

			if (*row_count == row_lengths_buffer_size) {
				break;
			}
		}
		else {
			// Current word fit.
			current_word = current_char;
		}
	}

	if (*current_line != '\0') {
		// Add last line
		row_lengths[*row_count] = (int)(current_char - current_line);
		++*row_count;
	}
}

Vector2 gui_text_size(GuiHandle gui_handle, const char* text, FontTemplateHandle font_handle) {
	Gui* gui = (Gui*)gui_handle;
	(void)gui;
	FontTemplate* font_template = (FontTemplate*)font_handle;

	if (text[0] == '\0') {
		return Vector2(0, 0);
	}

	int x, y;
	TTF_SizeText(font_template->font, text, &x, &y);
	Vector2 out_text_size((float)x, (float)y);
	return out_text_size;
}

Vector2 gui_get_window_size(GuiHandle gui_handle) {
	Gui* gui = (Gui*)gui_handle;
	int w, h;
	SDL_GetWindowSize(gui->window, &w, &h);
	return Vector2((float)w, (float)h);
}

SDL_GameController** gui_get_controllers(GuiHandle gui_handle) {
	Gui* gui = (Gui*)gui_handle;
	return gui->game_controllers;
}
