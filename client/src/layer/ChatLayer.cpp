#include "layer/ChatLayer.h"

#include "deserializer/UserAuthenticatedSocketEventPayloadDeserializer.h"

constexpr int SERVER_PORT =  5000;

// **********
// * PUBLIC *
// **********
ChatLayer::ChatLayer(const std::string& ID, const SocketClient& SocketClient, const Gui& Gui) : Layer(ID, std::make_shared<Logger>(ID, "client/src/layer/ChatLayer")), m_SocketClient(SocketClient), m_Gui(Gui)
{}

ChatLayer::ChatLayer(const std::string& ID, const SocketClient& SocketClient, const Gui& Gui, const std::shared_ptr<Logger>& Logger) : Layer(ID, Logger), m_SocketClient(SocketClient), m_Gui(Gui)
{}

void ChatLayer::OnAttach()
{
    // Users
    User CurrentUser = {};
    CurrentUser.ID = "CurrentUser";
    CurrentUser.FirstName = "Olivier";
    CurrentUser.LastName = "Perron";
    CurrentUser.ImageUrl = "http://fake.iamge.url";

    m_CurrentUser = std::make_shared<User>(CurrentUser);

    User User1 = {};
    User1.ID = "User1";
    User1.FirstName = "Marc";
    User1.LastName = "Bum";
    User1.ImageUrl = "http://fake.iamge.url";

    User User2 = {};
    User2.ID = "User2";
    User2.FirstName = "Simon";
    User2.LastName = "Robichaud";
    User2.ImageUrl = "http://fake.iamge.url";

    m_Users = {
        std::make_shared<User>(User1),
        std::make_shared<User>(User2),
    };

    // Conversations
    Conversation Conversation1 = {};
    Conversation1.ID = "Conversation1";
    Conversation1.Users = { CurrentUser, User1 };
    Conversation1.Messages = {};
    Conversation1.CreatedAt = std::time(0);

    Conversation Conversation2 = {};
    Conversation2.ID = "Conversation2";
    Conversation2.Users = { CurrentUser, User2 };
    Conversation2.Messages = {};
    Conversation2.CreatedAt = std::time(0);

    m_Conversations = {
        std::make_shared<Conversation>(Conversation1),
        std::make_shared<Conversation>(Conversation2)
    };

    m_SelectedConversation = m_Conversations[0];

    // Socket
    UserAuthenticatedSocketEventPayloadDeserializer UserAuthenticatedSocketEventPayloadDeserializer = {};
    SocketClientEventHandler HandlerUserAuthenticated = [this, &UserAuthenticatedSocketEventPayloadDeserializer](const std::string& SerializedUserAuthenticatedSocketEventPayload) {
        // Gets registered socket event payload
        const UserAuthenticatedSocketEventPayload& UserAuthenticatedSocketEventPayload = UserAuthenticatedSocketEventPayloadDeserializer.Deserialize(SerializedUserAuthenticatedSocketEventPayload);
        m_Logger->Info("Authenticated user ID: " + UserAuthenticatedSocketEventPayload.User.ID);
    };

    m_SocketClient.On(SocketEventName::USER_AUTHENTICATED, HandlerUserAuthenticated);

    // Textures
    m_BlankImageTexture.Load("../../assets/Blank.jpg", 0);
    m_BlankImageTexture.Bind();

    m_ClosableImageTexture.Load("../../assets/Closable.png", 1);
    m_ClosableImageTexture.Bind();

    m_WhiteLeftArrowImageTexture.Load("../../assets/WhiteLeftArrow.png", 2);
    m_WhiteLeftArrowImageTexture.Bind();
}

void ChatLayer::OnDetach()
{
    Reset();
}

