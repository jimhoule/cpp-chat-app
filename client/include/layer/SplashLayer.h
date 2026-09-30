#pragma once

#include "Gui.h"

#include "auth/AuthenticatedEvent.h"
#include "layer/Layer.h"
#include "observer/Observer.h"

// Forward declarations
class AuthApi;
class Logger;
class Navigation;

class SplashLayer : public Layer
{
public:
    SplashLayer(const std::string& id, const Gui& gui, Navigation& navigation, AuthApi& authApi, Logger& logger);

    void OnAttach() override;
    void OnDetach() override;
    void OnRender() override;
    void OnUpdate() override;

private:
    Gui m_gui = {};

    Navigation& m_navigation;

    AuthApi& m_authApi;
    Observer<AuthenticatedEvent, SplashLayer> m_authenticatedObserver;

    bool m_mustNavigateToLoginScreen = false;

    void HandleAuthenticated(const AuthenticatedEvent& authenticatedEvent);
};
