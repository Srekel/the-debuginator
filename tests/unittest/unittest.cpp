#include <stdio.h>
#include <assert.h>
#include <string.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

struct UnitTestData
{
	char errors[256][256];
	unsigned error_index;
	unsigned num_tests;

	bool simplebool_target;

	char stringtest[256];
};

static UnitTestData g_testdata = {};

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
#define ASSERT unittest_debuginator_assert

#define DEBUGINATOR_debug_print printf

#include "../../the_debuginator.h"

static void unittest_on_item_changed_stringtest(DebuginatorItem* item, void* value, const char* value_title) {
	(void)value_title;
	const char** string_ptr = (const char**)value;
	UnitTestData* callback_data = (UnitTestData*)item->user_data; // same as &g_testdata
	strncpy_s(callback_data->stringtest, *string_ptr, strlen(*string_ptr));
}

static void unittest_debug_menu_setup(TheDebuginator* debuginator) {
	debuginator_create_bool_item(debuginator, "SimpleBool 1", "Change a bool.", &g_testdata.simplebool_target);
	debuginator_create_bool_item(debuginator, "Folder/SimpleBool 2", "Change a bool.", &g_testdata.simplebool_target);
	debuginator_create_bool_item(debuginator, "Folder/SimpleBool 3", "Change a bool.", &g_testdata.simplebool_target);
	debuginator_create_bool_item(debuginator, "Folder/SimpleBool 4 with a really long long title", "Change a bool.", &g_testdata.simplebool_target);

	debuginator_new_folder_item(debuginator, NULL, "Folder 2", 0);

	static const char* string_values[3] = { "gamestring 1", "gamestring 2", "gamestring 3"};
	static const char* string_titles[3] = { "First value", "Second one", "This is the third." };
	debuginator_create_array_item(debuginator, NULL, "Folder 2/String item",
		"Do it", unittest_on_item_changed_stringtest, &g_testdata,
		string_titles, string_values, 3, sizeof(string_values[0]));

}

