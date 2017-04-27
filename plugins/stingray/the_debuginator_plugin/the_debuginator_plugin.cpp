
#include "c_api_the_debuginator.h"

#include <engine_plugin_api/plugin_api.h>
#include <plugin_foundation/platform.h>

/* PLUGIN API */

const char* get_name() { return "the_debuginator_plugin"; }

void setup_plugin(GetApiFunction get_engine_api)
{
	setup_api(get_engine_api, get_name());
}

void shutdown_plugin()
{
	shutdown_api();
}

void update_plugin(float dt)
{
	update_debuginators(dt);
}

extern "C" {

	/**
	 * Load and define plugin APIs.
	 */
	PLUGIN_DLLEXPORT void *get_plugin_api(unsigned api)
	{
		if (api == PLUGIN_API_ID) {
			static PluginApi plugin_api = { 0 };
			plugin_api.get_name = get_name;
			plugin_api.setup_game = setup_plugin;
			plugin_api.update_game = update_plugin;
			plugin_api.shutdown_game = shutdown_plugin;
			return &plugin_api;
		}
		else if (api == THE_DEBUGINATOR_API_ID)
		{
			static TheDebuginatorApi debuginator_api = { 0 };
			get_debuginator_api(&debuginator_api);
			return &debuginator_api;
		}

		return nullptr;
	}

}

/* EXAMPLE */

// #define DEBUGINATOR_EXAMPLE
#if defined(DEBUGINATOR_EXAMPLE)

#include "the_debuginator.h"

void* get_engine_api(int id) { return NULL; };

struct example_data {
	bool my_bool;
	bool limit_framerate;
};

static example_data my_userdata;

void example() {
	PluginManagerApi *plugin_manager_api = (PluginManagerApi*)get_engine_api(PLUGIN_MANAGER_API_ID);
	TheDebuginatorApi* debuginator_api = (TheDebuginatorApi*)plugin_manager_api->get_next_plugin_api(THE_DEBUGINATOR_API_ID, NULL);

	int max_number_of_items = 100000;
	int width = 500;
	float focus_height = 0.6f;
	TheDebuginator* debuginator = debuginator_api->create_debuginator("Example", NULL, max_number_of_items, width, focus_height, &my_userdata);

	debuginator_create_bool_item(debuginator, "SDL Demo/Throttle framerate", "Disables sleeping between frames.", &my_userdata.limit_framerate);

	debuginator_api->destroy_debuginator(debuginator);
}
#endif
