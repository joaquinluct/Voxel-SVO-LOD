#pragma once

#include <string>

class IDebug
{
public:
	virtual std::string GetDebugInfo() const { return ""; }
};
