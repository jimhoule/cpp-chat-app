#pragma once

#include "Gui.h"

#include "auth/RegisteredEvent.h"
#include "layer/Layer.h"
#include "observer/Observer.h"

// Forward declarations
class AuthApi;
class Logger;
class Navigation;

class RegisterLayer : public Layer
{
public:
    RegisterLayer(const std::string& id, const Gui& gui, Navigation& navigation, AuthApi& authApi, Logger& logger);

    void OnAttach() override;
    void OnDetach() override;
    void OnRender() override;

private:
    Gui m_gui = {};

    Navigation& m_navigation;

    AuthApi& m_authApi;
    Observer<RegisteredEvent, RegisterLayer> m_registeredObserver;

    std::string m_firstName = "";
    std::string m_lastName = "";
    std::string m_email = "";
    std::string m_password = "";

    void HandleLoginButtonClicked();
    void HandleRegisterButtonClicked();
    void HandleRegistered(const RegisteredEvent& registeredEvent);
    void Reset();
};
