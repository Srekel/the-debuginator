#include <SDL.h>
#include <SDL_ttf.h>

// Roughly matches Stingray's Gui API

typedef intptr_t GuiHandle;
typedef int FontTemplate;

struct Vector2 {
	float x;
	float y;
};

struct Color {
	char r;
	char g;
	char b;
	char a;
};

GuiHandle create_gui();

void draw_text(GuiHandle gui, const char* text, Vector2 position, FontTemplate font);
void draw_rect_filled(GuiHandle gui, const char* text, Vector2 position, Vector2 size, Color color);

