#include "ThreadPoolConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ThreadPoolConfig,"ThreadPoolConfig");

ThreadPoolConfig::ThreadPoolConfig(){
	enabled = true;
	dependencies = {
		"None"
	};
};
