#include "layer/LoginLayer.h"

// **********
// * PUBLIC *
// **********
LoginLayer::LoginLayer(const std::string& ID, const Gui& Gui) : Layer(ID, std::make_shared<Logger>(ID, "client/src/layer/LoginLayer")), m_Gui(Gui)
{}

LoginLayer::LoginLayer(const std::string& ID, const Gui& Gui, const std::shared_ptr<Logger>& Logger) : Layer(ID, Logger), m_Gui(Gui)
{}

void LoginLayer::OnDetach()
{
    Reset();
}

void LoginLayer::OnSuspend()
{
    Reset();
}

void LoginLayer::OnRender()
{
    Window LoginWindow = {};
    LoginWindow.Name = "LoginWindow";
    LoginWindow.Size = m_Gui.GetViewportSize();
    LoginWindow.BgColor = Rgba(26, 30, 67, 255);
    LoginWindow.DrawContent = [this]() {
        const Vector2 LOGIN_WINDOW_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

        // LOGIN FORM CONTAINER
        Container LoginFormContainer = {};
        LoginFormContainer.ID = "LoginFormContainer";
        LoginFormContainer.Size = Vector2(LOGIN_WINDOW_AVAILABLE_SPACE.X * 0.30f, LOGIN_WINDOW_AVAILABLE_SPACE.Y * 0.60f);
        LoginFormContainer.CornerRounding = 10.f;
        LoginFormContainer.BgColor = Rgba(50, 56, 102, 255);
        LoginFormContainer.DrawContent = [this](const ContainerState& State) {
            const Vector2 LOGIN_FORM_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

            // TITLE CONTAINER
            Container TitleContainer = {};
            TitleContainer.ID = "TitleContainer";
            TitleContainer.Size = Vector2(LOGIN_FORM_CONTAINER_AVAILABLE_SPACE.X, LOGIN_FORM_CONTAINER_AVAILABLE_SPACE.Y * 0.25f);
            // NOTE: Transparent background
            TitleContainer.BgColor = Rgba(0, 0, 0, 0);
            TitleContainer.DrawContent = [this](const ContainerState& State) {
                // TITLE TEXT
                Text TitleText = {};
                TitleText.Value = "Login";
                TitleText.Height = 40.0f;

                Vector2 TitleTextSize = m_Gui.GetTextSize(TitleText);
                m_Gui.AlignCenter(TitleTextSize);
                m_Gui.DrawText(TitleText);
            };

            m_Gui.DrawContainer(TitleContainer);

            // INPUTS CONTAINER
            Container InputsContainer = {};
            InputsContainer.ID = "InputsContainer";
            InputsContainer.Size = Vector2(LOGIN_FORM_CONTAINER_AVAILABLE_SPACE.X, LOGIN_FORM_CONTAINER_AVAILABLE_SPACE.Y * 0.50f);
            InputsContainer.Padding = Vector2(15.0f, 0.0f);
            // NOTE: Transparent background
            InputsContainer.BgColor = Rgba(0, 0, 0, 0);
            InputsContainer.DrawContent = [this](const ContainerState& State) {
                const Vector2 INPUTS_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

                // EMAIL TEXT INPUT SINGLELINE
                Placeholder EmailTextInputPlaceholder = {};
                EmailTextInputPlaceholder.Color = Rgba(120, 125, 172, 255);
                EmailTextInputPlaceholder.Text = "Email";

                TextInput EmailTextInput = {};
                EmailTextInput.ID = "EmailTextInput";
                EmailTextInput.Padding = Vector2(15.0f, 15.0f);
                EmailTextInput.CornerRounding = 10.f;
                EmailTextInput.BgColor = Rgba(26, 30, 67, 255);
                EmailTextInput.Placeholder = EmailTextInputPlaceholder;

                TextInputSingleline EmailTextInputSingleline = {};
                EmailTextInputSingleline.TextInput = EmailTextInput;
                EmailTextInputSingleline.Width = INPUTS_CONTAINER_AVAILABLE_SPACE.X;

                m_Gui.DrawTextInputSingleline(m_Email, EmailTextInputSingleline);

                // PASSWORD TEXT INPUT SINGLELINE
                Placeholder PasswordTextInputPlaceholder = {};
                PasswordTextInputPlaceholder.Color = Rgba(120, 125, 172, 255);
                PasswordTextInputPlaceholder.Text = "Password";

                TextInput PasswordTextInput = {};
                PasswordTextInput.ID = "PasswordTextInput";
                PasswordTextInput.Padding = Vector2(15.0f, 15.0f);
                PasswordTextInput.CornerRounding = 10.f;
                PasswordTextInput.BgColor = Rgba(26, 30, 67, 255);
                PasswordTextInput.Placeholder = PasswordTextInputPlaceholder;

                TextInputSingleline PasswordTextInputSingleline = {};
                PasswordTextInputSingleline.TextInput = PasswordTextInput;
                PasswordTextInputSingleline.Width = INPUTS_CONTAINER_AVAILABLE_SPACE.X;

                m_Gui.SetPositionY(INPUTS_CONTAINER_AVAILABLE_SPACE.Y * 0.50f);
                m_Gui.DrawTextInputSingleline(m_Password, PasswordTextInputSingleline);
            };

            m_Gui.SetPositionY(TitleContainer.Size.Y);
            m_Gui.DrawContainer(InputsContainer);

            // BUTTONS CONTAINER
            Container ButtonsContainer = {};
            ButtonsContainer.ID = "ButtonsContainer";
            ButtonsContainer.Size = Vector2(LOGIN_FORM_CONTAINER_AVAILABLE_SPACE.X, LOGIN_FORM_CONTAINER_AVAILABLE_SPACE.Y * 0.25f);
            ButtonsContainer.Padding = Vector2(15.0f, 0.0f);
            // NOTE: Transparent background
            ButtonsContainer.BgColor = Rgba(0, 0, 0, 0);
            ButtonsContainer.DrawContent = [this](const ContainerState& State) {
                const Vector2 BUTTONS_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

                // LOGIN BUTTON
                Button LoginButton = {};
                LoginButton.Label = "Login";
                LoginButton.Size = Vector2(BUTTONS_CONTAINER_AVAILABLE_SPACE.X, BUTTONS_CONTAINER_AVAILABLE_SPACE.Y * 0.50f);
                LoginButton.BgColor = Rgba(200, 30, 30, 255); // Red button
                LoginButton.BgColorActive = Rgba(150, 0, 0, 255); // Darker red when active
                LoginButton.BgColorHovered = Rgba(255, 100, 100, 255); // Lighter red on hover
                LoginButton.CornerRounding = 50.0f;
                LoginButton.IsDisabled = m_Email.empty() || m_Password.empty();
                LoginButton.OnClick = [this]() {
                    OnLoginButtonClick(m_Email, m_Password);
                };

                m_Gui.DrawButton(LoginButton);

                // REGISTER BUTTON CONTAINER
                const std::string& RegisterTextValue = "Don't have an account ? ";
                Vector2 RegisterTextValueSize = m_Gui.GetTextSize(RegisterTextValue);

                const std::string& RegisterButtonLabel = "Register";
                Vector2 RegisterButtonLabelSize = m_Gui.GetTextSize(RegisterButtonLabel);

                Container RegisterButtonContainer = {};
                RegisterButtonContainer.ID = "RegisterButtonContainer";
                RegisterButtonContainer.Size = Vector2(RegisterTextValueSize.X + RegisterButtonLabelSize.X, RegisterTextValueSize.Y);
                // NOTE: Transparent background
                RegisterButtonContainer.BgColor = Rgba(0, 0, 0, 0);
                RegisterButtonContainer.DrawContent = [this, &RegisterButtonLabel, &RegisterTextValue](const ContainerState& State) {
                    // REGISTER TEXT
                    Text RegisterText = {};
                    RegisterText.Value = RegisterTextValue;

                    m_Gui.DrawText(RegisterText);

                    // REGISTER BUTTON
                    Button RegisterButton = {};
                    RegisterButton.Label = RegisterButtonLabel;
                    RegisterButton.Size = m_Gui.GetTextSize(RegisterButtonLabel);
                    // NOTE: Transparent background
                    RegisterButton.BgColor = Rgba(0, 0, 0, 0);
                    // NOTE: Transparent background
                    RegisterButton.BgColorActive = Rgba(0, 0, 0, 0);
                    // NOTE: Transparent background
                    RegisterButton.BgColorHovered = Rgba(0, 0, 0, 0);
                    RegisterButton.TextColor = Rgba(255, 255, 0, 255);
                    RegisterButton.OnClick = [this]() {
                        OnRegisterButtonClick();
                    };

                    m_Gui.DisplayInline();
                    Vector2 RegisterTextSize = m_Gui.GetTextSize(RegisterText.Value);
                    m_Gui.SetPositionX(RegisterTextSize.X + 2.0f);
                    m_Gui.DrawButton(RegisterButton);
                };

                m_Gui.AlignCenter(RegisterButtonContainer.Size);
                m_Gui.DrawContainer(RegisterButtonContainer);
            };

            m_Gui.SetPositionY(TitleContainer.Size.Y + InputsContainer.Size.Y);
            m_Gui.DrawContainer(ButtonsContainer);
        };

        m_Gui.AlignCenter(LoginFormContainer.Size);
        m_Gui.DrawContainer(LoginFormContainer);
    };

    m_Gui.DrawWindow(LoginWindow);
}

// ***********
// * PRIVATE *
// ***********
void LoginLayer::Reset()
{
    m_Email = "";
    m_Password = "";
}
