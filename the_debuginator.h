#ifndef INCLUDE_THE_DEBUGINATOR_H
#define INCLUDE_THE_DEBUGINATOR_H

/*
# THE DEBUGINATOR

## Usage
	// Create
	static void on_debuginator_save(DebuginatorItemDefinition* item, void* value, const char* value_title) {
		game_save_debug_setting(item->path, value_title);
	}

	DebuginatorItemDefinition item_buffer[256];
	TheDebuginator debuginator = debuginator_create(item_buffer, 256, on_debuginator_save);

	// Load settings at startup
	void parse_command_line(int argc, char** argv) {
		if (argc > 2 && strcmp(argv[1], "--do_the_bool") == 0) {
			debuginator_pre_load_setting(debuginator, "Test/Bool with function callback", "True");
		}
	}

	// Add debug menu item for reals
	// (can happen later, and multiple times if you want to override something)
	static void on_item_changed(DebuginatorItemDefinition* item, void* value, const char* value_title) {
		bool new_value = *(bool*)value;
		printf("Item %s changed value to (%d) %s", item->title, value, value_title);
	}

	DEBUGINATOR_create_bool_item(debuginator, "Test/Bool with function callback",
	"Calls on_item_changed when user changes the value in the menu.",
		on_item_changed, NULL);

	// Validate configuration
	// debuginator_validate(&debuginator);
*/

#ifndef DEBUGINATOR_assert
#include <assert.h>
#define DEBUGINATOR_assert assert;
#endif

#ifndef DEBUGINATOR_memcpy
#include <cstring.h>
#define DEBUGINATOR_memcpy memcpy
#endif

#ifndef DEBUGINATOR_FREE_LIST_CAPACITY
#define DEBUGINATOR_FREE_LIST_CAPACITY 256
#endif

enum DebuginatorItemType {
	DebuginatorItemType_Undefined,
	// ItemType_Single,
	DebuginatorItemType_Folder,
	DebuginatorItemType_Array,
	DebuginatorItemType_ArrayOfPtrs,
};

/*
enum DebuginatorUpdateType {
	// Set content at init, will never change again.
	DebuginatorUpdateType_Never,

	// Will update when item is expanded or when cursor enters it
	DebuginatorUpdateType_OnShow,

	// Will update every frame when cursor is on it or its children
	DebuginatorUpdateType_WhenHot,
};
*/

struct DebuginatorItem;
struct DebuginatorFolderData {
	DebuginatorItem* first_child;
	DebuginatorItem* hot_child;
};

typedef void (*DebuginatorOnItemChangedCallback)(DebuginatorItem* item, void* value, const char value_title);

struct DebuginatorLeafData {
	void* user_data;
	DebuginatorDescription_t description;

	bool is_active;
	char* hot_value;
	unsigned hot_index;

	DebuginatorTitle_t* value_titles;
	DebuginatorDescription_t* value_descriptions;
	union {
		void* array_values;
		void** array_of_ptr_values;
	};

	unsigned num_values;
	unsigned array_element_size;

	void(*get_array_values)(DebuginatorItem* item,
		void** out_values,
		DebuginatorTitle_t** out_value_titles,
		DebuginatorDescription_t** out_value_descriptions,
		unsigned* out_num_values, unsigned* out_array_element_size);

	void(*get_array_of_ptr_values)(DebuginatorItem* item,
		void*** out_values,
		DebuginatorTitle_t** out_value_titles,
		DebuginatorDescription_t** out_value_descriptions,
		unsigned* out_num_values, unsigned* out_array_element_size);

	void* target;
	DebuginatorOnItemChangedCallback on_item_changed;
};

struct DebuginatorItem {
	DebuginatorItemType type;

	char path[128];
	char* title;
	unsigned title_length;

	DebuginatorItem* next_sibling;
	DebuginatorItem* parent;

	union {
		DebuginatorLeafData leaf;
		DebuginatorFolderData folder;
	};
};

struct TheDebuginator {
	DebuginatorItem* root;
	DebuginatorItem* hot_item;

	unsigned item_buffer_capacity;
	unsigned item_buffer_size;
	DebuginatorItem* item_buffer;

	unsigned free_list_size;
	unsigned free_list[DEBUGINATOR_FREE_LIST_CAPACITY];
};

