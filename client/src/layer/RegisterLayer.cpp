#include "layer/RegisterLayer.h"

#include "auth/AuthApi.h"
#include "navigation/Navigation.h"

// **********
// * PUBLIC *
// **********
RegisterLayer::RegisterLayer(const std::string& id, const Gui& gui, Navigation& navigation, AuthApi& authApi, Logger& logger)
    : Layer(id, logger)
    , m_gui(gui)
    , m_navigation(navigation)
    , m_authApi(authApi)
    , m_registeredObserver(*this, &RegisterLayer::HandleRegistered)
{}

void RegisterLayer::OnAttach()
{
    m_authApi.GetRegisteredSubject().Subscribe(&m_registeredObserver);
}

void RegisterLayer::OnDetach()
{
    m_authApi.GetRegisteredSubject().Unsubscribe(&m_registeredObserver);
    Reset();
}

void RegisterLayer::OnRender()
{
    Window registerWindow = {};
    registerWindow.Name = "RegisterWindow";
    registerWindow.Size = m_gui.GetViewportSize();
    registerWindow.BgColor = Rgba(26, 30, 67, 255);
    registerWindow.DrawContent = [this]() {
        const Vector2 registerWindowAvailableSpace = m_gui.GetAvailableSpace();

        // REGISTER FORM CONTAINER
        Container registerFormContainer = {};
        registerFormContainer.ID = "RegisterFormContainer";
        registerFormContainer.Size = Vector2(registerWindowAvailableSpace.X * 0.30f, registerWindowAvailableSpace.Y * 0.70f);
        registerFormContainer.CornerRounding = 10.f;
        registerFormContainer.BgColor = Rgba(50, 56, 102, 255);
        registerFormContainer.DrawContent = [this](const ContainerState& State) {
            const Vector2 registerFormContainerAvailableSpace = m_gui.GetAvailableSpace();

            // TITLE CONTAINER
            Container titleContainer = {};
            titleContainer.ID = "TitleContainer";
            titleContainer.Size = Vector2(registerFormContainerAvailableSpace.X, registerFormContainerAvailableSpace.Y * 0.20f);
            // NOTE: Transparent background
            titleContainer.BgColor = Rgba(0, 0, 0, 0);
            titleContainer.DrawContent = [this](const ContainerState& State) {
                // TITLE TEXT
                Text titleText = {};
                titleText.Value = "Register";
                titleText.Height = 40.0f;

                Vector2 titleTextSize = m_gui.GetTextSize(titleText);
                m_gui.AlignCenter(titleTextSize);
                m_gui.DrawText(titleText);
            };

            m_gui.DrawContainer(titleContainer);

            // INPUTS CONTAINER
            Container inputsContainer = {};
            inputsContainer.ID = "InputsContainer";
            inputsContainer.Size = Vector2(registerFormContainerAvailableSpace.X, registerFormContainerAvailableSpace.Y * 0.60f);
            inputsContainer.Padding = Vector2(15.0f, 0.0f);
            // NOTE: Transparent background
            inputsContainer.BgColor = Rgba(0, 0, 0, 0);
            inputsContainer.DrawContent = [this](const ContainerState& State) {
                const Vector2 inputsContainerAvailableSpace = m_gui.GetAvailableSpace();

                // FIRSTNAME TEXT INPUT SINGLELINE
                Placeholder firstNameTextInputPlaceholder = {};
                firstNameTextInputPlaceholder.Color = Rgba(120, 125, 172, 255);
                firstNameTextInputPlaceholder.Text = " FirstName";

                TextInput firstNameTextInput = {};
                firstNameTextInput.ID = "FirstNameTextInput";
                firstNameTextInput.Padding = Vector2(15.0f, 15.0f);
                firstNameTextInput.CornerRounding = 10.f;
                firstNameTextInput.BgColor = Rgba(26, 30, 67, 255);
                firstNameTextInput.Placeholder = firstNameTextInputPlaceholder;

                TextInputSingleline firstNameTextInputSingleline = {};
                firstNameTextInputSingleline.TextInput = firstNameTextInput;
                firstNameTextInputSingleline.Width = inputsContainerAvailableSpace.X;

                m_gui.DrawTextInputSingleline(m_firstName, firstNameTextInputSingleline);

                // LASTNAME TEXT INPUT SINGLELINE
                Placeholder lastNameTextInputPlaceholder = {};
                lastNameTextInputPlaceholder.Color = Rgba(120, 125, 172, 255);
                lastNameTextInputPlaceholder.Text = " LastName";

                TextInput lastNameTextInput = {};
                lastNameTextInput.ID = "LastNameTextInput";
                lastNameTextInput.Padding = Vector2(15.0f, 15.0f);
                lastNameTextInput.CornerRounding = 10.f;
                lastNameTextInput.BgColor = Rgba(26, 30, 67, 255);
                lastNameTextInput.Placeholder = lastNameTextInputPlaceholder;

                TextInputSingleline lastNameTextInputSingleline = {};
                lastNameTextInputSingleline.TextInput = lastNameTextInput;
                lastNameTextInputSingleline.Width = inputsContainerAvailableSpace.X;

                m_gui.SetPositionY(inputsContainerAvailableSpace.Y * 0.25f);
                m_gui.DrawTextInputSingleline(m_lastName, lastNameTextInputSingleline);

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

                m_gui.SetPositionY(inputsContainerAvailableSpace.Y * 0.50f);
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

                m_gui.SetPositionY(inputsContainerAvailableSpace.Y * 0.75f);
                m_gui.DrawTextInputSingleline(m_password, passwordTextInputSingleline);
            };

            m_gui.SetPositionY(titleContainer.Size.Y);
            m_gui.DrawContainer(inputsContainer);

            // BUTTONS CONTAINER
            Container buttonsContainer = {};
            buttonsContainer.ID = "ButtonsContainer";
            buttonsContainer.Size = Vector2(registerFormContainerAvailableSpace.X, registerFormContainerAvailableSpace.Y * 0.20f);
            buttonsContainer.Padding = Vector2(15.0f, 0.0f);
            // NOTE: Transparent background
            buttonsContainer.BgColor = Rgba(0, 0, 0, 0);
            buttonsContainer.DrawContent = [this](const ContainerState& State) {
                const Vector2 buttonsContainerAvailableSpace = m_gui.GetAvailableSpace();

                // REGISTER BUTTON
                Button registerButton = {};
                registerButton.Label = "Register";
                registerButton.Size = Vector2(buttonsContainerAvailableSpace.X, buttonsContainerAvailableSpace.Y * 0.50f);
                registerButton.BgColor = Rgba(200, 30, 30, 255); // Red button
                registerButton.BgColorActive = Rgba(150, 0, 0, 255); // Darker red when active
                registerButton.BgColorHovered = Rgba(255, 100, 100, 255); // Lighter red on hover
                registerButton.CornerRounding = 50.0f;
                registerButton.IsDisabled = m_email.empty() || m_password.empty();
                registerButton.OnClick = [this]() {
                    HandleRegisterButtonClicked();
                };

                m_gui.DrawButton(registerButton);

                // LOGIN BUTTON CONTAINER
                const std::string& loginTextValue = "Already have an account ? ";
                Vector2 loginTextValueSize = m_gui.GetTextSize(loginTextValue);

                const std::string& loginButtonLabel = "Login";
                Vector2 loginButtonLabelSize = m_gui.GetTextSize(loginButtonLabel);

                Container loginButtonContainer = {};
                loginButtonContainer.ID = "LoginButtonContainer";
                loginButtonContainer.Size = Vector2(loginTextValueSize.X + loginButtonLabelSize.X, loginTextValueSize.Y);
                // NOTE: Transparent background
                loginButtonContainer.BgColor = Rgba(0, 0, 0, 0);
                loginButtonContainer.DrawContent = [this, &loginButtonLabel, &loginTextValue](const ContainerState& State) {
                    // LOGIN TEXT
                    Text LoginText = {};
                    LoginText.Value = loginTextValue;

                    m_gui.DrawText(LoginText);

                    // LOGIN BUTTON
                    Button loginButton = {};
                    loginButton.Label = loginButtonLabel;
                    loginButton.Size = m_gui.GetTextSize(loginButtonLabel);
                    // NOTE: Transparent background
                    loginButton.BgColor = Rgba(0, 0, 0, 0);
                    // NOTE: Transparent background
                    loginButton.BgColorActive = Rgba(0, 0, 0, 0);
                    // NOTE: Transparent background
                    loginButton.BgColorHovered = Rgba(0, 0, 0, 0);
                    loginButton.TextColor = Rgba(255, 255, 0, 255);
                    loginButton.OnClick = [this]() {
                        HandleLoginButtonClicked();
                    };

                    m_gui.DisplayInline();
                    Vector2 loginTextSize = m_gui.GetTextSize(LoginText.Value);
                    m_gui.SetPositionX(loginTextSize.X + 2.0f);
                    m_gui.DrawButton(loginButton);
                };

                m_gui.AlignCenter(loginButtonContainer.Size);
                m_gui.DrawContainer(loginButtonContainer);
            };

            m_gui.SetPositionY(titleContainer.Size.Y + inputsContainer.Size.Y);
            m_gui.DrawContainer(buttonsContainer);
        };

        m_gui.AlignCenter(registerFormContainer.Size);
        m_gui.DrawContainer(registerFormContainer);
    };

    m_gui.DrawWindow(registerWindow);
}

// ***********
// * PRIVATE *
// ***********
void RegisterLayer::HandleLoginButtonClicked()
{
    m_navigation.GoBack();
}

void RegisterLayer::HandleRegisterButtonClicked()
{
    RegisterParams registerParams = {};
    registerParams.firstName = m_firstName;
    registerParams.lastName = m_lastName;
    registerParams.email = m_email;
    registerParams.password = m_password;
    m_authApi.Register(registerParams);
}

void RegisterLayer::HandleRegistered(const RegisteredEvent& registeredEvent)
{
    m_navigation.GoToChatScreen();
}

void RegisterLayer::Reset()
{
    m_firstName = "";
    m_lastName = "";
    m_email = "";
    m_password = "";
}
