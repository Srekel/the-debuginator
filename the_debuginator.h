enum DebugMenuItemType {
	ItemType_Undefined,
	// ItemType_Single,
	ItemType_Array,
	ItemType_ArrayOfPtrs,
	// ItemType_Preset,
	ItemType_Count
};

static int bool_values[2] = {1, 0};
static const char* bool_titles[2] = {"True, False"};

struct DebugMenuItemDefinition {
	void* user_data;
	DebugMenuItemType type;
	const char* title;
	const char* description;

	const char** value_titles;
	const char** value_descriptions;
	void** values;
	unsigned num_values;
	unsigned array_item_size;
	void (*get_values)(DebugMenuItemDefinition* item,
		void** out_values, const char*** out_value_titles, const char*** out_value_descriptions,
		unsigned* out_num_values, unsigned* out_array_item_size);

	void (*on_item_changed)(DebugMenuItemDefinition* item, void* value, const char* value_title);

	DebugMenuItemDefinition* children;
	unsigned num_children;

	DebugMenuItemDefinition* preset_items;

	unsigned num_preset_items;
};

struct DebugMenuInput {
	bool activate;
	bool go_sibling_down;
	bool go_sibling_down;
	bool go_parent;
	bool go_child;
};

DebugMenuItemDefinition debug_menu_define(DebugMenuItemType type,
	const char** value_titles, const char** value_descriptions, void* values, unsigned num_values) {

	DebugMenuItemDefinition item;
	item.type = type;
	item.value_titles = value_titles;
	item.value_descriptions = value_descriptions;
	item.values = values;
	item.num_values = num_values;
	return item;
}

DebugMenuItemDefinition debug_menu_define_bool(const char** value_descriptions = 0x0) {
	return debug_menu_define(ItemType_Array, bool_titles, value_descriptions, 0x0, bool_values, 2);
}

void debug_menu_activate(DebugMenuItemDefinition* item, unsigned option_index) {
	if (item->type == ItemType_ArrayOfPtrs) {
		item.on_item_changed(item, values[option_index], value_titles[option_index]);
	} else {
		item.on_item_changed(item, ((char*)values) + option_index * item->array_item_size, value_titles[option_index]);
	}
}

void debug_menu_handle_input() {

}

///////////// UNIT TEST ////////////////

#define DEBUG_MENU_UNIT_TEST
#if DEBUG_MENU_UNIT_TEST

void unittest_on_item_changed(DebugMenuItemDefinition* item, void* value, const char* value_title) {

}

void unittest_debug_menu_setup() {
	DebugMenuItemDefinition bool_test = debug_menu_define_bool();
	bool_test.user_data = "LOL";
	bool_test.title = "bool_simple";
	bool_test.description = "Simple boolean variable.";
	bool_test.on_item_changed = on_item_changed;
}

void unittest_debug_menu_run() {
	unittest_debug_menu_setup();

	debug_menu_handle_input(input);
}

#endif
