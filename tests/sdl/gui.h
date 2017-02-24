#include <SDL.h>
#include <SDL_ttf.h>

// Roughly matches Stingray's Gui API
// Very simple API solely for this particular demo

typedef intptr_t GuiHandle;
typedef intptr_t FontTemplateHandle;

struct Vector2 {
	Vector2() : x(0), y(0) {}
	Vector2(float x, float y) : x(x), y(y) {}
	float x;
	float y;
};

struct Color {
	Color() : r(0), g(0), b(0), a(0) {}
	Color(char r, char g, char b, char a) : r(r), g(g), b(b), a(a) {}
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

GuiHandle create_gui(int resx, int resy, const char* window_title);
void destroy_gui(GuiHandle gui);

void frame_begin(GuiHandle gui_hande, int lolframe);
void frame_end(GuiHandle gui_hande);

FontTemplateHandle register_font_template(GuiHandle gui, const char* font, int size);
void draw_text(GuiHandle gui, const char* text, Vector2 position, FontTemplateHandle font, Color color);
void draw_rect_filled(GuiHandle gui_handle, Vector2 position, Vector2 size, Color color);

