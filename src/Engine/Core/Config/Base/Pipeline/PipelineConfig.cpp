#include "PipelineConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(PipelineConfig,"PipelineConfig");

PipelineConfig::PipelineConfig(){
	mainViewport = "ViewportMainColor";
	mainRenderTargetView = "RenderTargetViewMainColor";
	mainStencilView = "StencilViewMainColor";
};
