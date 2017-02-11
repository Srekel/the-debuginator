#ifndef INCLUDE_THE_DEBUGINATOR_H
#define INCLUDE_THE_DEBUGINATOR_H

#ifndef DEBUGINATOR_assert
#include <assert.h>
#define DEBUGINATOR_assert assert;
#endif

#ifndef DEBUGINATOR_memcpy
#include <string.h>
#define DEBUGINATOR_memcpy memcpy
#endif

#ifndef DEBUGINATOR_OVERRIDE_DEFAULT_TYPES
typedef const char* DebuginatorTitle_t;
typedef const char* DebuginatorDescription_t;
#endif

enum DebuginatorItemType {
	DebuginatorItemType_Undefined,
	// ItemType_Single,
	DebuginatorItemType_Folder,
	DebuginatorItemType_Array,
	DebuginatorItemType_ArrayOfPtrs,
	// ItemType_Preset,
	//ItemType_Count
};

enum DebuginatorUpdateType {
	DebuginatorUpdateType_Undefined,

	// Set content at init, will never change again.
	DebuginatorUpdateType_Never,

	// Will update when item is expanded or when cursor enters it
	DebuginatorUpdateType_OnShow,

	// Will update every frame when cursor is on it or its children
	DebuginatorUpdateType_WhenHot,
};

struct DebuginatorItemDefinition {
	void* user_data;
	DebuginatorItemType type;
	DebuginatorUpdateType update_type;
	DebuginatorTitle_t title;
	DebuginatorDescription_t description;

	DebuginatorTitle_t* value_titles;
	DebuginatorDescription_t* value_descriptions;
	union {
		void* array_values;
		void** array_of_ptr_values;
	};

	unsigned num_values;
	unsigned array_item_size;
	
	void(*get_array_values)(DebuginatorItemDefinition* item,
		void** out_values,
		DebuginatorTitle_t** out_value_titles,
		DebuginatorDescription_t** out_value_descriptions,
		unsigned* out_num_values, unsigned* out_array_item_size);

	void(*get_array_of_ptr_values)(DebuginatorItemDefinition* item,
		void*** out_values,
		DebuginatorTitle_t** out_value_titles,
		DebuginatorDescription_t** out_value_descriptions,
		unsigned* out_num_values, unsigned* out_array_item_size);

	void* target;
	void (*on_item_changed)(DebuginatorItemDefinition* item,
		void* value, DebuginatorTitle_t value_title);

	unsigned hot_index; // Used for both parent -> child and item -> value
	bool is_active;

	DebuginatorItemDefinition* parent;
	DebuginatorItemDefinition** children;
	unsigned children_size;
	unsigned children_capacity;

	//DebuginatorItemDefinition* preset_items;
	//unsigned num_preset_items;
};

struct TheDebuginator {
	DebuginatorItemDefinition* root;
	DebuginatorItemDefinition* hot_item;
	//DebuginatorItemDefinition* active_item;

	unsigned item_buffer_capacity;
	unsigned item_buffer_size;
	DebuginatorItemDefinition* item_buffer;

	unsigned child_buffer_capacity;
	unsigned child_buffer_size;
	DebuginatorItemDefinition** child_buffer;
};

TheDebuginator debuginator_create(
	DebuginatorItemDefinition* item_buffer, unsigned item_buffer_capacity,
	DebuginatorItemDefinition** child_buffer, unsigned child_buffer_capacity) {
	TheDebuginator debuginator = {};
	debuginator.item_buffer_capacity = item_buffer_capacity;
	debuginator.item_buffer = item_buffer;
	memset(item_buffer, 0, sizeof(DebuginatorItemDefinition) * item_buffer_capacity);
	debuginator.child_buffer_capacity = child_buffer_capacity;
	debuginator.child_buffer = child_buffer;
	memset(child_buffer, 0, sizeof(DebuginatorItemDefinition*) * child_buffer_capacity);
	return debuginator;
}

DebuginatorItemDefinition* debuginator_new_leaf_item(TheDebuginator* debuginator) {
	DEBUGINATOR_assert(debuginator->item_buffer_size < debuginator->item_buffer_capacity);
	return &debuginator->item_buffer[debuginator->item_buffer_size++];
}

DebuginatorItemDefinition* debuginator_new_folder_item(TheDebuginator* debuginator, unsigned num_children) {
	DEBUGINATOR_assert(debuginator->item_buffer_size < debuginator->item_buffer_capacity);
	DebuginatorItemDefinition* folder_item = &debuginator->item_buffer[debuginator->item_buffer_size++];

	DEBUGINATOR_assert(debuginator->child_buffer_size + num_children < debuginator->child_buffer_capacity);
	folder_item->children_capacity = num_children;
	folder_item->children = debuginator->child_buffer + debuginator->child_buffer_size;
	debuginator->child_buffer_size += num_children;

	return folder_item;
}

void debuginator_add_child(DebuginatorItemDefinition* parent, DebuginatorItemDefinition* child) {
	DEBUGINATOR_assert(parent->children_size < parent->children_capacity);
	parent->children[parent->children_size++] = child;
	child->parent = parent;
}

static void debuginator_validate_item(DebuginatorItemDefinition* item) {
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
		DEBUGINATOR_assert(item->array_item_size == 0);
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
		DEBUGINATOR_assert(item->array_item_size > 0);
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
		DEBUGINATOR_assert(item->array_item_size > 0);
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

static void debuginator_activate(DebuginatorItemDefinition* item) {
	void* value;
	if (item->type == DebuginatorItemType_ArrayOfPtrs) {
		void** ptr_to_value = (item->array_of_ptr_values + item->hot_index);
		value = *ptr_to_value;
	}
	else {
		value = ((char*)item->array_values) + item->hot_index * item->array_item_size;	
	}

	if (item->target) {
		memcpy(item->target, value, item->array_item_size);
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
			DebuginatorItemDefinition* parent = debuginator->hot_item;
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
			DebuginatorItemDefinition* parent = debuginator->hot_item->parent;
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

#endif // INCLUDE_THE_DEBUGINATOR_H
