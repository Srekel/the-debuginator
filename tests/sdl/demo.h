#pragma once

#include <stdlib.h>

#include "gui.h"

struct TheDebuginator;

struct DemoSlide {
	const char* texts[8];
	float anim_times[8];
	int num_texts;
	int num_texts_visible;
};

struct DemoData {
	GuiHandle gui;
	TheDebuginator* debuginator;
	FontTemplateHandle font_handle;

	DemoSlide slides[32];
	int num_slides;
	int current_slide;
};


DemoData* demo_init(GuiHandle gui, TheDebuginator* debuginator);
void demo_update(DemoData* demo_data, float dt, Vector2 offset);
void demo_trigger_next(DemoData* demo_data);
