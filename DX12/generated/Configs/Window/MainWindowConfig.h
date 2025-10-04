#pragma once

#include <string>
#include <vector>
#include <map>
#include "IMainWindowConfig.h"

class MainWindowConfig : public IMainWindowConfig {
public:
	MainWindowConfig() 
		: title("DirectX 12 Voxel Engine")
		, width(1920)
		, height(1080)
		, fullscreen(false)
		, resizable(true)
	{
		// Simple inline constructor for now
	}
	
	// IMainWindowConfig implementation
	const std::string& GetTitle() const override {
		return title;
	}
	
	int GetWidth() const override { return width; }
	int GetHeight() const override { return height; }
	bool IsFullscreen() const override { return fullscreen; }
	bool IsResizable() const override { return resizable; }
	
	const std::string& GetConfigName() const override {
		return GetStaticConfigName();
	}
	
	static const std::string& GetStaticConfigName() {
		static const std::string className = "MainWindowConfig";
		return className;
	}
	
	// Configuration properties
	std::string title;
	int width;
	int height;
	bool fullscreen;
	bool resizable;
};
