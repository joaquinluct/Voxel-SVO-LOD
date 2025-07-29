#include "RendererConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(RendererConfig,"RendererConfig");

RendererConfig::RendererConfig(){
	name = "Renderer";
	enabled = true;
	version = "1.0.0";
	type = "DeferredRendering";
	dependencies = {
		"DeviceManager"
	};
};