TheDebuginator debuginator_create(DebuginatorItem* item_buffer, unsigned item_buffer_capacity) {
	TheDebuginator debuginator = {};
	debuginator.item_buffer_capacity = item_buffer_capacity;
	debuginator.item_buffer = item_buffer;
	memset(item_buffer, 0, sizeof(DebuginatorItem) * item_buffer_capacity);
	DebuginatorItem* item = debuginator_new_folder_item(&debuginator, "Menu Root");
	debuginator->root = item;
	return debuginator;
}

DebuginatorItem* debuginator_new_leaf_item(TheDebuginator* debuginator) {
	DEBUGINATOR_assert(debuginator->item_buffer_size < debuginator->item_buffer_capacity);
	return &debuginator->item_buffer[debuginator->item_buffer_size++];
}

DebuginatorItem* debuginator_get_free_item(TheDebuginator* debuginator) {
	DebuginatorItem* item;
	if (debuginator->free_list_size > 0) {
		unsigned free_index = debuginator->free_list[--debuginator->free_list_size];
		DEBUGINATOR_assert(0 <= free_index && free_index < debuginator->item_buffer_capacity);
		item = &debuginator->item_buffer[free_index];
	}
	else {
		DEBUGINATOR_assert(debuginator->item_buffer_size < debuginator->item_buffer_capacity);
		item = &debuginator->item_buffer[debuginator->item_buffer_size++];
	}

	memset(item, 0, sizeof(*item));
	return item;
}

DebuginatorItem* debuginator_find_item_recursively(DebuginatorItem* item, const char* path) {
	char* first_slash = strchr(path, '/');
	if (first_slash == NULL) {
		// Reached a leaf item
		if (strcmp(path, item->title) == 0) {
			return item;
		}

		return NULL;
	}

	#define DEBUGINATOR_min(a, b) (((a) < (b)) ? (a) : (b))
	if (first_slash - path != strlen(item->title) || memcmp(path, item->title, first_slash - path) != 0) {
		return NULL;
	}

	DebuginatorItem* child = item->first_child;
	while (child) {
		DebuginatorItem* found_item = debuginator_find_item_recursively(child, first_slash);
		if (found_item != NULL) {
			return found_item;
		}

		child = child->next_sibling;
	}

	return NULL;
}

DebuginatorItem* debuginator_find_item(TheDebuginator* debuginator, const char* path) {
	DebuginatorItem* child = debuginator->root->first_child;
	while (child) {
		DebuginatorItem* found_item = debuginator_find_item_recursively(child, path);
		if (found_item != NULL) {
			return found_item;
		}

		child = child->next_sibling;
	}

	return NULL;
}

DebuginatorItem* debuginator_create_path_recursively(TheDebuginator* debuginator, const char* path) {

}

