#pragma once

#include <string>

/**
 * @brief Interface for MainWindow configuration
 * This interface provides the contract for MainWindow configuration classes
 */
class IMainWindowConfig {
public:
    virtual ~IMainWindowConfig() = default;
    
    // Window properties
    virtual const std::string& GetTitle() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual bool IsFullscreen() const = 0;
    virtual bool IsResizable() const = 0;
    
    // Configuration metadata
    virtual const std::string& GetConfigName() const = 0;
};
