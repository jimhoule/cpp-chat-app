#include "layer/LoginLayer.h"

#include "auth/AuthApi.h"
#include "navigation/Navigation.h"

// **********
// * PUBLIC *
// **********
LoginLayer::LoginLayer(const std::string& id, const Gui& gui, Navigation& navigation, AuthApi& authApi, Logger& logger)
    : Layer(id, logger)
    , m_gui(gui)
    , m_navigation(navigation)
    , m_authApi(authApi)
    , m_loggedInObserver(*this, &LoginLayer::HandleLoggedIn)
{}

void LoginLayer::OnAttach()
{
    m_authApi.GetLoggedInSubject().Subscribe(&m_loggedInObserver);
}

void LoginLayer::OnDetach()
{
    m_authApi.GetLoggedInSubject().Unsubscribe(&m_loggedInObserver);
    Reset();
}

void LoginLayer::OnSuspend()
{
    Reset();
}

void LoginLayer::OnRender()
{
    Gui::Window loginWindow = {};
    loginWindow.name = "LoginWindow";
    loginWindow.size = m_gui.GetViewportSize();
    loginWindow.bgColor = Rgba(26, 30, 67, 255);
    loginWindow.DrawContent = [this]() {
        const Vector2 loginWindowAvailableSpace = m_gui.GetAvailableSpace();

        // LOGIN FORM CONTAINER
        Gui::Container loginFormContainer = {};
        loginFormContainer.id = "LoginFormContainer";
        loginFormContainer.size = Vector2(loginWindowAvailableSpace.x * 0.30f, loginWindowAvailableSpace.y * 0.60f);
        loginFormContainer.cornerRounding = 10.f;
        loginFormContainer.bgColor = Rgba(50, 56, 102, 255);
        loginFormContainer.DrawContent = [this](const Gui::ContainerState& state) {
            const Vector2 loginFormContainerAvailableSpace = m_gui.GetAvailableSpace();

            // TITLE CONTAINER
            Gui::Container titleContainer = {};
            titleContainer.id = "TitleContainer";
            titleContainer.size = Vector2(loginFormContainerAvailableSpace.x, loginFormContainerAvailableSpace.y * 0.25f);
            // NOTE: Transparent background
            titleContainer.bgColor = Rgba(0, 0, 0, 0);
            titleContainer.DrawContent = [this](const Gui::ContainerState& state) {
                // TITLE TEXT
                Gui::Text titleText = {};
                titleText.value = "Login";
                titleText.height = 40.0f;

                Vector2 titleTextSize = m_gui.GetTextSize(titleText);
                m_gui.AlignCenter(titleTextSize);
                m_gui.DrawText(titleText);
            };

            m_gui.DrawContainer(titleContainer);

            // INPUTS CONTAINER
            Gui::Container inputsContainer = {};
            inputsContainer.id = "InputsContainer";
            inputsContainer.size = Vector2(loginFormContainerAvailableSpace.x, loginFormContainerAvailableSpace.y * 0.50f);
            inputsContainer.padding = Vector2(15.0f, 0.0f);
            // NOTE: Transparent background
            inputsContainer.bgColor = Rgba(0, 0, 0, 0);
            inputsContainer.DrawContent = [this](const Gui::ContainerState& state) {
                const Vector2 inputsContainerAvailableSpace = m_gui.GetAvailableSpace();

                // EMAIL TEXT INPUT SINGLELINE
                Gui::Placeholder emailTextInputPlaceholder = {};
                emailTextInputPlaceholder.color = Rgba(120, 125, 172, 255);
                emailTextInputPlaceholder.text = "Email";

                Gui::TextInput emailTextInput = {};
                emailTextInput.id = "EmailTextInput";
                emailTextInput.padding = Vector2(15.0f, 15.0f);
                emailTextInput.cornerRounding = 10.f;
                emailTextInput.bgColor = Rgba(26, 30, 67, 255);
                emailTextInput.placeholder = emailTextInputPlaceholder;

                Gui::TextInputSingleline emailTextInputSingleline = {};
                emailTextInputSingleline.textInput = emailTextInput;
                emailTextInputSingleline.width = inputsContainerAvailableSpace.x;

                m_gui.DrawTextInputSingleline(m_email, emailTextInputSingleline);

                // PASSWORD TEXT INPUT SINGLELINE
                Gui::Placeholder passwordTextInputPlaceholder = {};
                passwordTextInputPlaceholder.color = Rgba(120, 125, 172, 255);
                passwordTextInputPlaceholder.text = "Password";

                Gui::TextInput passwordTextInput = {};
                passwordTextInput.id = "PasswordTextInput";
                passwordTextInput.padding = Vector2(15.0f, 15.0f);
                passwordTextInput.cornerRounding = 10.f;
                passwordTextInput.bgColor = Rgba(26, 30, 67, 255);
                passwordTextInput.placeholder = passwordTextInputPlaceholder;

                Gui::TextInputSingleline passwordTextInputSingleline = {};
                passwordTextInputSingleline.textInput = passwordTextInput;
                passwordTextInputSingleline.width = inputsContainerAvailableSpace.x;

                m_gui.SetPositionY(inputsContainerAvailableSpace.y * 0.50f);
                m_gui.DrawTextInputSingleline(m_password, passwordTextInputSingleline);
            };

            m_gui.SetPositionY(titleContainer.size.y);
            m_gui.DrawContainer(inputsContainer);

            // BUTTONS CONTAINER
            Gui::Container buttonsContainer = {};
            buttonsContainer.id = "ButtonsContainer";
            buttonsContainer.size = Vector2(loginFormContainerAvailableSpace.x, loginFormContainerAvailableSpace.y * 0.25f);
            buttonsContainer.padding = Vector2(15.0f, 0.0f);
            // NOTE: Transparent background
            buttonsContainer.bgColor = Rgba(0, 0, 0, 0);
            buttonsContainer.DrawContent = [this](const Gui::ContainerState& state) {
                const Vector2 buttonsContainerAvailableSpace = m_gui.GetAvailableSpace();

                // LOGIN BUTTON
                Gui::Button loginButton = {};
                loginButton.label = "Login";
                loginButton.size = Vector2(buttonsContainerAvailableSpace.x, buttonsContainerAvailableSpace.y * 0.50f);
                loginButton.bgColor = Rgba(200, 30, 30, 255); // Red button
                loginButton.bgColorActive = Rgba(150, 0, 0, 255); // Darker red when active
                loginButton.bgColorHovered = Rgba(255, 100, 100, 255); // Lighter red on hover
                loginButton.cornerRounding = 50.0f;
                loginButton.isDisabled = m_email.empty() || m_password.empty();
                loginButton.OnClick = [this]() {
                    HandleLoginButtonClicked();
                };

                m_gui.DrawButton(loginButton);

                // REGISTER BUTTON CONTAINER
                const std::string& registerTextValue = "Don't have an account ? ";
                Vector2 registerTextValueSize = m_gui.GetTextSize(registerTextValue);

                const std::string& registerButtonLabel = "Register";
                Vector2 registerButtonLabelSize = m_gui.GetTextSize(registerButtonLabel);

                Gui::Container registerButtonContainer = {};
                registerButtonContainer.id = "RegisterButtonContainer";
                registerButtonContainer.size = Vector2(registerTextValueSize.x + registerButtonLabelSize.x, registerTextValueSize.y);
                // NOTE: Transparent background
                registerButtonContainer.bgColor = Rgba(0, 0, 0, 0);
                registerButtonContainer.DrawContent = [this, &registerButtonLabel, &registerTextValue](const Gui::ContainerState& state) {
                    // REGISTER TEXT
                    Gui::Text registerText = {};
                    registerText.value = registerTextValue;

                    m_gui.DrawText(registerText);

                    // REGISTER BUTTON
                    Gui::Button registerButton = {};
                    registerButton.label = registerButtonLabel;
                    registerButton.size = m_gui.GetTextSize(registerButtonLabel);
                    // NOTE: Transparent background
                    registerButton.bgColor = Rgba(0, 0, 0, 0);
                    // NOTE: Transparent background
                    registerButton.bgColorActive = Rgba(0, 0, 0, 0);
                    // NOTE: Transparent background
                    registerButton.bgColorHovered = Rgba(0, 0, 0, 0);
                    registerButton.textColor = Rgba(255, 255, 0, 255);
                    registerButton.OnClick = [this]() {
                        HandleRegisterButtonClicked();
                    };

                    m_gui.DisplayInline();
                    Vector2 RegisterTextSize = m_gui.GetTextSize(registerText.value);
                    m_gui.SetPositionX(RegisterTextSize.x + 2.0f);
                    m_gui.DrawButton(registerButton);
                };

                m_gui.AlignCenter(registerButtonContainer.size);
                m_gui.DrawContainer(registerButtonContainer);
            };

            m_gui.SetPositionY(titleContainer.size.y + inputsContainer.size.y);
            m_gui.DrawContainer(buttonsContainer);
        };

        m_gui.AlignCenter(loginFormContainer.size);
        m_gui.DrawContainer(loginFormContainer);
    };

    m_gui.DrawWindow(loginWindow);
}

// ***********
// * PRIVATE *
// ***********
void LoginLayer::HandleLoginButtonClicked()
{
    LoginParams loginParams = {};
    loginParams.email = m_email;
    loginParams.password = m_password;
    m_authApi.Login(loginParams);
}

void LoginLayer::HandleRegisterButtonClicked()
{
    m_navigation.GoToRegisterScreen();
}

void LoginLayer::HandleLoggedIn(const LoggedInEvent& loggedInEvent)
{
    m_navigation.GoToChatScreen();
}

void LoginLayer::Reset()
{
    m_email = "";
    m_password = "";
}
