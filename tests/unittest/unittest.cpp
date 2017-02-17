#include <stdio.h>
#include <assert.h>
#include <string.h>

struct UnitTestData
{
	char errors[256][256];
	unsigned error_index;
	unsigned num_tests;

	bool simplebool_target;
	bool simplebool_on_change;
	int simple_bool_counter;
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

#include "../../the_debuginator.h"


//
//DebuginatorItem debug_menu_define(DebuginatorItemType type,
//	const char** value_titles, const char** value_descriptions, void** values, unsigned num_values) {
//
//	DebuginatorItem item = {};
//	item.type = type;
//	item.value_titles = value_titles;
//	item.value_descriptions = value_descriptions;
//	item.array_values = *values;
//	item.num_values = num_values;
//	return item;
//}
//
//DebuginatorItem debug_menu_define_bool(const char** value_descriptions = 0x0) {
//	static unsigned bool_values[2] = { 1, 0 };
//	static const char* bool_titles[2] = { "True, False" };
//	return debug_menu_define(ItemType_Array, bool_titles, value_descriptions, 0x0, bool_values, 2);
//}



static void on_item_changed_simplebool(DebuginatorItem* item, void* value, const char* /*value_title*/) {
	UnitTestData* testdata_userdata = ((UnitTestData*)item->user_data);
	testdata_userdata->simplebool_on_change = *((bool*)value);
	testdata_userdata->simple_bool_counter++;
}

static void unittest_debug_menu_setup(TheDebuginator* debuginator, UnitTestData* testdata) {
	debuginator_create_bool_item(debuginator, "SimpleBool 1", "Change a bool.", &g_testdata.simplebool_target);
	debuginator_create_bool_item(debuginator, "Folder/SimpleBool 2", "Change a bool.", &g_testdata.simplebool_target);
	debuginator_create_bool_item(debuginator, "Folder/SimpleBool 3", "Change a bool.", &g_testdata.simplebool_target);
	debuginator_new_folder_item(debuginator, NULL, "Folder2", 0);
	debuginator_create_bool_item(debuginator, "Folder2/SimpleBool 4", "Change a bool.", &g_testdata.simplebool_target);

	/*
	{
		static bool bool_values[2] = { 1, 0 };
		static const char* bool_titles[2] = { "True", "False" };


		DEBUGINATOR_create_bool_item(debuginator,
			"AI/Debug paths", "Show AI pathfinding",
			on_item_changed_simplebool, g_testdata.simplebool_target);
	}

	{
		static bool bool_values[2] = { 1, 0 };
		static const char* bool_titles[2] = { "True", "False" };

		DebuginatorItem* bool_test = debuginator_new_leaf_item(debuginator);
		bool_test->type = DebuginatorItemType_Array;
		bool_test->update_type = DebuginatorUpdateType_Never;
		bool_test->title = "bool_simple";
		bool_test->description = "Simple boolean variable.";
		bool_test->value_titles = bool_titles;
		bool_test->array_values = bool_values;
		bool_test->num_values = 2;
		bool_test->array_item_size = sizeof(bool_values[0]);
		bool_test->target = &testdata->simplebool_target;
		bool_test->on_item_changed = on_item_changed_simplebool;
		bool_test->user_data = testdata;
		debuginator_add_child(root, bool_test);
	}
	{
		static bool bool_array[2] = { 1, 0 };
		static bool* bool_values[2] = { bool_array, bool_array+1};
		static const char* bool_titles[2] = { "True", "False" };

		DebuginatorItem* bool_test = debuginator_new_leaf_item(debuginator);
		bool_test->type = DebuginatorItemType_ArrayOfPtrs;
		bool_test->update_type = DebuginatorUpdateType_Never;
		bool_test->title = "bool_simple array of ptrs";
		bool_test->description = "Simple boolean variable 2.";
		bool_test->value_titles = bool_titles;
		bool_test->array_of_ptr_values = (void**)bool_values;
		bool_test->num_values = 2;
		bool_test->array_item_size = sizeof(bool_values[0][0]);
		bool_test->target = &testdata->simplebool_target;
		bool_test->on_item_changed = on_item_changed_simplebool;
		bool_test->user_data = testdata;

		debuginator_add_child(root, bool_test);
	}*/
}

static void unittest_debug_menu_run() {
	UnitTestData& testdata = g_testdata;
	DebuginatorItem item_buffer[16];
	TheDebuginator debuginator = debuginator_create(item_buffer, sizeof(item_buffer) / sizeof(item_buffer[0]));
	unittest_debug_menu_setup(&debuginator, &testdata);
	
	printf("Setup errors found: %u/%u\n", 
		testdata.error_index, testdata.num_tests);
	if (testdata.error_index > 0) {
		printf("Errors found during setup, exiting.\n");
		return;
	}

	testdata.num_tests = 0;

	{
		ASSERT(testdata.simplebool_target == false);
		DebuginatorInput input = {};
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(testdata.simplebool_target == false);

	}

	/*{
		DebuginatorInput input = {};
		input.go_child = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(debuginator.hot_item == debuginator.root->children[0]);
		ASSERT(!debuginator.hot_item->is_active);
		
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(debuginator.hot_item == debuginator.root->children[0]);
		ASSERT(debuginator.hot_item->is_active);
	}
	{
		DebuginatorInput input = {};
		input.activate = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(testdata.simplebool_target == true);
		ASSERT(testdata.simplebool_on_change == true);
		ASSERT(testdata.simple_bool_counter == 1);
	}
	{
		DebuginatorInput input = {};
		input.go_sibling_down = true;
		input.activate = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(testdata.simplebool_target == false);
		ASSERT(testdata.simplebool_on_change == false);
		ASSERT(testdata.simple_bool_counter == 2);
	}
	{
		DebuginatorInput input = {};
		input.go_sibling_down = true;
		input.activate = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(testdata.simplebool_target == true);
		ASSERT(testdata.simplebool_on_change == true);
		ASSERT(testdata.simple_bool_counter == 3);
	}
*/
	printf("Run errors found:   %u/%u\n",
		testdata.error_index, testdata.num_tests);
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
