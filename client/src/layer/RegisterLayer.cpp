#include "layer/RegisterLayer.h"

// **********
// * PUBLIC *
// **********
RegisterLayer::RegisterLayer(const std::string& ID, const Gui& Gui) : Layer(ID), m_Gui(Gui)
{}

void RegisterLayer::OnDetach()
{
    Reset();
}

void RegisterLayer::OnRender()
{
    Window RegisterWindow = {};
    RegisterWindow.Name = "RegisterWindow";
    RegisterWindow.Size = m_Gui.GetViewportSize();
    RegisterWindow.BgColor = Rgba(26, 30, 67, 255);
    RegisterWindow.DrawContent = [this]() {
        const Vector2 REGISTER_WINDOW_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

        // REGISTER FORM CONTAINER
        Container RegisterFormContainer = {};
        RegisterFormContainer.ID = "RegisterFormContainer";
        RegisterFormContainer.Size = Vector2(REGISTER_WINDOW_AVAILABLE_SPACE.X * 0.30f, REGISTER_WINDOW_AVAILABLE_SPACE.Y * 0.70f);
        RegisterFormContainer.CornerRounding = 10.f;
        RegisterFormContainer.BgColor = Rgba(50, 56, 102, 255);
        RegisterFormContainer.DrawContent = [this](const ContainerState& State) {
            const Vector2 REGISTER_FORM_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

            // TITLE CONTAINER
            Container TitleContainer = {};
            TitleContainer.ID = "TitleContainer";
            TitleContainer.Size = Vector2(REGISTER_FORM_CONTAINER_AVAILABLE_SPACE.X, REGISTER_FORM_CONTAINER_AVAILABLE_SPACE.Y * 0.20f);
            // NOTE: Transparent background
            TitleContainer.BgColor = Rgba(0, 0, 0, 0);
            TitleContainer.DrawContent = [this](const ContainerState& State) {
                // TITLE TEXT
                Text TitleText = {};
                TitleText.Value = "Register";
                TitleText.Height = 40.0f;

                Vector2 TitleTextSize = m_Gui.GetTextSize(TitleText);
                m_Gui.AlignCenter(TitleTextSize);
                m_Gui.DrawText(TitleText);
            };

            m_Gui.DrawContainer(TitleContainer);

            // INPUTS CONTAINER
            Container InputsContainer = {};
            InputsContainer.ID = "InputsContainer";
            InputsContainer.Size = Vector2(REGISTER_FORM_CONTAINER_AVAILABLE_SPACE.X, REGISTER_FORM_CONTAINER_AVAILABLE_SPACE.Y * 0.60f);
            InputsContainer.Padding = Vector2(15.0f, 0.0f);
            // NOTE: Transparent background
            InputsContainer.BgColor = Rgba(0, 0, 0, 0);
            InputsContainer.DrawContent = [this](const ContainerState& State) {
                const Vector2 INPUTS_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

                // FIRSTNAME TEXT INPUT SINGLELINE
                Placeholder FirstNameTextInputPlaceholder = {};
                FirstNameTextInputPlaceholder.Color = Rgba(120, 125, 172, 255);
                FirstNameTextInputPlaceholder.Text = " FirstName";

                TextInput FirstNameTextInput = {};
                FirstNameTextInput.ID = "FirstNameTextInput";
                FirstNameTextInput.Padding = Vector2(15.0f, 15.0f);
                FirstNameTextInput.CornerRounding = 10.f;
                FirstNameTextInput.BgColor = Rgba(26, 30, 67, 255);
                FirstNameTextInput.Placeholder = FirstNameTextInputPlaceholder;

                TextInputSingleline FirstNameTextInputSingleline = {};
                FirstNameTextInputSingleline.TextInput = FirstNameTextInput;
                FirstNameTextInputSingleline.Width = INPUTS_CONTAINER_AVAILABLE_SPACE.X;

                m_Gui.DrawTextInputSingleline(m_FirstName, FirstNameTextInputSingleline);

                // LASTNAME TEXT INPUT SINGLELINE
                Placeholder LastNameTextInputPlaceholder = {};
                LastNameTextInputPlaceholder.Color = Rgba(120, 125, 172, 255);
                LastNameTextInputPlaceholder.Text = " LastName";

                TextInput LastNameTextInput = {};
                LastNameTextInput.ID = "LastNameTextInput";
                LastNameTextInput.Padding = Vector2(15.0f, 15.0f);
                LastNameTextInput.CornerRounding = 10.f;
                LastNameTextInput.BgColor = Rgba(26, 30, 67, 255);
                LastNameTextInput.Placeholder = LastNameTextInputPlaceholder;

                TextInputSingleline LastNameTextInputSingleline = {};
                LastNameTextInputSingleline.TextInput = LastNameTextInput;
                LastNameTextInputSingleline.Width = INPUTS_CONTAINER_AVAILABLE_SPACE.X;

                m_Gui.SetPositionY(INPUTS_CONTAINER_AVAILABLE_SPACE.Y * 0.25f);
                m_Gui.DrawTextInputSingleline(m_LastName, LastNameTextInputSingleline);

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

                m_Gui.SetPositionY(INPUTS_CONTAINER_AVAILABLE_SPACE.Y * 0.50f);
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

                m_Gui.SetPositionY(INPUTS_CONTAINER_AVAILABLE_SPACE.Y * 0.75f);
                m_Gui.DrawTextInputSingleline(m_Password, PasswordTextInputSingleline);
            };

            m_Gui.SetPositionY(TitleContainer.Size.Y);
            m_Gui.DrawContainer(InputsContainer);

            // BUTTONS CONTAINER
            Container ButtonsContainer = {};
            ButtonsContainer.ID = "ButtonsContainer";
            ButtonsContainer.Size = Vector2(REGISTER_FORM_CONTAINER_AVAILABLE_SPACE.X, REGISTER_FORM_CONTAINER_AVAILABLE_SPACE.Y * 0.20f);
            ButtonsContainer.Padding = Vector2(15.0f, 0.0f);
            // NOTE: Transparent background
            ButtonsContainer.BgColor = Rgba(0, 0, 0, 0);
            ButtonsContainer.DrawContent = [this](const ContainerState& State) {
                const Vector2 BUTTONS_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

                // REGISTER BUTTON
                Button RegisterButton = {};
                RegisterButton.Label = "Register";
                RegisterButton.Size = Vector2(BUTTONS_CONTAINER_AVAILABLE_SPACE.X, BUTTONS_CONTAINER_AVAILABLE_SPACE.Y * 0.50f);
                RegisterButton.BgColor = Rgba(200, 30, 30, 255); // Red button
                RegisterButton.BgColorActive = Rgba(150, 0, 0, 255); // Darker red when active
                RegisterButton.BgColorHovered = Rgba(255, 100, 100, 255); // Lighter red on hover
                RegisterButton.CornerRounding = 50.0f;
                RegisterButton.IsDisabled = m_Email.empty() || m_Password.empty();
                RegisterButton.OnClick = [this]() {
                    OnRegisterButtonClick(m_FirstName, m_LastName, m_Email, m_Password);
                };

                m_Gui.DrawButton(RegisterButton);

                // LOGIN BUTTON CONTAINER
                const std::string& LoginTextValue = "Already have an account ? ";
                Vector2 LoginTextValueSize = m_Gui.GetTextSize(LoginTextValue);

                const std::string& LoginButtonLabel = "Login";
                Vector2 LoginButtonLabelSize = m_Gui.GetTextSize(LoginButtonLabel);

                Container LoginButtonContainer = {};
                LoginButtonContainer.ID = "LoginButtonContainer";
                LoginButtonContainer.Size = Vector2(LoginTextValueSize.X + LoginButtonLabelSize.X, LoginTextValueSize.Y);
                // NOTE: Transparent background
                LoginButtonContainer.BgColor = Rgba(0, 0, 0, 0);
                LoginButtonContainer.DrawContent = [this, &LoginButtonLabel, &LoginTextValue](const ContainerState& State) {
                    // LOGIN TEXT
                    Text LoginText = {};
                    LoginText.Value = LoginTextValue;

                    m_Gui.DrawText(LoginText);

                    // LOGIN BUTTON
                    Button LoginButton = {};
                    LoginButton.Label = LoginButtonLabel;
                    LoginButton.Size = m_Gui.GetTextSize(LoginButtonLabel);
                    // NOTE: Transparent background
                    LoginButton.BgColor = Rgba(0, 0, 0, 0);
                    // NOTE: Transparent background
                    LoginButton.BgColorActive = Rgba(0, 0, 0, 0);
                    // NOTE: Transparent background
                    LoginButton.BgColorHovered = Rgba(0, 0, 0, 0);
                    LoginButton.TextColor = Rgba(255, 255, 0, 255);
                    LoginButton.OnClick = [this]() {
                        OnLoginButtonClick();
                    };

                    m_Gui.DisplayInline();
                    Vector2 LoginTextSize = m_Gui.GetTextSize(LoginText.Value);
                    m_Gui.SetPositionX(LoginTextSize.X + 2.0f);
                    m_Gui.DrawButton(LoginButton);
                };

                m_Gui.AlignCenter(LoginButtonContainer.Size);
                m_Gui.DrawContainer(LoginButtonContainer);
            };

            m_Gui.SetPositionY(TitleContainer.Size.Y + InputsContainer.Size.Y);
            m_Gui.DrawContainer(ButtonsContainer);
        };

        m_Gui.AlignCenter(RegisterFormContainer.Size);
        m_Gui.DrawContainer(RegisterFormContainer);
    };

    m_Gui.DrawWindow(RegisterWindow);
}

// ***********
// * PRIVATE *
// ***********
void RegisterLayer::Reset()
{
    m_FirstName = "";
    m_LastName = "";
    m_Email = "";
    m_Password = "";
}
