#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef _MSC_VER
#pragma warning( disable: 4464 4548 4710 4820 )
#endif

#ifdef __clang__

#endif
typedef struct UnitTestData
{
	char errors[256][256];
	unsigned error_index;
	unsigned num_tests;

	bool simplebool_target;
	bool generatedbool_target;

	char stringtest[256];
} UnitTestData;

static UnitTestData g_testdata;

static void unittest_debuginator_assert(bool test) {
	++g_testdata.num_tests;
	if (g_testdata.error_index == 256) {
		assert(false);
	}
	if (!test) {
		memcpy(g_testdata.errors[g_testdata.error_index++], "LOL", 4);
	}
	//DebugBreak();
}

#define DEBUGINATOR_assert unittest_debuginator_assert
#define DEBUGINATOR_static_assert unittest_debuginator_assert
#define ASSERT unittest_debuginator_assert

#define DEBUGINATOR_debug_print printf
#define DEBUGINATOR_IMPLEMENTATION

#include "../../the_debuginator.h"

// Debuginator Callbacks
#pragma warning(suppress: 4100) // Unreferenced param
void draw_text(const char* text, DebuginatorVector2* position, DebuginatorColor* color, DebuginatorFont* font, void* userdata) {
}

#pragma warning(suppress: 4100) // Unreferenced param
void draw_rect(DebuginatorVector2* position, DebuginatorVector2* size, DebuginatorColor* color, void* userdata) {
}

#pragma warning(suppress: 4100) // Unreferenced param
void word_wrap(const char* text, DebuginatorFont font, float max_width, int* row_count, int* row_lengths, int row_lengths_buffer_size, void* app_userdata) {
	*row_count = 0;
}

#pragma warning(suppress: 4100) // Unreferenced param
DebuginatorVector2 text_size(const char* text, DebuginatorFont* font, void* userdata) {
	DebuginatorVector2 size = {0, 0};
	return size;
}

// Item callback
static void unittest_on_item_changed_stringtest(DebuginatorItem* item, void* value, const char* value_title, void* app_userdata) {
	(void)value_title;
	(void)app_userdata;
	const char** string_ptr = (const char**)value;
	UnitTestData* callback_data = (UnitTestData*)item->user_data; // same as &g_testdata

#ifdef __cplusplus
	strncpy_s(callback_data->stringtest, *string_ptr, strlen(*string_ptr));
#else
	strncpy_s(callback_data->stringtest, sizeof(callback_data->stringtest), *string_ptr, strlen(*string_ptr));
#endif
}

static void unittest_debug_menu_setup(TheDebuginator* debuginator) {
	debuginator_create_bool_item(debuginator, "SimpleBool 1", "Change a bool.", &g_testdata.simplebool_target);
	debuginator_create_bool_item(debuginator, "Folder/SimpleBool 2", "Change a bool.", &g_testdata.simplebool_target);
	debuginator_create_bool_item(debuginator, "Folder/SimpleBool 3", "Change a bool.", &g_testdata.simplebool_target);
	debuginator_create_bool_item(debuginator, "Folder/SimpleBool 4 with a really really long title", "Change a bool.", &g_testdata.simplebool_target);

	debuginator_create_folder_item(debuginator, NULL, "Folder 2");

	static const char* string_values[3] = { "gamestring 1", "gamestring 2", "gamestring 3"};
	static const char* string_titles[3] = { "First value", "Second one", "This is the third." };
	debuginator_create_array_item(debuginator, NULL, "Folder 2/String item",
		"Do it", unittest_on_item_changed_stringtest, &g_testdata,
		string_titles, (void*)string_values, 3, sizeof(string_values[0]));
}

