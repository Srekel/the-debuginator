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

}

static void unittest_debug_menu_run() {
	UnitTestData& testdata = g_testdata;
	DebuginatorItem item_buffer[16];
	TheDebuginator debuginator = debuginator_create(item_buffer, sizeof(item_buffer) / sizeof(item_buffer[0]));
	unittest_debug_menu_setup(&debuginator, &testdata);
	debuginator_initialize(&debuginator);
	
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

		ASSERT(testdata.simplebool_target == false);
	}
	{
		// Nothing changes if nothing happens
		DebuginatorInput input = {};
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(testdata.simplebool_target == false);
	}
	{
		// Activating SimpleBool 1 changes bool
		DebuginatorInput input = {};
		input.activate = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(testdata.simplebool_target == true);
	}
	{
		// Activating SimpleBool 1's second option changes bool to false
		DebuginatorInput input = {};
		input.activate = true;
		input.go_sibling_down = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(testdata.simplebool_target == false);
	}
	{
		// Activating SimpleBool 1's first option changes bool to true
		DebuginatorInput input = {};
		input.activate = true;
		input.go_sibling_down = true;
		debug_menu_handle_input(&debuginator, &input);
		ASSERT(testdata.simplebool_target == true);
	}
	/*
	{
		DebuginatorInput input = {};
		input.go_child = true;
		debug_menu_handle_input(&debuginator, &input);
		//ASSERT(debuginator.hot_item == debuginator.root->children[0]);
		//ASSERT(!debuginator.hot_item->is_active);
		
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
	*/
	/*{
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
