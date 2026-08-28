#pragma once

#include "Gui.h"

#include "auth/LoggedInEvent.h"
#include "layer/Layer.h"
#include "observer/Observer.h"

// Forward declarations
class AuthApi;
class Logger;
class Navigation;

class LoginLayer : public Layer
{
public:
    LoginLayer(const std::string& id, const Gui& gui, Navigation& navigation, AuthApi& authApi, Logger& logger);

    void OnAttach() override;
    void OnDetach() override;
    void OnSuspend() override;
    void OnRender() override;

private:
    Gui m_gui = {};

    Navigation& m_navigation;

    AuthApi& m_authApi;
    Observer<LoggedInEvent, LoginLayer> m_loggedInObserver;

    std::string m_email = "";
    std::string m_password = "";

    void HandleLoginButtonClicked();
    void HandleRegisterButtonClicked();
    void HandleLoggedIn(const LoggedInEvent& loggedInEvent);
    void Reset();
};