static void unittest_debug_menu_run(void) {

	memset(&g_testdata, 0, sizeof(g_testdata));
	UnitTestData* testdata = &g_testdata;


	int memory_arena_capacity = 1024 * 1024 * 1;
	char* memory_arena = (char*)malloc(memory_arena_capacity);
	TheDebuginatorConfig config;
	debuginator_get_default_config(&config);
	config.memory_arena = memory_arena;
	config.memory_arena_capacity = memory_arena_capacity;
	config.draw_rect = draw_rect;
	config.draw_text = draw_text;
	config.word_wrap = word_wrap;
	config.text_size = text_size;
	config.app_user_data = &g_testdata;
	config.size.x = 500;
	config.size.y = 1000;
	config.screen_resolution.x = 500;
	config.screen_resolution.y = 1000;
	config.focus_height = 0.3f;
	config.create_default_debuginator_items = false;

	TheDebuginator debuginator;
	TheDebuginator* thed = &debuginator; // Lazy shorthand
	debuginator_create(&config, thed);

	unittest_debug_menu_setup(thed);

	printf("\n");
	printf("Setup errors found: %u/%u\n",
		testdata->error_index, testdata->num_tests);

	if (testdata->error_index > 0) {
		printf("Errors found during setup, exiting.\n");
		return;
	}

	testdata->num_tests = 0;

	DebuginatorItem* sb1_item = debuginator_get_item(thed, NULL, "SimpleBool 1", false);
	DebuginatorItem* sb2_item = debuginator_get_item(thed, NULL, "Folder/SimpleBool 2", false);
	DebuginatorItem* sb3_item = debuginator_get_item(thed, NULL, "Folder/SimpleBool 3", false);
	DebuginatorItem* sb4_item = debuginator_get_item(thed, NULL, "Folder/SimpleBool 4 with a really really long title", false);
	DebuginatorItem* str_item = debuginator_get_item(thed, NULL, "Folder 2/String item", false);

	{
		// Are our expectations after setup correct?
		ASSERT(sb1_item != NULL);
		ASSERT(sb2_item != NULL);
		ASSERT(sb3_item != NULL);
		ASSERT(sb4_item != NULL);
		ASSERT(str_item != NULL);

		ASSERT(debuginator.root->folder.num_visible_children = 5);

		DebuginatorItem* expected_hot_item = debuginator_get_item(thed, NULL, "SimpleBool 1", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
		ASSERT(expected_hot_item->leaf.is_expanded == false);

		ASSERT(testdata->simplebool_target == false);
		ASSERT(testdata->generatedbool_target == false);
	}
	{
		// Can we add and remove stuff?
		char item_name[64] = { 0 };
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				sprintf_s(item_name, 64, "Game/Test%02d/GameBool%02d", i, j);
				debuginator_create_bool_item(thed, item_name, "Generated bool item.", &g_testdata.generatedbool_target);
			}
		}

		ASSERT(debuginator.root->folder.num_visible_children == 6);

		DebuginatorItem* game_item = debuginator_get_item(thed, NULL, "Game", false);
		ASSERT(game_item->folder.num_visible_children == 10);

		DebuginatorItem* remove_item = debuginator_get_item(thed, NULL, "Game/Test01", false);
		debuginator_remove_item(thed, remove_item);
		ASSERT(game_item->folder.num_visible_children == 9);

		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				sprintf_s(item_name, 64, "Game/Test%02d/GameBool%02d", i, j);
				debuginator_create_bool_item(thed, item_name, "Generated bool item.", &g_testdata.generatedbool_target);
			}
		}

		ASSERT(game_item->folder.num_visible_children == 10);

		remove_item = debuginator_get_item(thed, NULL, "Game/Test01", false);
		debuginator_remove_item(thed, remove_item);

		ASSERT(game_item->folder.num_visible_children == 9);
	}

	/*
	{
		debuginator_move_to_next_leaf(thed, false);
		// Going to child activates SimpleBool 1
		DebuginatorInput input = {0};
		input.move_to_child = true;
		debug_menu_handle_input(thed, &input);
		ASSERT(debuginator.hot_item->leaf.is_expanded == true);
	}
	{
		// Going to child changes SimpleBool 1 bool
		DebuginatorInput input = {0};
		input.move_to_child = true;
		debug_menu_handle_input(thed, &input);
		ASSERT(testdata->simplebool_target == false);
	}
	{
		// Going to child and sibling at the same time changes SimpleBool 1's to second option and sets bool to true
		DebuginatorInput input = {0};
		input.move_to_child = true;
		input.move_sibling_next = true;
		debug_menu_handle_input(thed, &input);
		ASSERT(testdata->simplebool_target == true);
	}
	{
		// Going to child SimpleBool 1's first option changes bool to false
		DebuginatorInput input = {0};
		input.move_to_child = true;
		input.move_sibling_next = true;
		debug_menu_handle_input(thed, &input);
		ASSERT(testdata->simplebool_target == false);
	}
	{
		// Going to parent inactivates item
		DebuginatorInput input = {0};
		input.move_to_parent = true;
		debug_menu_handle_input(thed, &input);
		ASSERT(debuginator.hot_item->leaf.is_expanded == false);
	}
	{
		// Going to parent does nothing
		DebuginatorInput input = {0};
		input.move_to_parent = true;
		debug_menu_handle_input(thed, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(thed, NULL, "SimpleBool 1", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Going down goes to Folder
		DebuginatorInput input = {0};
		input.move_sibling_next = true;
		debug_menu_handle_input(thed, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(thed, NULL, "Folder", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Going down goes to Folder 2
		DebuginatorInput input = {0};
		input.move_sibling_next = true;
		debug_menu_handle_input(thed, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(thed, NULL, "Folder 2", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Going down wraps to SimpleBool 1
		DebuginatorInput input = {0};
		input.move_sibling_next = true;
		debug_menu_handle_input(thed, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(thed, NULL, "SimpleBool 1", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Go to Folder
		DebuginatorInput input = {0};
		input.move_sibling_next = true;
		debug_menu_handle_input(thed, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(thed, NULL, "Folder", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Going to child goes to SimpleBool 2
		DebuginatorInput input = {0};
		input.move_to_child = true;
		debug_menu_handle_input(thed, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(thed, NULL, "Folder/SimpleBool 2", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Going to child activates SimpleBool 2
		DebuginatorInput input = {0};
		input.move_to_child = true;
		debug_menu_handle_input(thed, &input);
		ASSERT(debuginator.hot_item->leaf.is_expanded == true);
	}
	{
		// Going to child changes SimpleBool 2 bool
		DebuginatorInput input = {0};
		input.move_to_child = true;
		debug_menu_handle_input(thed, &input);
		ASSERT(testdata->simplebool_target == false);
	}
	{
		// Going to parent inactivates item
		DebuginatorInput input = {0};
		input.move_to_parent = true;
		debug_menu_handle_input(thed, &input);
		ASSERT(debuginator.hot_item->leaf.is_expanded == false);
	}
	{
		// Going to sibling works as expected
		DebuginatorInput input = {0};
		input.move_sibling_next = true;
		debug_menu_handle_input(thed, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(thed, NULL, "Folder/SimpleBool 3", false);
		ASSERT(expected_hot_item == debuginator.hot_item);

		debug_menu_handle_input(thed, &input);
		expected_hot_item = debuginator_get_item(thed, NULL, "Folder/SimpleBool 4 with a really long long title", false);
		ASSERT(expected_hot_item == debuginator.hot_item);

		debug_menu_handle_input(thed, &input);
		expected_hot_item = debuginator_get_item(thed, NULL, "Folder/SimpleBool 2", false);
		ASSERT(expected_hot_item == debuginator.hot_item);

		debug_menu_handle_input(thed, &input);
		expected_hot_item = debuginator_get_item(thed, NULL, "Folder/SimpleBool 3", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Go to Folder 2/String item
		DebuginatorInput input = {0};
		input.move_to_parent = true;
		debug_menu_handle_input(thed, &input);
		debug_menu_handle_input(thed, &input);
		input.move_to_parent = false;
		input.move_sibling_next = true;
		input.move_to_child = true;
		debug_menu_handle_input(thed, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(thed, NULL, "Folder 2/String item", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Going to child activates string item
		DebuginatorInput input = {0};
		input.move_to_child = true;
		debug_menu_handle_input(thed, &input);
		ASSERT(debuginator.hot_item->leaf.is_expanded == true);
	}
	{
		// Going to child changes string
		DebuginatorInput input = {0};
		input.move_to_child = true;
		debug_menu_handle_input(thed, &input);
		ASSERT(strcmp(testdata->stringtest, "gamestring 1") == 0);
	}
	{
		// Going down goes to next string
		DebuginatorInput input = {0};
		input.move_sibling_next = true;
		debug_menu_handle_input(thed, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(thed, NULL, "Folder 2/String item", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Going to child changes string
		DebuginatorInput input = {0};
		input.move_to_child = true;
		debug_menu_handle_input(thed, &input);
		ASSERT(strcmp(testdata->stringtest, "gamestring 2") == 0);
	}
	{
		// Overwrite the item with another item
		debuginator_create_bool_item(thed, "Folder 2/String item", "Change a bool.", &testdata->simplebool_target);
	}
	{
		// Activate it, we should still be on the second value, so bool should turn to true
		ASSERT(testdata->simplebool_target == false);
		DebuginatorInput input = {0};
		input.move_to_child = true;
		debug_menu_handle_input(thed, &input);
		ASSERT(testdata->simplebool_target == true);
	}
	{
		// Remove item
		debuginator_remove_item_by_path(thed, "Folder 2/String item");
		DebuginatorItem* expected_null_item = debuginator_get_item(thed, NULL, "Folder 2/String item", false);
		ASSERT(expected_null_item == NULL);

		DebuginatorItem* expected_hot_item = debuginator_get_item(thed, NULL, "Folder 2", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Set hot item
		debuginator_set_hot_item(thed, "Folder/SimpleBool 2");

		DebuginatorItem* expected_hot_item = debuginator_get_item(thed, NULL, "Folder/SimpleBool 2", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}

	*/

	printf("Run errors found:   %u/%u\n",
		testdata->error_index, testdata->num_tests);

	printf("\n");
	if (testdata->error_index == 0) {
		printf("No errors found, YAY!\n");
	}
	else {
		printf("U are teh sux.\n");
	}
}

int main(int argc, char **argv)
{
	(void)(argc, argv);
	unittest_debug_menu_run();

	while (true)
	{

	}
	return 0;
}
