#include "ServiceConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ServiceConfig,"ServiceConfig");

ServiceConfig::ServiceConfig(){
	services_init = {
		"Keyboard",
		"KeyBindings",
		"Mouse",
		"Material",
		"UIBase"
	};
	services_render_order = {
		"Mouse",
		"Keyboard",
		"UIBase"
	};
	services_update_order = {
		"Mouse",
		"Keyboard",
		"UIBase",
		"Material"
	};
};
