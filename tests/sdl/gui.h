#include <SDL.h>
#include <SDL_ttf.h>

// Roughly matches Stingray's Gui API
// Very simple API solely for this particular demo

typedef intptr_t GuiHandle;
typedef intptr_t FontTemplateHandle;
//typedef DebuginatorVector2 Vector2;


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

GuiHandle gui_create_gui(int resx, int resy, const char* window_title);
void gui_destroy_gui(GuiHandle gui);

void gui_frame_begin(GuiHandle gui_hande, int lolframe);
void gui_frame_end(GuiHandle gui_hande);

FontTemplateHandle gui_register_font_template(GuiHandle gui, const char* font, int size);
void gui_unregister_font_template(GuiHandle gui_handle, FontTemplateHandle font_handle);

void gui_draw_text(GuiHandle gui, const char* text, Vector2 position, FontTemplateHandle font, Color color);
void gui_draw_rect_filled(GuiHandle gui_handle, Vector2 position, Vector2 size, Color color);

const char* gui_word_wrap(GuiHandle gui_handle, FontTemplateHandle font_handle, const char* text, int max_width, char* buffer, int buffer_size);
