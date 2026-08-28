#pragma once

#include "layer/Layer.h"

#include <functional>
#include <map>
#include <memory>

// Forward declarations
class LayerStack;
class Logger;

class Navigation
{
public:
    enum class ScreenName
    {
        CHAT,
        LOGIN,
        REGISTER
    };

    struct Screen
    {
        std::unique_ptr<Layer> layer = nullptr;
    };

    Navigation(LayerStack& layerStack, Logger& logger);

    // Rule of Five: Explicitly delete copy/move to prevent slicing
    Navigation(const Navigation&) = delete;
    Navigation& operator=(const Navigation&) = delete;
    Navigation(Navigation&&) = delete;
    Navigation& operator=(Navigation&&) = delete;

    void AddScreen(ScreenName name, Screen screen);
    void GoBack();
    void GoToChatScreen();
    void GoToLoginScreen();
    void GoToRegisterScreen();
    void SetInitialScreen(ScreenName name);

private:
    LayerStack& m_layerStack;

    std::unordered_map<ScreenName, Screen> m_screensMap = {};

    Logger& m_logger;

    void Clear();
};