static void unittest_debug_menu_run() {
	UnitTestData& testdata = g_testdata;
	DebuginatorItem item_buffer[16];
	TheDebuginator debuginator = debuginator_create(item_buffer, sizeof(item_buffer) / sizeof(item_buffer[0]));
	unittest_debug_menu_setup(&debuginator);
	debuginator_initialize(&debuginator);

	printf("\n");
	printf("Setup errors found: %u/%u\n", 
		testdata.error_index, testdata.num_tests);

	if (testdata.error_index > 0) {
		printf("Errors found during setup, exiting.\n");
		return;
	}

	testdata.num_tests = 0;

	{
		// Are our expectations after setup correct?
		DebuginatorItem* expected_hot_item = debuginator_get_item(&debuginator, NULL, "SimpleBool 1", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
		ASSERT(expected_hot_item->leaf.is_active == false);

		ASSERT(testdata.simplebool_target == false);
	}
	{
		// Nothing changes if nothing happens
		DebuginatorInput input = {};
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(testdata.simplebool_target == false);
	}
	{
		// Going to child activates SimpleBool 1
		DebuginatorInput input = {};
		input.go_child = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(debuginator.hot_item->leaf.is_active == true);
	}
	{
		// Going to child changes SimpleBool 1 bool
		DebuginatorInput input = {};
		input.go_child = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(testdata.simplebool_target == true);
	}
	{
		// Going to child and sibling at the same time changes SimpleBool 1's to second option and sets bool to false
		DebuginatorInput input = {};
		input.go_child = true;
		input.go_sibling_down = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(testdata.simplebool_target == false);
	}
	{
		// Going to child SimpleBool 1's first option changes bool to true
		DebuginatorInput input = {};
		input.go_child = true;
		input.go_sibling_down = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(testdata.simplebool_target == true);
	}
	{
		// Going to parent inactivates item
		DebuginatorInput input = {};
		input.go_parent = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(debuginator.hot_item->leaf.is_active == false);
	}
	{
		// Going to parent does nothing
		DebuginatorInput input = {};
		input.go_parent = true;
		debug_menu_handle_input(&debuginator, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(&debuginator, NULL, "SimpleBool 1", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Going down goes to Folder
		DebuginatorInput input = {};
		input.go_sibling_down = true;
		debug_menu_handle_input(&debuginator, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(&debuginator, NULL, "Folder", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Going down goes to Folder 2
		DebuginatorInput input = {};
		input.go_sibling_down = true;
		debug_menu_handle_input(&debuginator, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(&debuginator, NULL, "Folder 2", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Going down wraps to SimpleBool 1
		DebuginatorInput input = {};
		input.go_sibling_down = true;
		debug_menu_handle_input(&debuginator, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(&debuginator, NULL, "SimpleBool 1", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Go to Folder
		DebuginatorInput input = {};
		input.go_sibling_down = true;
		debug_menu_handle_input(&debuginator, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(&debuginator, NULL, "Folder", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Going to child goes to SimpleBool 2
		DebuginatorInput input = {};
		input.go_child = true;
		debug_menu_handle_input(&debuginator, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(&debuginator, NULL, "Folder/SimpleBool 2", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Going to child activates SimpleBool 2
		DebuginatorInput input = {};
		input.go_child = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(debuginator.hot_item->leaf.is_active == true);
	}
	{
		// Going to child changes SimpleBool 2 bool
		DebuginatorInput input = {};
		input.go_child = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(testdata.simplebool_target == true);
	}
	{
		// Going to parent inactivates item
		DebuginatorInput input = {};
		input.go_parent = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(debuginator.hot_item->leaf.is_active == false);
	}
	{
		// Going to sibling works as expected
		DebuginatorInput input = {};
		input.go_sibling_down = true;
		debug_menu_handle_input(&debuginator, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(&debuginator, NULL, "Folder/SimpleBool 3", false);
		ASSERT(expected_hot_item == debuginator.hot_item);

		debug_menu_handle_input(&debuginator, &input);
		expected_hot_item = debuginator_get_item(&debuginator, NULL, "Folder/SimpleBool 4 with a really long long title", false);
		ASSERT(expected_hot_item == debuginator.hot_item);

		debug_menu_handle_input(&debuginator, &input);
		expected_hot_item = debuginator_get_item(&debuginator, NULL, "Folder/SimpleBool 2", false);
		ASSERT(expected_hot_item == debuginator.hot_item);

		debug_menu_handle_input(&debuginator, &input);
		expected_hot_item = debuginator_get_item(&debuginator, NULL, "Folder/SimpleBool 3", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Go to Folder 2/String item
		DebuginatorInput input = {};
		input.go_parent = true;
		debug_menu_handle_input(&debuginator, &input);
		debug_menu_handle_input(&debuginator, &input);
		input.go_parent = false;
		input.go_sibling_down = true;
		input.go_child = true;
		debug_menu_handle_input(&debuginator, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(&debuginator, NULL, "Folder 2/String item", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Going to child activates string item
		DebuginatorInput input = {};
		input.go_child = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(debuginator.hot_item->leaf.is_active == true);
	}
	{
		// Going to child changes string
		DebuginatorInput input = {};
		input.go_child = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(strcmp(testdata.stringtest, "gamestring 1") == 0);
	}
	{
		// Going down goes to next string
		DebuginatorInput input = {};
		input.go_sibling_down = true;
		debug_menu_handle_input(&debuginator, &input);
		DebuginatorItem* expected_hot_item = debuginator_get_item(&debuginator, NULL, "Folder 2/String item", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Going to child changes string
		DebuginatorInput input = {};
		input.go_child = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(strcmp(testdata.stringtest, "gamestring 2") == 0);
	}
	{
		// Overwrite the item with another item
		debuginator_create_bool_item(&debuginator, "Folder 2/String item", "Change a bool.", &g_testdata.simplebool_target);
	}
	{
		// Activate it, we should still be on the second value, so bool should turn to false
		ASSERT(testdata.simplebool_target == true);
		DebuginatorInput input = {};
		input.go_child = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(testdata.simplebool_target == false);
	}
	{
		// Remove item
		debuginator_remove_item(&debuginator, "Folder 2/String item");
		DebuginatorItem* expected_null_item = debuginator_get_item(&debuginator, NULL, "Folder 2/String item", false);
		ASSERT(expected_null_item == NULL);

		DebuginatorItem* expected_hot_item = debuginator_get_item(&debuginator, NULL, "Folder 2", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}
	{
		// Set hot item
		debuginator_set_hot_item(&debuginator, "Folder/SimpleBool 2");

		DebuginatorItem* expected_hot_item = debuginator_get_item(&debuginator, NULL, "Folder/SimpleBool 2", false);
		ASSERT(expected_hot_item == debuginator.hot_item);
	}

	printf("Run errors found:   %u/%u\n",
		testdata.error_index, testdata.num_tests);

	printf("\n");
	if (testdata.error_index == 0) {
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
