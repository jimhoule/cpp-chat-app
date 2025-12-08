#pragma once

#include "Gui.h"
#include "layer/Layer.h"

#include <functional>
#include <memory>
#include <string>

class RegisterLayer : public Layer
{
public:
    RegisterLayer(const std::string& ID, const Gui& Gui);

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
