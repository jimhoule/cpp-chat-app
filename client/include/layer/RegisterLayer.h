#pragma once

#include "Gui.h"
#include "layer/Layer.h"

#include <functional>

class RegisterLayer : public Layer
{
public:
    RegisterLayer(const std::string& ID, const Gui& Gui);
    RegisterLayer(const std::string& ID, const Gui& Gui, const std::shared_ptr<Logger>& Logger);

    void OnDetach() override;
    void OnRender() override;

    std::function<void()> OnLoginButtonClick;
    std::function<void(const std::string&, const std::string&, const std::string&, const std::string&)> OnRegisterButtonClick;

private:
    Gui m_Gui = {};

    std::string m_FirstName = "";
    std::string m_LastName = "";
    std::string m_Email = "";
    std::string m_Password = "";

    void Reset();
};
