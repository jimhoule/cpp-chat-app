#include "layer/SplashLayer.h"

#include "auth/AuthApi.h"
#include "navigation/Navigation.h"

// **********
// * PUBLIC *
// **********
SplashLayer::SplashLayer(const std::string& id, const Gui& gui, Navigation& navigation, AuthApi& authApi, Logger& logger)
    : Layer(id, logger)
    , m_gui(gui)
    , m_navigation(navigation)
    , m_authApi(authApi)
    , m_authenticatedObserver(*this, &SplashLayer::HandleAuthenticated)
{}

void SplashLayer::OnAttach()
{
    m_authApi.GetAuthenticatedSubject().Subscribe(&m_authenticatedObserver);

    if (!m_authApi.HasStoredSession())
    {
        // NOTE: Navigating to login here would clear the layer stack before pushing into it while it is still pushing this layer so it is deferred to the next update
        m_mustNavigateToLoginScreen = true;

        return;
    }

    m_authApi.Authenticate();
}

void SplashLayer::OnDetach()
{
    m_authApi.GetAuthenticatedSubject().Unsubscribe(&m_authenticatedObserver);
}

void SplashLayer::OnRender()
{
    Gui::Window splashWindow = {};
    splashWindow.name = "SplashWindow";
    splashWindow.size = m_gui.GetViewportSize();
    splashWindow.bgColor = Rgba(26, 30, 67, 255);
    splashWindow.DrawContent = [this]() {
        // TITLE TEXT
        Gui::Text titleText = {};
        titleText.value = "Splash";
        titleText.height = 40.0f;
        titleText.color = Rgba(0, 255, 0, 255);

        Vector2 titleTextSize = m_gui.GetTextSize(titleText);
        m_gui.AlignCenter(titleTextSize);
        m_gui.DrawText(titleText);
    };

    m_gui.DrawWindow(splashWindow);
}

void SplashLayer::OnUpdate()
{
    if (!m_mustNavigateToLoginScreen)
    {
        return;
    }

    m_mustNavigateToLoginScreen = false;
    m_navigation.GoToLoginScreen();
}

// ***********
// * PRIVATE *
// ***********
void SplashLayer::HandleAuthenticated(const AuthenticatedEvent& authenticatedEvent)
{
    m_navigation.GoToChatScreen();
}