void ChatLayer::OnRender()
{
    // WINDOW
    Window ChatWindow = {};
    ChatWindow.Name = "ChatWindow";
    ChatWindow.Size = m_Gui.GetViewportSize();
    ChatWindow.BgColor = Rgba(26, 30, 67, 255);
    ChatWindow.DrawContent = [this]() {
        const Vector2 MAIN_WINDOW_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

        // NAVBAR CONTAINER
        Container NavbarContainer = {};
        NavbarContainer.ID = "NavbarContainer";
        NavbarContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X, MAIN_WINDOW_AVAILABLE_SPACE.Y * 0.15f);
        NavbarContainer.Padding = Vector2(15.0f, 15.0f);
        // NOTE: Transparent background
        NavbarContainer.BgColor = Rgba(0, 0, 0, 0);
        NavbarContainer.DrawContent = [this](const ContainerState& State) {
            const Vector2 NAVBAR_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

            // NAVBAR
            Container Navbar = {};
            Navbar.ID = "Navbar";
            Navbar.Size = Vector2(NAVBAR_CONTAINER_AVAILABLE_SPACE);
            Navbar.CornerRounding = 10.f;
            Navbar.BgColor = Rgba(50, 56, 102, 255);
            Navbar.DrawContent = [this](const ContainerState& State) {
                const Vector2 NAVBAR_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

                // SEARCH TEXT INPUT CONTAINER
                Container SearchTextInputContainer = {};
                SearchTextInputContainer.ID = "SearchTextInputContainer";
                SearchTextInputContainer.Size = Vector2(NAVBAR_AVAILABLE_SPACE.X * 0.50f, NAVBAR_AVAILABLE_SPACE.Y);
                SearchTextInputContainer.CornerRounding = 10.0f;
                // NOTE: Transparent background
                SearchTextInputContainer.BgColor = Rgba(0, 0, 0, 0);
                SearchTextInputContainer.DrawContent = [this](const ContainerState& State) {
                    const Vector2 SEARCH_TEXTINPUT_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

                    // SEARCH MODAL
                    Modal SearchModal = {};
                    SearchModal.ID = "SearchModal";
                    SearchModal.Size = m_Gui.GetViewportSize();

                    Container SearchModalHeaderContainer = {};
                    SearchModalHeaderContainer.ID = "SearchModalHeaderContainer";
                    SearchModalHeaderContainer.Size = Vector2(SearchModal.Size.X , 0.0f);
                    // NOTE: Transparent background
                    SearchModalHeaderContainer.BgColor = Rgba(0, 0, 0, 0);
                    SearchModalHeaderContainer.CornerRounding = SearchModal.CornerRounding;
                    SearchModalHeaderContainer.IsAutoResizableY = true;
                    SearchModalHeaderContainer.DrawContent = [this, &SearchModal](const ContainerState& State) {
                        const Vector2 SEARCH_MODAL_HEADER_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

                        Container SearchModalCloseImageButtonContainer = {};
                        SearchModalCloseImageButtonContainer.ID = "SearchModalCloseImageButtonContainer";
                        SearchModalCloseImageButtonContainer.Size = Vector2(
                            SEARCH_MODAL_HEADER_CONTAINER_AVAILABLE_SPACE.Y ,
                            SEARCH_MODAL_HEADER_CONTAINER_AVAILABLE_SPACE.Y
                        );
                        SearchModalCloseImageButtonContainer.Padding = Vector2(10.0f, 10.0f);
                        // NOTE: Transparent background
                        SearchModalCloseImageButtonContainer.BgColor = Rgba(0, 0, 0, 0);
                        SearchModalCloseImageButtonContainer.DrawContent = [this, &SearchModal](const ContainerState& State) {
                            Image SearchModalCloseImageButtonImage = {};
                            SearchModalCloseImageButtonImage.TextureID = m_WhiteLeftArrowImageTexture.GetID();
                            SearchModalCloseImageButtonImage.Size = m_Gui.GetAvailableSpace();
                            SearchModalCloseImageButtonImage.TintColor = Rgba(255, 255, 255, 255);

                            ImageButton SearchModalCloseImageButton = {};
                            SearchModalCloseImageButton.ID = "SearchModalCloseImageButton";
                            SearchModalCloseImageButton.Image = SearchModalCloseImageButtonImage;
                            SearchModalCloseImageButton.TintColorHovered = Rgba(200, 200, 0, 255);
                            SearchModalCloseImageButton.OnClick = [this, &SearchModal]() {
                                m_Gui.CloseModal(SearchModal.ID);
                            };

                            m_Gui.DrawImageButton(SearchModalCloseImageButton);
                        };

                        m_Gui.DrawContainer(SearchModalCloseImageButtonContainer);

                        Placeholder SearchModalTextInputPlaceholder = {};
                        SearchModalTextInputPlaceholder.Text = "Enter search here...";

                        TextInput SearchModalTextInput = {};
                        SearchModalTextInput.ID = "SearchModalTextInput";
                        SearchModalTextInput.Padding = Vector2(15.0f, 15.0f);
                        SearchModalTextInput.CornerRounding = 10.f;
                        SearchModalTextInput.BgColor = Rgba(0, 0, 0, 0);
                        SearchModalTextInput.Placeholder  = SearchModalTextInputPlaceholder;

                        TextInputSingleline SearchModalTextInputSingleline = {};
                        SearchModalTextInputSingleline.TextInput = SearchModalTextInput;
                        SearchModalTextInputSingleline.Width = SEARCH_MODAL_HEADER_CONTAINER_AVAILABLE_SPACE.X - SEARCH_MODAL_HEADER_CONTAINER_AVAILABLE_SPACE.Y;

                        m_Gui.DisplayInline();
                        m_Gui.SetPositionX(SearchModalCloseImageButtonContainer.Size.X);
                        m_Gui.DrawTextInputSingleline(m_SearchValue, SearchModalTextInputSingleline);

                        Divider BottomDivider = {};
                        BottomDivider.Color = Rgba(100, 100, 100, 255);

                        m_Gui.SetPositionY(SEARCH_MODAL_HEADER_CONTAINER_AVAILABLE_SPACE.Y - BottomDivider.Height);
                        m_Gui.DrawDivider(BottomDivider);

                    };

                    Container SearchModalBodyContainer = {};
                    SearchModalBodyContainer.ID = "SearchModalBodyContainer";
                    SearchModalBodyContainer.Size = Vector2(SearchModal.Size.X , 0.0f);
                    SearchModalBodyContainer.Padding = Vector2(15.0f, 15.0f);
                    // NOTE: Transparent background
                    SearchModalBodyContainer.BgColor = Rgba(0, 0, 0, 0);
                    SearchModalBodyContainer.CornerRounding = SearchModal.CornerRounding;
                    SearchModalBodyContainer.IsAutoResizableY = true;
                    SearchModalBodyContainer.DrawContent = [this, &SearchModal](const ContainerState& State) {
                        if (m_SearchValue.empty()) return;

                        const Vector2 SEARCH_MODAL_BODY_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

                        std::function<std::vector<std::shared_ptr<User>>(std::string&)> Search = [this](std::string& Value) {
                            // Defines a lambda function as the predicate for filtering
                            std::function<bool(std::shared_ptr<User>)> IsFound = [&Value](std::shared_ptr<User> User) {
                                // Defines a lambda function for converting a char to a lowercase char
                                std::function<int(unsigned char)> ConvertCharToLowercase = [&Value](unsigned char Character){
                                    return std::tolower(Character);
                                };

                                std::string& LowercaseFirstName = User->FirstName;
                                std::transform(LowercaseFirstName.begin(), LowercaseFirstName.end(), LowercaseFirstName.begin(), ConvertCharToLowercase);

                                std::string& LowercaseLastName = User->LastName;
                                std::transform(LowercaseLastName.begin(), LowercaseLastName.end(), LowercaseLastName.begin(), ConvertCharToLowercase);

                                // std::string& LowercaseValue = m_SearchValue;
                                std::string& LowercaseValue = Value;
                                std::transform(LowercaseValue.begin(), LowercaseValue.end(), LowercaseValue.begin(), ConvertCharToLowercase);

                                return (
                                    LowercaseFirstName.find(LowercaseValue) != std::string::npos ||
                                    LowercaseLastName.find(LowercaseValue) != std::string::npos
                                );
                            };

                            // Copies elements that satisfy the predicate into the new vector
                            std::vector<std::shared_ptr<User>> FilteredUsers = {};
                            std::copy_if(m_Users.begin(), m_Users.end(), std::back_inserter(FilteredUsers), IsFound);

                            return std::move(FilteredUsers);
                        };

                        std::vector<std::shared_ptr<User>> Users = Search(m_SearchValue);
                        if (Users.empty()) return;

                        for (int Index = 0; Index < Users.size(); Index++)
                        {
                            std::shared_ptr<User> User = Users[Index];

                            // USER CONTAINER
                            Container UserContainer = {};
                            UserContainer.ID = User->ID;
                            UserContainer.Size = Vector2(SEARCH_MODAL_BODY_CONTAINER_AVAILABLE_SPACE.X, 40.0f);
                            UserContainer.CornerRounding = 10.f;
                            // NOTE: Transparent background
                            UserContainer.BgColor = Rgba(0, 0, 0, 0);
                            UserContainer.BgColorHovered = Rgba(50, 56, 102, 255);
                            UserContainer.IsAutoResizableY = true;
                            UserContainer.OnClick = [this, &SearchModal, &User]() {
                                m_Logger->Info("selected User with ID " + User->ID);
                                m_Gui.CloseModal(SearchModal.ID);
                                m_SearchValue = "";
                            };
                            UserContainer.DrawContent = [this, &User](const ContainerState& State) {
                                const Vector2 USER_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

                                // USER IMAGE
                                Image UserImage = {};
                                UserImage.TextureID = m_BlankImageTexture.GetID();
                                UserImage.Size = Vector2(USER_CONTAINER_AVAILABLE_SPACE.Y, USER_CONTAINER_AVAILABLE_SPACE.Y);
                                UserImage.CornerRounding = 10.0f;

                                m_Gui.DrawImage(UserImage);

                                // USER TEXT
                                Text UserText = {};
                                UserText.Value = User->FirstName + " " + User->LastName;

                                m_Gui.DisplayInline();
                                m_Gui.SetPositionX(UserImage.Size.X * 1.3f);
                                Vector2 TextSize = m_Gui.GetTextSize(UserText.Value);
                                m_Gui.AlignCenterY(TextSize.Y);
                                m_Gui.DrawText(UserText);
                            };

                            m_Gui.DrawContainer(UserContainer);
                        }
                    };

                    SearchModal.HeaderContainer = SearchModalHeaderContainer;
                    SearchModal.BodyContainer = SearchModalBodyContainer;

                    m_Gui.DrawModal(SearchModal);

                    // SEARCH TEXT INPUT
                    Placeholder SearchTextInputPlaceholder = {};
                    SearchTextInputPlaceholder.Color = Rgba(120, 125, 172, 255);
                    if (!m_Gui.AreAnyModalsOpen()) SearchTextInputPlaceholder.Text = "Enter search here...";

                    TextInput SearchTextInput = {};
                    SearchTextInput.ID = "SearchTextInput";
                    SearchTextInput.Padding = Vector2(15.0f, 15.0f);
                    SearchTextInput.CornerRounding = 10.f;
                    SearchTextInput.BgColor = Rgba(26, 30, 67, 255);
                    SearchTextInput.Placeholder = SearchTextInputPlaceholder;
                    SearchTextInput.OnClick = [this, &SearchModal]() {
                        m_Gui.OpenModal(SearchModal.ID);
                    };

                    TextInputSingleline SearchTextInputSingleline = {};
                    SearchTextInputSingleline.TextInput = SearchTextInput;
                    SearchTextInputSingleline.Width = SEARCH_TEXTINPUT_CONTAINER_AVAILABLE_SPACE.X * 0.80f;

                    // NOTE: SearchTextInputSingleline is only a trigger to show SearchModal
                    std::string EmptyValue = "";
                    float SearchTextInputSinglelineHeight = m_Gui.GetTextInputSinglelineHeight();
                    m_Gui.AlignCenter(Vector2(SearchTextInputSingleline.Width, SearchTextInputSinglelineHeight));
                    m_Gui.DrawTextInputSingleline(EmptyValue, SearchTextInputSingleline);
                };

                m_Gui.DrawContainer(SearchTextInputContainer);

                // SETTINGS CONTAINER
                Container SettingsContainer = {};
                SettingsContainer.ID = "SettingsContainer";
                SettingsContainer.Size = Vector2(NAVBAR_AVAILABLE_SPACE.X * 0.50f, NAVBAR_AVAILABLE_SPACE.Y);
                SettingsContainer.CornerRounding = 10.0f;
                // NOTE: Transparent background
                SettingsContainer.BgColor = Rgba(0, 0, 0, 0);
                SettingsContainer.DrawContent = [this](const ContainerState& State) {
                    const Vector2 SETTINGS_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

                    // CURRENT USER IMAGE CONTAINER
                    Container CurrentUserImageContainer = {};
                    CurrentUserImageContainer.ID = "CurrentUserImageContainer";
                    CurrentUserImageContainer.Size = Vector2(SETTINGS_CONTAINER_AVAILABLE_SPACE.Y, SETTINGS_CONTAINER_AVAILABLE_SPACE.Y);
                    CurrentUserImageContainer.CornerRounding = 10.f;
                    CurrentUserImageContainer.Padding = Vector2(10.0f, 10.0f);
                    // NOTE: Transparent background
                    CurrentUserImageContainer.BgColor = Rgba(0, 0, 0, 0);
                    CurrentUserImageContainer.DrawContent = [this](const ContainerState& State) {
                        // CURRENT USER IMAGE BUTTON
                        Image CurrentUserImage = {};
                        CurrentUserImage.TextureID = m_BlankImageTexture.GetID();
                        CurrentUserImage.Size = m_Gui.GetAvailableSpace();
                        CurrentUserImage.TintColor = Rgba(255, 255, 255, 255);
                        CurrentUserImage.CornerRounding = 10.0f;

                        ImageButton CurrentUserImageButton = {};
                        CurrentUserImageButton.ID = "CurrentUserImageButton";
                        CurrentUserImageButton.Image = CurrentUserImage;
                        CurrentUserImageButton.TintColorHovered = Rgba(200, 200, 0, 255);
                        CurrentUserImageButton.OnClick = [this]() {
                            m_IsSettingsDropDownMenuOpen = !m_IsSettingsDropDownMenuOpen;
                        };

                        m_Gui.DrawImageButton(CurrentUserImageButton);

                        // Closes setting drop down menu if a modal is open
                        if (m_Gui.AreAnyModalsOpen()) m_IsSettingsDropDownMenuOpen = false;
                        if (!m_IsSettingsDropDownMenuOpen) return;

                        // SETTINGS DROP DOWN MENU
                        DropDownMenuItem ProfileDropDownMenuItem = {};
                        ProfileDropDownMenuItem.Text = "Profile";
                        ProfileDropDownMenuItem.TextColor = Rgba(255, 255, 255, 255);
                        ProfileDropDownMenuItem.BgColorHovered = Rgba(50, 56, 102, 255);
                        ProfileDropDownMenuItem.OnClick = [this]() {
                            m_Logger->Info("Profile clicked!");
                        };

                        DropDownMenuItem PreferencesDropDownMenuItem = {};
                        PreferencesDropDownMenuItem.Text = "Preferences";
                        PreferencesDropDownMenuItem.TextColor = Rgba(255, 255, 255, 255);
                        PreferencesDropDownMenuItem.BgColorHovered = Rgba(50, 56, 102, 255);
                        PreferencesDropDownMenuItem.OnClick = [this]() {
                            m_Logger->Info("Preferences clicked!");
                        };

                        DropDownMenuItem LogoutDropDownMenuItem = {};
                        LogoutDropDownMenuItem.Text = "Logout";
                        LogoutDropDownMenuItem.TextColor = Rgba(255, 255, 255, 255);
                        LogoutDropDownMenuItem.BgColorHovered = Rgba(50, 56, 102, 255);
                        LogoutDropDownMenuItem.OnClick = [this]() {
                            OnLogoutButtonClick();
                        };

                        std::vector<std::shared_ptr<DropDownMenuItem>> DropDownMenuItems = {
                            std::make_shared<DropDownMenuItem>(ProfileDropDownMenuItem),
                            std::make_shared<DropDownMenuItem>(PreferencesDropDownMenuItem),
                            std::make_shared<DropDownMenuItem>(LogoutDropDownMenuItem),
                        };

                        // Calculates drop down menu size
                        Vector2 DropDownMenuSize = Vector2(0.0f, 0.0f);
                        for (std::shared_ptr<DropDownMenuItem> DropDownMenuItem : DropDownMenuItems)
                        {
                            Vector2 TextSize = m_Gui.GetTextSize(DropDownMenuItem->Text);

                            DropDownMenuSize.Y += TextSize.Y;
                            DropDownMenuSize.X = std::max(DropDownMenuSize.X, TextSize.X);
                        }

                        Border SettingsDropDownMenuBorder = {};
                        SettingsDropDownMenuBorder.Color = Rgba(255, 255, 255, 255);
                        SettingsDropDownMenuBorder.Height = 2.0f;

                        DropDownMenu SettingsDropDownMenu = {};
                        SettingsDropDownMenu.Border = SettingsDropDownMenuBorder;
                        SettingsDropDownMenu.Size = DropDownMenuSize;
                        SettingsDropDownMenu.OriginOffset = Vector2(-80.0f, 10.0f);
                        SettingsDropDownMenu.OuterPadding = Vector2(15.0f, 15.0f);
                        SettingsDropDownMenu.BgColor = Rgba(26, 30, 67, 255);
                        SettingsDropDownMenu.CornerRounding = 10.0f;
                        SettingsDropDownMenu.LineHeight = 10.0f;
                        SettingsDropDownMenu.Items = DropDownMenuItems;

                        m_Gui.DrawDropDownMenu(SettingsDropDownMenu);

                    };

                    m_Gui.SetPositionX(SETTINGS_CONTAINER_AVAILABLE_SPACE.X - CurrentUserImageContainer.Size.X);
                    m_Gui.DrawContainer(CurrentUserImageContainer);
                };

                m_Gui.DisplayInline();
                m_Gui.SetPositionX(SearchTextInputContainer.Size.X);
                m_Gui.DrawContainer(SettingsContainer);
            };

            m_Gui.DrawContainer(Navbar);
        };

        m_Gui.DrawContainer(NavbarContainer);

        // CHATS CONTAINER
        Container ChatsContainer = {};
        ChatsContainer.ID = "ChatsContainer";
        ChatsContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.25f, MAIN_WINDOW_AVAILABLE_SPACE.Y - NavbarContainer.Size.Y);
        ChatsContainer.Padding = Vector2(15.0f, 15.0f);
        // NOTE: Transparent background
        ChatsContainer.BgColor = Rgba(0, 0, 0, 0);
        ChatsContainer.DrawContent = [this](const ContainerState& State) {
            const Vector2 CHATS_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

            // CONVERSATIONS CONTAINER
            Container ConversationsContainer = {};
            ConversationsContainer.ID = "ConversationsContainer";
            ConversationsContainer.Size = Vector2(CHATS_CONTAINER_AVAILABLE_SPACE);
            ConversationsContainer.CornerRounding = 10.f;
            ConversationsContainer.BgColor = Rgba(50, 56, 102, 255);
            ConversationsContainer.DrawContent = [this](const ContainerState& State) {
                const Vector2 CONVERSATIONS_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

                // CONVERSATIONS NODE
                Node ConversationsNode = {};
                ConversationsNode.Name = "Conversations";
                ConversationsNode.DrawContent = [this]() {
                    const Vector2 CONVERSATIONS_NODE_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

                    for (int Index = 0; Index < m_Conversations.size(); Index++)
                    {
                        const std::shared_ptr<Conversation> Conversation = m_Conversations[Index];

                        // CONVERSATION CONTAINER
                        Rgba BgColor = Rgba(50, 56, 102, 255);
                        if (Conversation->ID == m_SelectedConversation->ID) BgColor = Rgba(100, 100, 100, 255);

                        Container ConversationContainer = {};
                        ConversationContainer.ID = "ConversationContainer" + Conversation->ID;
                        ConversationContainer.Size = Vector2(CONVERSATIONS_NODE_AVAILABLE_SPACE.X, CONVERSATIONS_NODE_AVAILABLE_SPACE.Y * 0.05f);
                        ConversationContainer.CornerRounding = 10.f;
                        ConversationContainer.BgColor = BgColor;
                        ConversationContainer.BgColorHovered = Rgba(0, 0, 0, 255);
                        ConversationContainer.IsAutoResizableY = true;
                        ConversationContainer.OnClick = [this, &Conversation]() {
                            m_SelectedConversation = Conversation;
                            m_Logger->Info("SELECTED CONVERSATION ID: " + m_SelectedConversation->ID);
                        };
                        ConversationContainer.DrawContent = [this, &Conversation, Index](const ContainerState& State) {
                            const Vector2 CONVERSATION_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

                            // CONVERSATION IMAGE
                            Image ConversationImage = {};
                            ConversationImage.TextureID = m_BlankImageTexture.GetID();
                            ConversationImage.Size = Vector2(CONVERSATION_CONTAINER_AVAILABLE_SPACE.Y, CONVERSATION_CONTAINER_AVAILABLE_SPACE.Y);
                            ConversationImage.CornerRounding = 10.0f;
                            m_Gui.DrawImage(ConversationImage);

                            // CONVERSATION TEXT
                            Text ConversationText = {};
                            ConversationText.Value = Conversation->Users[1].FirstName;

                            m_Gui.SetPositionX(ConversationImage.Size.X + 10.0f);
                            m_Gui.DrawText(ConversationText);

                            // CLOSE CONVERSATION IMAGE BUTTON CONTAINER
                            if (!State.IsHovered) return;

                            Container CloseConversationImageButtonContainer = {};
                            CloseConversationImageButtonContainer.ID = "CloseConversationImageButtonContainer" + Conversation->ID;
                            CloseConversationImageButtonContainer.Size = ConversationImage.Size;
                            CloseConversationImageButtonContainer.Padding = Vector2(5.0f, 5.0f);
                            // NOTE: Transparent background
                            CloseConversationImageButtonContainer.BgColor = Rgba(0, 0, 0, 0);
                            CloseConversationImageButtonContainer.DrawContent = [this, &Conversation, Index](const ContainerState& State) {
                                // CLOSE CONVERSATION IMAGE BUTTON
                                Image CloseConversationImageButtonImage = {};
                                CloseConversationImageButtonImage.TextureID = m_ClosableImageTexture.GetID();
                                CloseConversationImageButtonImage.Size = m_Gui.GetAvailableSpace();
                                CloseConversationImageButtonImage.TintColor = Rgba(255, 255, 255, 255);
                                CloseConversationImageButtonImage.CornerRounding = 0.0f;

                                ImageButton CloseConversationImageButton = {};
                                CloseConversationImageButton.ID = "CloseConversationImageButton" + Conversation->ID;
                                CloseConversationImageButton.Image = CloseConversationImageButtonImage;
                                CloseConversationImageButton.TintColorHovered = Rgba(200, 200, 0, 255);
                                CloseConversationImageButton.OnClick = [this, Index]() {
                                    const std::string& ID = m_Conversations[Index]->ID;

                                    // Deletes conversation
                                    m_Conversations.erase(m_Conversations.begin() + Index);
                                    // Selects first conversation if deleted conversation is the selected one
                                    if (m_SelectedConversation->ID == ID) m_SelectedConversation = m_Conversations[0];

                                    m_Logger->Info("DELETED CONVERSATION ID: " + ID);
                                };

                                m_Gui.DrawImageButton(CloseConversationImageButton);
                            };

                            m_Gui.DisplayInline();
                            m_Gui.SetPositionX(CONVERSATION_CONTAINER_AVAILABLE_SPACE.X - CloseConversationImageButtonContainer.Size.X);
                            m_Gui.DrawContainer(CloseConversationImageButtonContainer);
                        };

                        m_Gui.DrawContainer(ConversationContainer);
                    }
                };

                m_Gui.DrawNode(ConversationsNode);
            };

            m_Gui.DrawContainer(ConversationsContainer);
        };

        m_Gui.DrawContainer(ChatsContainer);

        // SELECTED CONVERSATION CONTAINER
        Container SelectedConversationContainer = {};
        SelectedConversationContainer.ID = "SelectedConversationContainer";
        SelectedConversationContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.75f, MAIN_WINDOW_AVAILABLE_SPACE.Y * 0.70f);
        SelectedConversationContainer.Padding = Vector2(15.0f, 15.0f);
        // NOTE: Transparent background
        SelectedConversationContainer.BgColor = Rgba(0, 0, 0, 0);
        SelectedConversationContainer.DrawContent = [this](const ContainerState& State) {
            const Vector2 SELECTED_CONVERSATION_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

            // MESSSAGES CONTAINER
            Container MessagesContainer = {};
            MessagesContainer.ID = "MessagesContainer";
            MessagesContainer.Size = Vector2(SELECTED_CONVERSATION_CONTAINER_AVAILABLE_SPACE);
            MessagesContainer.CornerRounding = 10.f;
            MessagesContainer.BgColor = Rgba(50, 56, 102, 255);
            MessagesContainer.DrawContent = [this](const ContainerState& State) {
                const Vector2 MESSAGES_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

                for (int Index = 0; Index < m_SelectedConversation->Messages.size(); Index++)
                {
                    // MESSAGE CONTAINER
                    const Message& MESSAGE = m_SelectedConversation->Messages[Index];
                    const std::string& ID = "MessageContainer" + std::to_string(Index);

                    Container MessageContainer = {};
                    MessageContainer.ID = ID;
                    MessageContainer.Size = Vector2(MESSAGES_CONTAINER_AVAILABLE_SPACE.X, 0.0f);
                    MessageContainer.Padding = Vector2(10.0f, 10.0f);
                    // NOTE: Transparent background
                    MessageContainer.BgColor = Rgba(0, 0, 0, 0);
                    MessageContainer.IsAutoResizableY = true;
                    MessageContainer.DrawContent = [this, &MESSAGE](const ContainerState& State) {
                        const Vector2 MESSAGE_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

                        // MESSAGE SENDER IMAGE
                        // NOTE: Images are drawn directly over elements so anything that needs to go beside will have to be postioned manually
                        Image MessageSenderImage = {};
                        MessageSenderImage.TextureID = m_BlankImageTexture.GetID();
                        MessageSenderImage.Size = Vector2(MESSAGE_CONTAINER_AVAILABLE_SPACE.X * 0.05f, MESSAGE_CONTAINER_AVAILABLE_SPACE.X * 0.05f);
                        MessageSenderImage.CornerRounding = 10.0f;
                        m_Gui.DrawImage(MessageSenderImage);

                        // MESSAGE DETAILS CONTAINER
                        Container MessageDetailsContainer = {};
                        MessageDetailsContainer.ID = "MessageDetailsContainer";
                        MessageDetailsContainer.Size = Vector2(MESSAGE_CONTAINER_AVAILABLE_SPACE.X - MessageSenderImage.Size.X, 0.0f);
                        MessageDetailsContainer.Padding = Vector2(m_Gui.GetParentContainerPaddingSize().X / 1.5f, 0.0f);
                        // NOTE: Transparent background
                        MessageDetailsContainer.BgColor = Rgba(0, 0, 0, 0);
                        MessageDetailsContainer.IsAutoResizableY = true;
                        MessageDetailsContainer.DrawContent = [this, &MESSAGE](const ContainerState& State) {
                            // MESSAGE SENDER FIRSTNAME TEXT
                            Text MessageSenderFirstNameText = {};
                            MessageSenderFirstNameText.Value = MESSAGE.SenderFirstName;
                            m_Gui.DrawText(MessageSenderFirstNameText);

                            // MESSAGE CREATED AT TEXT
                            std::tm* MessageCreatedAtDate = std::localtime(&MESSAGE.CreatedAt);
                            const std::string& MESSAGE_CREATED_AT_STRING_DATE = asctime(MessageCreatedAtDate);

                            Text MessageCreatedAtText = {};
                            MessageCreatedAtText.Value = MESSAGE_CREATED_AT_STRING_DATE;
                            m_Gui.DisplayInline();
                            m_Gui.DrawText(MessageCreatedAtText);

                            // MESSAGE TEXT
                            Text MessageText = {};
                            MessageText.Value = MESSAGE.Text;
                            m_Gui.DrawTextWrapped(MessageText);

                        };

                        m_Gui.SetPositionX(MessageSenderImage.Size.X + 10.0f);
                        m_Gui.DrawContainer(MessageDetailsContainer);

                    };

                    m_Gui.DrawContainer(MessageContainer);
                }

                // Before drawing content, check if we are already at the bottom
                const bool IsAtBottom = m_Gui.GetScrollPositionY() >= m_Gui.GetMaxScrollPositionY();
                // Auto-scroll logic: only scroll if the user hasn't moved away from the bottom
                if (IsAtBottom)
                {
                    // Scrolls to the end
                    m_Gui.ScrollToY(1.0f);
                }
            };

            m_Gui.DrawContainer(MessagesContainer);
        };

        m_Gui.DisplayInline();
        m_Gui.SetPositionX(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.25f);
        m_Gui.DrawContainer(SelectedConversationContainer);

        // MESSAGE TEXT INPUT CONTAINER
        Container MessageTextInputContainer = {};
        MessageTextInputContainer.ID = "TextInputContainer";
        MessageTextInputContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.60f, MAIN_WINDOW_AVAILABLE_SPACE.Y * 0.15f);
        MessageTextInputContainer.Padding = Vector2(15.0f, 15.0f);
        // NOTE: Transparent background
        MessageTextInputContainer.BgColor = Rgba(0, 0, 0, 0);
        MessageTextInputContainer.DrawContent = [this](const ContainerState& State) {
            const Vector2 MESSAGE_TEXTINPUT_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

            // MESSAGE TEXT INPUT
            Placeholder MessageTextInputPlaceholder = {};
            MessageTextInputPlaceholder.Color = Rgba(120, 125, 172, 255);
            if (!m_Gui.AreAnyModalsOpen()) MessageTextInputPlaceholder.Text = "Enter message here...";

            TextInput MessageTextInput = {};
            MessageTextInput.ID = "MessageTextInput";
            MessageTextInput.Padding = Vector2(15.0f, 15.0f);
            MessageTextInput.CornerRounding = 10.f;
            MessageTextInput.BgColor = Rgba(43, 50, 94, 255);
            MessageTextInput.Placeholder = MessageTextInputPlaceholder;

            TextInputMultiline MessageTextInputMultiline = {};
            MessageTextInputMultiline.TextInput = MessageTextInput;
            MessageTextInputMultiline.Size = Vector2(MESSAGE_TEXTINPUT_CONTAINER_AVAILABLE_SPACE);

            m_Gui.DrawTextInputMultiline(m_MessageValue, MessageTextInputMultiline);
        };

        m_Gui.SetPositionX(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.25f);
        m_Gui.SetPositionY(MAIN_WINDOW_AVAILABLE_SPACE.Y * 0.85f);
        m_Gui.DrawContainer(MessageTextInputContainer);

        // SEND BUTTON CONTAINER
        Container SendButtonContainer = {};
        SendButtonContainer.ID = "SendButtonContainer";
        SendButtonContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.15f, MAIN_WINDOW_AVAILABLE_SPACE.Y * 0.15f);
        SendButtonContainer.Padding = Vector2(15.0f, 15.0f);
        // NOTE: Transparent background
        SendButtonContainer.BgColor = Rgba(0, 0, 0, 0);
        SendButtonContainer.DrawContent = [this](const ContainerState& State) {
            const Vector2 SEND_BUTTON_CONTAINER_AVAILABLE_SPACE = m_Gui.GetAvailableSpace();

            // SEND BUTTON
            Button SendButton = {};
            SendButton.Label = "Send";
            SendButton.Size = Vector2(SEND_BUTTON_CONTAINER_AVAILABLE_SPACE.X * 0.70f, SEND_BUTTON_CONTAINER_AVAILABLE_SPACE.Y * 0.40f);
            SendButton.BgColor = Rgba(200, 30, 30, 255); // Red button
            SendButton.BgColorActive = Rgba(150, 0, 0, 255); // Darker red when active
            SendButton.BgColorHovered = Rgba(255, 100, 100, 255); // Lighter red on hover
            SendButton.CornerRounding = 10.0f;
            SendButton.IsDisabled = m_MessageValue.empty();
            SendButton.OnClick = [this]() {
                Message NewMessage = {};
                NewMessage.ID = "FakeID";
                NewMessage.ConversationID = m_SelectedConversation->ID;
                NewMessage.SenderID = m_CurrentUser->ID;
                NewMessage.SenderFirstName = m_CurrentUser->FirstName;
                NewMessage.SenderImageUrl = m_CurrentUser->ImageUrl;
                NewMessage.Text = m_MessageValue;
                NewMessage.CreatedAt = std::time(0);

                // TODO: Server call to persist message will go there

                m_SelectedConversation->Messages.push_back(NewMessage);

                m_Logger->Info("SENT: " + m_MessageValue);
            };

            m_Gui.AlignCenter(SendButton.Size);
            m_Gui.DrawButton(SendButton);
        };

        m_Gui.DisplayInline();
        m_Gui.SetPositionX(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.85f);
        m_Gui.DrawContainer(SendButtonContainer);
    };

    m_Gui.DrawWindow(ChatWindow);
}

// ***********
// * PRIVATE *
// ***********
void ChatLayer::Reset()
{
    // Settings
    m_IsSettingsDropDownMenuOpen = false;

    // Search
    m_SearchValue.clear();

    // Messages
    m_MessageValue.clear();

    // Users
    m_CurrentUser = nullptr;
    m_Users.clear();

    // Conversations
    /**
     * NOTES:
     *  - Clearing m_Conversations will also reset m_SelectedConversation which is a shared pointer to one of the conversations in m_Conversations
     *  - Resetting m_SelectedConversation manually before clearing m_Conversations would mke the app crash as clearing would try to release an
     *    already released resource
     */
    m_Conversations.clear();

    // Textures
    m_BlankImageTexture.Destroy();
    m_ClosableImageTexture.Destroy();
    m_WhiteLeftArrowImageTexture.Destroy();
}
