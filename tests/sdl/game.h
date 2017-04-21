#pragma once

#include <stdlib.h>

#include "gui.h"

struct TheDebuginator;

struct GameBox {
	GameBox() {
	}
	void randomize() {
		pos.x = pos.y = 0;
		size.x = 50 + 100 * ((float)rand()) / RAND_MAX;
		size.y = 50 + 100 * ((float)rand()) / RAND_MAX;
		velocity.x = 5 + 150 * ((float)rand()) / RAND_MAX;
		velocity.y = 5 + 150 * ((float)rand()) / RAND_MAX;
		color.r = (unsigned char)(50 + 100 * ((float)rand()) / RAND_MAX);
		color.g = (unsigned char)(50 + 100 * ((float)rand()) / RAND_MAX);
		color.b = (unsigned char)(50 + 100 * ((float)rand()) / RAND_MAX);
		color.a = (unsigned char)(150 + 100 * ((float)rand()) / RAND_MAX);
	}
	Vector2 pos;
	Vector2 size;
	Vector2 velocity;
	Color color;
};

struct GameData {
	GuiHandle gui;
	bool mybool;
	bool gamebool;
	bool load_test;
	bool draw_boxes;
	char mystring[256];
	GameBox boxes[256];
	int boxes_n;
	char box_string[32];
	Vector2 window_size;
};


GameData* game_init(GuiHandle gui, TheDebuginator* debuginator);
void game_update(GameData* game_data, float dt);
