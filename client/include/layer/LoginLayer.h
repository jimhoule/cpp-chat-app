#pragma once

#include "Gui.h"
#include "layer/Layer.h"

#include <functional>

class LoginLayer : public Layer
{
public:
    LoginLayer(const std::string& ID, const Gui& Gui);
    LoginLayer(const std::string& ID, const Gui& Gui, const std::shared_ptr<Logger>& Logger);

    void OnDetach() override;
    void OnSuspend() override;
    void OnRender() override;

    std::function<void(const std::string&, const std::string&)> OnLoginButtonClick;
    std::function<void()> OnRegisterButtonClick;

private:
    Gui m_Gui = {};

    std::string m_Email = "";
    std::string m_Password = "";

    void Reset();
};
