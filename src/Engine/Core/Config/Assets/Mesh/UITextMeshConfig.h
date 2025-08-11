#pragma once

#include <string>
#include <vector>
#include <map>
#include <MeshAssetConfigBase.h>

class UITextMeshConfig : public MeshAssetConfigBase {
public:
	UITextMeshConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "UITextMeshConfig";
		return className;
	}
	bool enabled;
};
