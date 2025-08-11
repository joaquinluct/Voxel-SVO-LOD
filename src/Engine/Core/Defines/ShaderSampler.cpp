#include "ShaderSampler.h"

namespace ShaderSampler
{
	ShaderSampler::SamplerVariant GetSampler(std::string samplerName) {
		if (samplerName == "RegularSampler")
			return RegularSampler();
		else if (samplerName == "ShadowMapSampler")
			return ShadowMapSampler();
		else if (samplerName == "UISampler")
			return UISampler();
		else
			return {};
	}
}