DebuginatorItem* debuginator_create_path(TheDebuginator* debuginator, const char* path) {
	char* first_slash = strchr(path, '/');
	DebuginatorItem* child = debuginator->root->first_child;
	while (child) {
		if (memcmp(child->title, path, DEBUGINATOR_min(first_slash - path, strlen(child->title) == 0)
		DebuginatorItem* found_item = debuginator_find_item_recursively(child, path);
		if (found_item != NULL) {
			return found_item;
		}

		child = child->next_sibling;
	}
}

DebuginatorItem* debuginator_new_folder_item(TheDebuginator* debuginator, const char* path, DebuginatorItem* parent) {
	DEBUGINATOR_assert(debuginator->item_buffer_size < debuginator->item_buffer_capacity);
	DebuginatorItem* folder_item;
	if (debuginator->free_list_size > 0) {
		folder_item = &debuginator->item_buffer[--debuginator->free_list_size];
	}
	else {
	 	folder_item = &debuginator->item_buffer[debuginator->item_buffer_size++];
	}

	folder_item->type = DebuginatorItemType_Folder;
	folder_item->title = title;
	folder_item->user_data = user_data;

	if (path != NULL) {
		DebuginatorItem* parent = locate
	}

	return folder_item;
}

DebuginatorItem* debuginator_create_array_item(TheDebuginator* debuginator,
	const char* path, const char* description,
	DebuginatorOnItemChangedCallback callback, void* target, unsigned element_size,
	const char* value_titles, void* array_values, unsigned num_values, void* user_data) {

	DebuginatorItem* item = NULL;
	DebuginatorItem* parent = debuginator->root;
	const char* temp_path = path;
	while (temp_path) {
		const char* next_slash = strchr(temp_path, '/');

		DebuginatorItem* parent_child = parent->first_child;
		while (parent_child) {
			unsigned path_part_length = next_slash ? next_slash - temp_path : strlen(temp_path);
			unsigned title_length = strlen(parent_child->title); // strlen :(
			unsigned num_characters = (path_part_length > title_length) ? title_length : path_part_length;
			if (memcmp(parent_child->title, temp_path, num_characters * sizeof(char)) == 0) {
				parent = parent_child;
				break;
			}

			parent_child = parent_child->next_sibling;
		}

		if (next_slash == NULL) {
			// Found the last part of the path
			DebuginatorItem* parent_child = parent->first_child;
			while (parent_child) {
				if (strcmp(parent_child->title, )
			}
		}
		else {
			// Found a folder

			if (parent == NULL) {
				// Parent item doesn't exist yet
				parent = debuginator_new_folder_item(debuginator, path, path - next_slash, parent);
			}
		}
	}
	// Find slash character between item and parent
	unsigned path_index = DEBUGINATOR_strlen(path);
	while(path_index > 0 && path[path_index] != '/') {
		--path_index;
	}

	DebuginatorItem* item = debuginator_find_item(debuginator, path);
	if (item == NULL) {
		// Item has never existed, create new one
		item = debuginator_get_free_item(debuginator);

		if (path_index > 0) {
			// Item has a parent, find it or create it (recursively)
			DEBUGINATOR_assert(path_index < 128);
			char parent_path[128];
			strncpy(parent_path, path, path_index);
			parent_path[path_index] = '\0';
			parent = debuginator_create_folder_item(debuginator, parent_path);
			item->parent = parent;
		}
	}

	item->type = DebuginatorItemType_Array;
	item->num_values = num_values;
	item->array_values = array_values;
	item->array_element_size = element_size;
	item->value_titles = value_titles;
	item->user_data = user_data;

	if (item->hot_index >= num_values) {
		item->hot_index = num_values - 1;
	}
}

void debuginator_add_child(DebuginatorItem* parent, DebuginatorItem* child) {
	DEBUGINATOR_assert(parent->children_size < parent->children_capacity);
	parent->children[parent->children_size++] = child;
	child->parent = parent;
}

static void debuginator_validate_item(DebuginatorItem* item) {
	DEBUGINATOR_assert(item->type != DebuginatorItemType_Undefined);

	if (item->type == DebuginatorItemType_Folder) {
		DEBUGINATOR_assert(item->children != 0);
		//DEBUGINATOR_assert(item->children_capacity > 0);
		//DEBUGINATOR_assert(item->title != NULL); // TODO typedef support
		DEBUGINATOR_assert(item->target == 0);
		DEBUGINATOR_assert(item->on_item_changed == 0);
		DEBUGINATOR_assert(item->num_values == 0);
		DEBUGINATOR_assert(item->value_titles == 0);
		DEBUGINATOR_assert(item->value_descriptions == 0);
		DEBUGINATOR_assert(item->array_values == 0);
		DEBUGINATOR_assert(item->array_of_ptr_values == 0);
		DEBUGINATOR_assert(item->array_element_size == 0);
		DEBUGINATOR_assert(item->get_array_values == 0);
		DEBUGINATOR_assert(item->get_array_of_ptr_values == 0);

		for (unsigned i = 0; i < item->children_capacity; i++) {
			debuginator_validate_item(item->children[i]);
		}
	}
	else if (item->type == DebuginatorItemType_Array) {
		DEBUGINATOR_assert(item->children == NULL);
		DEBUGINATOR_assert(item->children_capacity == 0);
		DEBUGINATOR_assert(item->target != NULL
			|| item->on_item_changed != NULL);
		DEBUGINATOR_assert(item->num_values > 0);
		DEBUGINATOR_assert(item->value_titles != NULL);
		//DEBUGINATOR_assert(item->value_descriptions == NULL);
		DEBUGINATOR_assert(item->array_values != NULL);
		DEBUGINATOR_assert(item->array_element_size > 0);
	}
	else if (item->type == DebuginatorItemType_ArrayOfPtrs) {
		DEBUGINATOR_assert(item->children == NULL);
		DEBUGINATOR_assert(item->children_capacity == 0);
		DEBUGINATOR_assert(item->target != NULL
			|| item->on_item_changed != NULL);
		DEBUGINATOR_assert(item->num_values > 0);
		DEBUGINATOR_assert(item->value_titles != NULL);
		//DEBUGINATOR_assert(item->value_descriptions == NULL);
		DEBUGINATOR_assert(item->array_of_ptr_values != NULL);
		DEBUGINATOR_assert(item->array_element_size > 0);
	}

	if (item->update_type != DebuginatorUpdateType_Never) {
		DEBUGINATOR_assert(item->get_array_values == 0
			|| item->get_array_of_ptr_values == 0);
	}
}

void debuginator_validate(TheDebuginator* debuginator) {
	//printf("Root item:   %llx\n", &debuginator->root);
	//printf("Hot item:    %llx\n", debuginator->hot_item);
	//printf("Active item: %llx\n", debuginator->active_item);
	DEBUGINATOR_assert(debuginator->hot_item == debuginator->root);
	//DEBUGINATOR_assert(debuginator->active_item == debuginator->root);
	debuginator_validate_item(debuginator->root);
}

struct DebuginatorInput {
	bool activate;
	bool go_sibling_up;
	bool go_sibling_down;
	bool go_parent;
	bool go_child;
};

static void debuginator_activate(DebuginatorItem* item) {
	void* value;
	if (item->type == DebuginatorItemType_ArrayOfPtrs) {
		void** ptr_to_value = (item->array_of_ptr_values + item->hot_index);
		value = *ptr_to_value;
	}
	else {
		value = ((char*)item->array_values) + item->hot_index * item->array_element_size;
	}

	if (item->target) {
		memcpy(item->target, value, item->array_element_size);
	}

	if (item->on_item_changed) {
		item->on_item_changed(item, value, item->value_titles[item->hot_index]);
	}
}

void debug_menu_handle_input(TheDebuginator* debuginator, DebuginatorInput* input) {
	if (input->go_child) {
		if (debuginator->hot_item->type != DebuginatorItemType_Folder && !debuginator->hot_item->is_active) {
			debuginator->hot_item->is_active = true;
		}
		else {
			DebuginatorItem* parent = debuginator->hot_item;
			if (parent->type == DebuginatorItemType_Folder) {
				debuginator->hot_item = parent->children[0];
			}
		}
	}

	if (input->go_sibling_down) {
		if (debuginator->hot_item->is_active) {
			if (++debuginator->hot_item->hot_index == debuginator->hot_item->num_values) {
				debuginator->hot_item->hot_index = 0;
			}
		}
		else {
			DebuginatorItem* parent = debuginator->hot_item->parent;
			if (++parent->hot_index == parent->children_size) {
				parent->hot_index = 0;
			}

			debuginator->hot_item = parent->children[parent->hot_index];
		}
	}

	if (input->activate && debuginator->hot_item->is_active) {
		debuginator_activate(debuginator->hot_item);
	}
}

// ██╗   ██╗████████╗██╗██╗     ██╗████████╗██╗   ██╗
// ██║   ██║╚══██╔══╝██║██║     ██║╚══██╔══╝╚██╗ ██╔╝
// ██║   ██║   ██║   ██║██║     ██║   ██║    ╚████╔╝
// ██║   ██║   ██║   ██║██║     ██║   ██║     ╚██╔╝
// ╚██████╔╝   ██║   ██║███████╗██║   ██║      ██║
//  ╚═════╝    ╚═╝   ╚═╝╚══════╝╚═╝   ╚═╝      ╚═╝


#define DEBUGINATOR_create_bool_item(debuginator, path, description, function, variable) \
	do { \
		static bool bool_values[2] = { true, false }; \
		static const char* bool_titles[2] = { "True", "False" }; \
		debuginator_create_item(debuginator, path, \
			description, function, &variable, sizeof(variable), \
			bool_titles, bool_values); \
	} while(false)

#endif // INCLUDE_THE_DEBUGINATOR_H
