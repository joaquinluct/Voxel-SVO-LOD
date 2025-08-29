#pragma once

#include <string>
#include <Util/Text/Text.h>

namespace PipelineHelper
{
	inline static std::string GetConfigStringValue(std::string configValue) {
		std::string result = configValue;
		result = StrToLower(result);
		if (result.empty() || result == "none" || result == "null" || result == "0" || result == "false") {
			return "";
		}
		return configValue;
	}
	
} // namespace PipelineHelper
