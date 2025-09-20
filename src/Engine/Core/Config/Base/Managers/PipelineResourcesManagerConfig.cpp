#include "PipelineResourcesManagerConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(PipelineResourcesManagerConfig,"PipelineResourcesManagerConfig");

PipelineResourcesManagerConfig::PipelineResourcesManagerConfig(){
	dependencies = {
		"DeviceManager"
	};
};
