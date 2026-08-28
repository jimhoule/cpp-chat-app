#include "navigation/Navigation.h"

#include "layer/LayerStack.h"

// **********
// * PUBLIC *
// **********
Navigation::Navigation(LayerStack& layerStack, Logger& logger)
    : m_layerStack(layerStack)
    , m_logger(logger)
{}

void Navigation::AddScreen(ScreenName name, Screen screen)
{
    m_screensMap.insert(std::pair(name, std::move(screen)));
}

void Navigation::GoBack()
{
    // NOTE: Never pops the root screens as there would be nothing left to render
    if (m_layerStack.GetSize() <= 1)
    {
        return;
    }

    m_layerStack.Pop();

    const Layer* topLayer = m_layerStack.GetTop();
    if (topLayer == nullptr)
    {
        return;
    }

    if (topLayer->IsSuspended())
    {
        m_layerStack.Unsuspend(topLayer->GetId());
    }
}

void Navigation::GoToChatScreen()
{
    Clear();

    Layer* chatScreenLayer = m_screensMap.at(ScreenName::CHAT).layer.get();
    m_layerStack.Push(chatScreenLayer);
}

void Navigation::GoToLoginScreen()
{
    Clear();

    Layer* loginScreenLayer = m_screensMap.at(ScreenName::LOGIN).layer.get();
    m_layerStack.Push(loginScreenLayer);
}

void Navigation::GoToRegisterScreen()
{
    /**
     * NOTES:
     *  - Uses at() over find() as an unregistered screen is a programming error, not a runtime case
     *  - Gets a non-owning pointer to the screen's layer, which stays owned by the map
     *  - Throws std::out_of_range and terminates the app as nothing catches it
     *  - Only returns the value, not the key
     */
    Layer* registerScreenLayer = m_screensMap.at(ScreenName::REGISTER).layer.get();
    Layer* topLayer = m_layerStack.GetTop();

    m_layerStack.Suspend(topLayer->GetId());
    m_layerStack.Push(registerScreenLayer);
}

void Navigation::SetInitialScreen(ScreenName name)
{
    Layer* initialScreenLayer = m_screensMap.at(name).layer.get();

    m_layerStack.Push(initialScreenLayer);
}

// ***********
// * PRIVATE *
// ***********
void Navigation::Clear()
{
    // Unsuspends all suspended layers
    for (const std::pair<const ScreenName, Screen>& screensMapEntry : m_screensMap)
    {
        Layer* layer =  screensMapEntry.second.layer.get();
        if (layer->IsSuspended())
        {
            m_layerStack.Unsuspend(layer->GetId());
        }
    }

    m_layerStack.Clear();
}
