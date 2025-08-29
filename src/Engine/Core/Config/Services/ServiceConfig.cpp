#include "ServiceConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ServiceConfig,"ServiceConfig");

ServiceConfig::ServiceConfig(){
	services_init = {
		"Keyboard",
		"KeyBindings",
		"Mouse",
		"Material",
		"ChunkService",
		"ProceduralService",
		"UIText",
		"FrameStateService",
		"ThreadPool"
	};
	services_post_init = {
		"none"
	};
	services_render_order = {
		"Mouse",
		"Keyboard",
		"UIText",
		"ChunkService",
		"ProceduralService",
		"FrameStateService"
	};
	services_update_order = {
		"Mouse",
		"Keyboard",
		"UIText",
		"Material",
		"ChunkService",
		"FrameStateService",
		"ProceduralService"
	};
};
