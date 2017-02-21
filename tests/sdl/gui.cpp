#include <SDL.h>
#include <SDL_ttf.h>

#include "gui.h"

struct Gui {
	SDL_Renderer* renderer;
};

static Gui guis[1];
static int gui_count = 0;

GuiHandle create_gui() {
	if (gui_count > 0) {
		return (GuiHandle)nullptr;
	}

	return (GuiHandle)&guis[gui_count++];
}


void draw_text(Gui gui, const char* text, Vector2 position, FontTemplate font) {

}

void draw_rect_filled(Gui gui, const char* text, Vector2 position, Vector2 size, Color color) {

}
