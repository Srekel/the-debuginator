#include <SDL.h>
#include <SDL_ttf.h>

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

	FontTemplate font_templates[8];
	int font_template_count;
};

static Gui guis[1];
static int gui_count = 0;


GuiHandle gui_create_gui(int resx, int resy, const char* window_title, bool vsync_on) {
	if (gui_count > 0) {
		return (GuiHandle)nullptr;
	}

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Quit();
		return (GuiHandle)nullptr;
	}

	if (TTF_Init() != 0) {
		SDL_Quit();
		return (GuiHandle)nullptr;
	}

	SDL_Window* window = SDL_CreateWindow(window_title,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resx, resy, SDL_WINDOW_OPENGL);
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

	return (GuiHandle)gui;
}

void gui_destroy_gui(GuiHandle gui_handle) {
	Gui* gui = (Gui*)gui_handle;

	SDL_DestroyWindow(gui->window);
	SDL_Quit();
}

void gui_frame_begin(GuiHandle gui_handle) {
	Gui* gui = (Gui*)gui_handle;
	SDL_SetRenderDrawColor(gui->renderer, 0, 0, 0, 255);
	SDL_RenderClear(gui->renderer);
}

void gui_frame_end(GuiHandle gui_handle){
	Gui* gui = (Gui*)gui_handle;
	SDL_RenderPresent(gui->renderer);
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

const char* gui_word_wrap(GuiHandle gui_handle, FontTemplateHandle font_handle, const char* text, int max_width, char* buffer, int buffer_size) {
	(void)gui_handle;

	// This is really stupid but it works for now.
	// TODO: Fix dropping words bug.
	FontTemplate* font_template = (FontTemplate*)font_handle;
	char temp_buffer[256];
	const char* next_word = strchr(text, ' ');
	while (true) {
		if (next_word) {
			strncpy_s(temp_buffer, 256, text, next_word - text);
		}
		else {
			strncpy_s(temp_buffer, 256, text, strlen(text));
		}

		int width;
		if (TTF_SizeText(font_template->font, temp_buffer, &width, NULL) != 0) {
			return next_word;
		}

		if (width >= max_width) {
			return next_word;
		}

		strcpy_s(buffer, buffer_size, temp_buffer);
		if (next_word == NULL) {
			return NULL;
		}

		next_word = strchr(next_word + 1, ' ');
	}
}

void gui_word_wrap2(GuiHandle gui_handle, FontTemplateHandle font_handle, const char* text, int max_width, char** buffer, int buffer_size) {
	(void)gui_handle;
	FontTemplate* font_template = (FontTemplate*)font_handle;

	// Step until find a word end (' ' '\n')
	// Is it longer than max_width? If so, copy into buffer, move to next line.

	// "Multiple strings."

	char** pointers = buffer;
	char* line = (char*)(buffer + (buffer_size / 8 / 2)); // Reserve first half of buffer for the pointers
	*pointers = line;
	const char* text_word = text;
	const char* text_char = text;
	char* current_line_pos = line;
	char* current_word_pos = line;
	while (*text_char != '\0') {
		while (*text_char != '\0') {
			*current_line_pos++ = *text_char++;
			if (*text_char == ' ') {
				break;
				//strncpy_s(line, current_line_pos, text_char - current_line_pos);
			}

		}

		// We found a word end.
		int width;
		if (TTF_SizeText(font_template->font, line, &width, NULL) != 0) {
			break;
		}
		
		if (width >= max_width) 
		{
			// Current line is too big
			if (current_word_pos == line) {
				// Word is too long to fit on a line
				*++line = '\0';
				pointers++;
				*pointers = ++line;
				text_word = text_char;
			}
			else {
				// Move current word to next line.
				*current_word_pos = '\0';
				line = current_word_pos;
			}

			while (*text_char == ' ') {
				++text_char;
			}
		}
		else {
			// Current word fit.
			text_word = text_char;
		}
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
