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
    Window loginWindow = {};
    loginWindow.Name = "LoginWindow";
    loginWindow.Size = m_gui.GetViewportSize();
    loginWindow.BgColor = Rgba(26, 30, 67, 255);
    loginWindow.DrawContent = [this]() {
        const Vector2 loginWindowAvailableSpace = m_gui.GetAvailableSpace();

        // LOGIN FORM CONTAINER
        Container loginFormContainer = {};
        loginFormContainer.ID = "LoginFormContainer";
        loginFormContainer.Size = Vector2(loginWindowAvailableSpace.X * 0.30f, loginWindowAvailableSpace.Y * 0.60f);
        loginFormContainer.CornerRounding = 10.f;
        loginFormContainer.BgColor = Rgba(50, 56, 102, 255);
        loginFormContainer.DrawContent = [this](const ContainerState& State) {
            const Vector2 loginFormContainerAvailableSpace = m_gui.GetAvailableSpace();

            // TITLE CONTAINER
            Container titleContainer = {};
            titleContainer.ID = "TitleContainer";
            titleContainer.Size = Vector2(loginFormContainerAvailableSpace.X, loginFormContainerAvailableSpace.Y * 0.25f);
            // NOTE: Transparent background
            titleContainer.BgColor = Rgba(0, 0, 0, 0);
            titleContainer.DrawContent = [this](const ContainerState& State) {
                // TITLE TEXT
                Text titleText = {};
                titleText.Value = "Login";
                titleText.Height = 40.0f;

                Vector2 titleTextSize = m_gui.GetTextSize(titleText);
                m_gui.AlignCenter(titleTextSize);
                m_gui.DrawText(titleText);
            };

            m_gui.DrawContainer(titleContainer);

            // INPUTS CONTAINER
            Container inputsContainer = {};
            inputsContainer.ID = "InputsContainer";
            inputsContainer.Size = Vector2(loginFormContainerAvailableSpace.X, loginFormContainerAvailableSpace.Y * 0.50f);
            inputsContainer.Padding = Vector2(15.0f, 0.0f);
            // NOTE: Transparent background
            inputsContainer.BgColor = Rgba(0, 0, 0, 0);
            inputsContainer.DrawContent = [this](const ContainerState& State) {
                const Vector2 inputsContainerAvailableSpace = m_gui.GetAvailableSpace();

                // EMAIL TEXT INPUT SINGLELINE
                Placeholder emailTextInputPlaceholder = {};
                emailTextInputPlaceholder.Color = Rgba(120, 125, 172, 255);
                emailTextInputPlaceholder.Text = "Email";

                TextInput emailTextInput = {};
                emailTextInput.ID = "EmailTextInput";
                emailTextInput.Padding = Vector2(15.0f, 15.0f);
                emailTextInput.CornerRounding = 10.f;
                emailTextInput.BgColor = Rgba(26, 30, 67, 255);
                emailTextInput.Placeholder = emailTextInputPlaceholder;

                TextInputSingleline emailTextInputSingleline = {};
                emailTextInputSingleline.TextInput = emailTextInput;
                emailTextInputSingleline.Width = inputsContainerAvailableSpace.X;

                m_gui.DrawTextInputSingleline(m_email, emailTextInputSingleline);

                // PASSWORD TEXT INPUT SINGLELINE
                Placeholder passwordTextInputPlaceholder = {};
                passwordTextInputPlaceholder.Color = Rgba(120, 125, 172, 255);
                passwordTextInputPlaceholder.Text = "Password";

                TextInput passwordTextInput = {};
                passwordTextInput.ID = "PasswordTextInput";
                passwordTextInput.Padding = Vector2(15.0f, 15.0f);
                passwordTextInput.CornerRounding = 10.f;
                passwordTextInput.BgColor = Rgba(26, 30, 67, 255);
                passwordTextInput.Placeholder = passwordTextInputPlaceholder;

                TextInputSingleline passwordTextInputSingleline = {};
                passwordTextInputSingleline.TextInput = passwordTextInput;
                passwordTextInputSingleline.Width = inputsContainerAvailableSpace.X;

                m_gui.SetPositionY(inputsContainerAvailableSpace.Y * 0.50f);
                m_gui.DrawTextInputSingleline(m_password, passwordTextInputSingleline);
            };

            m_gui.SetPositionY(titleContainer.Size.Y);
            m_gui.DrawContainer(inputsContainer);

            // BUTTONS CONTAINER
            Container buttonsContainer = {};
            buttonsContainer.ID = "ButtonsContainer";
            buttonsContainer.Size = Vector2(loginFormContainerAvailableSpace.X, loginFormContainerAvailableSpace.Y * 0.25f);
            buttonsContainer.Padding = Vector2(15.0f, 0.0f);
            // NOTE: Transparent background
            buttonsContainer.BgColor = Rgba(0, 0, 0, 0);
            buttonsContainer.DrawContent = [this](const ContainerState& State) {
                const Vector2 buttonsContainerAvailableSpace = m_gui.GetAvailableSpace();

                // LOGIN BUTTON
                Button loginButton = {};
                loginButton.Label = "Login";
                loginButton.Size = Vector2(buttonsContainerAvailableSpace.X, buttonsContainerAvailableSpace.Y * 0.50f);
                loginButton.BgColor = Rgba(200, 30, 30, 255); // Red button
                loginButton.BgColorActive = Rgba(150, 0, 0, 255); // Darker red when active
                loginButton.BgColorHovered = Rgba(255, 100, 100, 255); // Lighter red on hover
                loginButton.CornerRounding = 50.0f;
                loginButton.IsDisabled = m_email.empty() || m_password.empty();
                loginButton.OnClick = [this]() {
                    HandleLoginButtonClicked();
                };

                m_gui.DrawButton(loginButton);

                // REGISTER BUTTON CONTAINER
                const std::string& registerTextValue = "Don't have an account ? ";
                Vector2 registerTextValueSize = m_gui.GetTextSize(registerTextValue);

                const std::string& registerButtonLabel = "Register";
                Vector2 registerButtonLabelSize = m_gui.GetTextSize(registerButtonLabel);

                Container registerButtonContainer = {};
                registerButtonContainer.ID = "RegisterButtonContainer";
                registerButtonContainer.Size = Vector2(registerTextValueSize.X + registerButtonLabelSize.X, registerTextValueSize.Y);
                // NOTE: Transparent background
                registerButtonContainer.BgColor = Rgba(0, 0, 0, 0);
                registerButtonContainer.DrawContent = [this, &registerButtonLabel, &registerTextValue](const ContainerState& State) {
                    // REGISTER TEXT
                    Text registerText = {};
                    registerText.Value = registerTextValue;

                    m_gui.DrawText(registerText);

                    // REGISTER BUTTON
                    Button registerButton = {};
                    registerButton.Label = registerButtonLabel;
                    registerButton.Size = m_gui.GetTextSize(registerButtonLabel);
                    // NOTE: Transparent background
                    registerButton.BgColor = Rgba(0, 0, 0, 0);
                    // NOTE: Transparent background
                    registerButton.BgColorActive = Rgba(0, 0, 0, 0);
                    // NOTE: Transparent background
                    registerButton.BgColorHovered = Rgba(0, 0, 0, 0);
                    registerButton.TextColor = Rgba(255, 255, 0, 255);
                    registerButton.OnClick = [this]() {
                        HandleRegisterButtonClicked();
                    };

                    m_gui.DisplayInline();
                    Vector2 RegisterTextSize = m_gui.GetTextSize(registerText.Value);
                    m_gui.SetPositionX(RegisterTextSize.X + 2.0f);
                    m_gui.DrawButton(registerButton);
                };

                m_gui.AlignCenter(registerButtonContainer.Size);
                m_gui.DrawContainer(registerButtonContainer);
            };

            m_gui.SetPositionY(titleContainer.Size.Y + inputsContainer.Size.Y);
            m_gui.DrawContainer(buttonsContainer);
        };

        m_gui.AlignCenter(loginFormContainer.Size);
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
