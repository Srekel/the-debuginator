#pragma once

#define THE_DEBUGINATOR_API_ID 0x0ac5036c // 0x0ac5036c049d75bf

#include "the_debuginator.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct CApiWindow CApiWindow;
	typedef const CApiWindow* ConstWindowPtr;
	typedef struct CApiGui* GuiPtr;
	typedef struct CApiMaterial* MaterialPtr;
	typedef void *(*GetApiFunction)(unsigned api);

	typedef struct DebuginatorPluginCreateContext {
		TheDebuginatorConfig config;
		ConstWindowPtr window;
		GuiPtr gui;
		uint64_t font;
		MaterialPtr font_material;
		int font_size;
	} DebuginatorPluginCreateContext;

	typedef enum DebuginatorInputDevices {
		Debuginator_Keyboard = 1 << 0,
		Debuginator_Gamepad = 1 << 1,
		Debuginator_Mouse = 1 << 2,
		Debuginator_Touch = 1 << 3,
	} DebuginatorInputDevices;

	typedef struct TheDebuginatorApi {
		// Plugin API
		TheDebuginator*(*create_debuginator)(const char* id, DebuginatorPluginCreateContext* context);
		TheDebuginator*(*get_debuginator)(const char* id);
		void(*destroy_debuginator)(TheDebuginator* debuginator);

		void(*handle_default_input)(TheDebuginator* debuginator, unsigned devices);
		void(*set_ui_size)(TheDebuginator* debuginator, int font_size, int item_height);

		// Direct Debuginator API
		void(*get_default_config)(TheDebuginatorConfig* config);
		bool(*is_open)(TheDebuginator* debuginator);
		void(*set_open)(TheDebuginator* debuginator, bool open);

		DebuginatorItem*(*create_array_item)(TheDebuginator* debuginator,
			DebuginatorItem* parent, const char* path, const char* description,
			DebuginatorOnItemChangedCallback on_item_changed_callback, void* user_data,
			const char** value_titles, void* values, int num_values, int value_size);

		DebuginatorItem*(*create_bool_item)(TheDebuginator* debuginator, const char* path, const char* description, void* user_data);
		DebuginatorItem*(*create_preset_item)(TheDebuginator* debuginator, const char* path, const char** paths, const char** value_titles, int** value_indices, int num_paths);

		DebuginatorItem*(*new_folder_item)(TheDebuginator* debuginator, DebuginatorItem* parent, const char* title, int title_length);
		DebuginatorItem*(*get_item)(TheDebuginator* debuginator, DebuginatorItem* parent, const char* path, bool create_if_not_exist);
		void(*set_hot_item)(TheDebuginator* debuginator, const char* path);
		DebuginatorItem*(*get_hot_item)(TheDebuginator* debuginator);
		void(*remove_item)(TheDebuginator* debuginator, DebuginatorItem* item);
		void(*remove_item_by_path)(TheDebuginator* debuginator, const char* path); 

		int(*save)(TheDebuginator* debuginator, DebuginatorSaveItemCallback callback, char* save_buffer, int save_buffer_size);
		void(*load_item)(TheDebuginator* debuginator, const char* path, const char* value_title);
		void(*set_default_value)(TheDebuginator* debuginator, const char* path, const char* value_title, int value_index); // value index is used if value_title == NULL
		void(*set_edit_type)(TheDebuginator* debuginator, const char* path, DebuginatorItemEditorDataType edit_type);

		void(*activate)(TheDebuginator* debuginator, DebuginatorItem* item, bool animate);
		void(*move_to_next_leaf)(TheDebuginator* debuginator, bool long_move);
		void(*move_to_prev_leaf)(TheDebuginator* debuginator, bool long_move);
		void(*move_to_child)(TheDebuginator* debuginator, bool toggle_and_activate);
		void(*move_to_parent)(TheDebuginator* debuginator);

		bool(*is_filtering_enabled)(TheDebuginator* debuginator);
		void(*set_filtering_enabled)(TheDebuginator* debuginator, bool enabled);
		const char*(*get_filter)(TheDebuginator* debuginator);
		void(*update_filter)(TheDebuginator* debuginator, const char* wanted_filter);

		void(*set_item_height)(TheDebuginator* debuginator, int item_height);
		void(*set_size)(TheDebuginator* debuginator, int width, int height);
	} TheDebuginatorApi;

	void get_debuginator_api(TheDebuginatorApi* api);
	void setup_api(GetApiFunction get_engine_api, const char* plugin_name);
	void shutdown_api();
	void update_debuginators(float dt);

	void* start_reload(GetApiFunction get_engine_api);
	void finish_reload(GetApiFunction get_engine_api, void *state);

#ifdef __cplusplus
}
#endif
