#include "ServiceConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ServiceConfig,"ServiceConfig");

ServiceConfig::ServiceConfig(){
	services_init = {
		"Keyboard",
		"KeyBindings",
		"Mouse",
		"Material",
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
		"ProceduralService",
		"FrameStateService"
	};
	services_update_order = {
		"Mouse",
		"Keyboard",
		"UIText",
		"Material",
		"FrameStateService",
		"ProceduralService"
	};
};
