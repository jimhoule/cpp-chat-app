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
    Gui::Window registerWindow = {};
    registerWindow.name = "RegisterWindow";
    registerWindow.size = m_gui.GetViewportSize();
    registerWindow.bgColor = Rgba(26, 30, 67, 255);
    registerWindow.DrawContent = [this]() {
        const Vector2 registerWindowAvailableSpace = m_gui.GetAvailableSpace();

        // REGISTER FORM CONTAINER
        Gui::Container registerFormContainer = {};
        registerFormContainer.id = "RegisterFormContainer";
        registerFormContainer.size = Vector2(registerWindowAvailableSpace.x * 0.30f, registerWindowAvailableSpace.y * 0.70f);
        registerFormContainer.cornerRounding = 10.f;
        registerFormContainer.bgColor = Rgba(50, 56, 102, 255);
        registerFormContainer.DrawContent = [this](const Gui::ContainerState& state) {
            const Vector2 registerFormContainerAvailableSpace = m_gui.GetAvailableSpace();

            // TITLE CONTAINER
            Gui::Container titleContainer = {};
            titleContainer.id = "TitleContainer";
            titleContainer.size = Vector2(registerFormContainerAvailableSpace.x, registerFormContainerAvailableSpace.y * 0.20f);
            // NOTE: Transparent background
            titleContainer.bgColor = Rgba(0, 0, 0, 0);
            titleContainer.DrawContent = [this](const Gui::ContainerState& state) {
                // TITLE TEXT
                Gui::Text titleText = {};
                titleText.value = "Register";
                titleText.height = 40.0f;

                Vector2 titleTextSize = m_gui.GetTextSize(titleText);
                m_gui.AlignCenter(titleTextSize);
                m_gui.DrawText(titleText);
            };

            m_gui.DrawContainer(titleContainer);

            // INPUTS CONTAINER
            Gui::Container inputsContainer = {};
            inputsContainer.id = "InputsContainer";
            inputsContainer.size = Vector2(registerFormContainerAvailableSpace.x, registerFormContainerAvailableSpace.y * 0.60f);
            inputsContainer.padding = Vector2(15.0f, 0.0f);
            // NOTE: Transparent background
            inputsContainer.bgColor = Rgba(0, 0, 0, 0);
            inputsContainer.DrawContent = [this](const Gui::ContainerState& state) {
                const Vector2 inputsContainerAvailableSpace = m_gui.GetAvailableSpace();

                // FIRSTNAME TEXT INPUT SINGLELINE
                Gui::Placeholder firstNameTextInputPlaceholder = {};
                firstNameTextInputPlaceholder.color = Rgba(120, 125, 172, 255);
                firstNameTextInputPlaceholder.text = " FirstName";

                Gui::TextInput firstNameTextInput = {};
                firstNameTextInput.id = "FirstNameTextInput";
                firstNameTextInput.padding = Vector2(15.0f, 15.0f);
                firstNameTextInput.cornerRounding = 10.f;
                firstNameTextInput.bgColor = Rgba(26, 30, 67, 255);
                firstNameTextInput.placeholder = firstNameTextInputPlaceholder;

                Gui::TextInputSingleline firstNameTextInputSingleline = {};
                firstNameTextInputSingleline.textInput = firstNameTextInput;
                firstNameTextInputSingleline.width = inputsContainerAvailableSpace.x;

                m_gui.DrawTextInputSingleline(m_firstName, firstNameTextInputSingleline);

                // LASTNAME TEXT INPUT SINGLELINE
                Gui::Placeholder lastNameTextInputPlaceholder = {};
                lastNameTextInputPlaceholder.color = Rgba(120, 125, 172, 255);
                lastNameTextInputPlaceholder.text = " LastName";

                Gui::TextInput lastNameTextInput = {};
                lastNameTextInput.id = "LastNameTextInput";
                lastNameTextInput.padding = Vector2(15.0f, 15.0f);
                lastNameTextInput.cornerRounding = 10.f;
                lastNameTextInput.bgColor = Rgba(26, 30, 67, 255);
                lastNameTextInput.placeholder = lastNameTextInputPlaceholder;

                Gui::TextInputSingleline lastNameTextInputSingleline = {};
                lastNameTextInputSingleline.textInput = lastNameTextInput;
                lastNameTextInputSingleline.width = inputsContainerAvailableSpace.x;

                m_gui.SetPositionY(inputsContainerAvailableSpace.y * 0.25f);
                m_gui.DrawTextInputSingleline(m_lastName, lastNameTextInputSingleline);

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

                m_gui.SetPositionY(inputsContainerAvailableSpace.y * 0.50f);
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

                m_gui.SetPositionY(inputsContainerAvailableSpace.y * 0.75f);
                m_gui.DrawTextInputSingleline(m_password, passwordTextInputSingleline);
            };

            m_gui.SetPositionY(titleContainer.size.y);
            m_gui.DrawContainer(inputsContainer);

            // BUTTONS CONTAINER
            Gui::Container buttonsContainer = {};
            buttonsContainer.id = "ButtonsContainer";
            buttonsContainer.size = Vector2(registerFormContainerAvailableSpace.x, registerFormContainerAvailableSpace.y * 0.20f);
            buttonsContainer.padding = Vector2(15.0f, 0.0f);
            // NOTE: Transparent background
            buttonsContainer.bgColor = Rgba(0, 0, 0, 0);
            buttonsContainer.DrawContent = [this](const Gui::ContainerState& state) {
                const Vector2 buttonsContainerAvailableSpace = m_gui.GetAvailableSpace();

                // REGISTER BUTTON
                Gui::Button registerButton = {};
                registerButton.label = "Register";
                registerButton.size = Vector2(buttonsContainerAvailableSpace.x, buttonsContainerAvailableSpace.y * 0.50f);
                registerButton.bgColor = Rgba(200, 30, 30, 255); // Red button
                registerButton.bgColorActive = Rgba(150, 0, 0, 255); // Darker red when active
                registerButton.bgColorHovered = Rgba(255, 100, 100, 255); // Lighter red on hover
                registerButton.cornerRounding = 50.0f;
                registerButton.isDisabled = m_email.empty() || m_password.empty();
                registerButton.OnClick = [this]() {
                    HandleRegisterButtonClicked();
                };

                m_gui.DrawButton(registerButton);

                // LOGIN BUTTON CONTAINER
                const std::string& loginTextValue = "Already have an account ? ";
                Vector2 loginTextValueSize = m_gui.GetTextSize(loginTextValue);

                const std::string& loginButtonLabel = "Login";
                Vector2 loginButtonLabelSize = m_gui.GetTextSize(loginButtonLabel);

                Gui::Container loginButtonContainer = {};
                loginButtonContainer.id = "LoginButtonContainer";
                loginButtonContainer.size = Vector2(loginTextValueSize.x + loginButtonLabelSize.x, loginTextValueSize.y);
                // NOTE: Transparent background
                loginButtonContainer.bgColor = Rgba(0, 0, 0, 0);
                loginButtonContainer.DrawContent = [this, &loginButtonLabel, &loginTextValue](const Gui::ContainerState& state) {
                    // LOGIN TEXT
                    Gui::Text loginText = {};
                    loginText.value = loginTextValue;

                    m_gui.DrawText(loginText);

                    // LOGIN BUTTON
                    Gui::Button loginButton = {};
                    loginButton.label = loginButtonLabel;
                    loginButton.size = m_gui.GetTextSize(loginButtonLabel);
                    // NOTE: Transparent background
                    loginButton.bgColor = Rgba(0, 0, 0, 0);
                    // NOTE: Transparent background
                    loginButton.bgColorActive = Rgba(0, 0, 0, 0);
                    // NOTE: Transparent background
                    loginButton.bgColorHovered = Rgba(0, 0, 0, 0);
                    loginButton.textColor = Rgba(255, 255, 0, 255);
                    loginButton.OnClick = [this]() {
                        HandleLoginButtonClicked();
                    };

                    m_gui.DisplayInline();
                    Vector2 loginTextSize = m_gui.GetTextSize(loginText.value);
                    m_gui.SetPositionX(loginTextSize.x + 2.0f);
                    m_gui.DrawButton(loginButton);
                };

                m_gui.AlignCenter(loginButtonContainer.size);
                m_gui.DrawContainer(loginButtonContainer);
            };

            m_gui.SetPositionY(titleContainer.size.y + inputsContainer.size.y);
            m_gui.DrawContainer(buttonsContainer);
        };

        m_gui.AlignCenter(registerFormContainer.size);
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
    AuthApi::RegisterParams registerParams = {};
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
