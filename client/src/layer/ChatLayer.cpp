#include "layer/ChatLayer.h"

#include "auth/AuthApi.h"
#include "deserializer/MessageCreatedSocketEventPayloadDeserializer.h"
#include "deserializer/UserAuthenticatedSocketEventPayloadDeserializer.h"
#include "log/Logger.h"
#include "navigation/Navigation.h"
#include "messages/MessagesApi.h"

constexpr int SERVER_PORT =  5000;

// **********
// * PUBLIC *
// **********

ChatLayer::ChatLayer(const std::string& id, const Gui& gui, Navigation& navigation, AuthApi& authApi, MessagesApi& messagesApi, Logger& logger)
    : Layer(id, logger)
    , m_gui(gui)
    , m_navigation(navigation)
    , m_authApi(authApi)
    , m_messagesApi(messagesApi)
    , m_userAuthenticatedObserver(*this, &ChatLayer::HandleUserAuthenticated)
    , m_messageCreatedObserver(*this, &ChatLayer::HandleMessageCreated)
{}

void ChatLayer::OnAttach()
{
    m_authApi.GetUserAuthenticatedSubject().Subscribe(&m_userAuthenticatedObserver);
    m_messagesApi.GetMessageCreatedSubject().Subscribe(&m_messageCreatedObserver);

    // Users
    User CurrentUser = {};
    CurrentUser.id = "CurrentUser";
    CurrentUser.email = "currentuser@fake.com";
    CurrentUser.firstName = "Olivier";
    CurrentUser.lastName = "Perron";
    CurrentUser.password = "passwordCurrentUser";

    m_currentUser = std::make_shared<User>(CurrentUser);

    User User1 = {};
    User1.id = "User1";
    User1.email = "user1@fake.com";
    User1.firstName = "Marc";
    User1.lastName = "Bum";
    User1.password = "passwordUser1";

    User User2 = {};
    User2.id = "User2";
    User1.email = "user2@fake.com";
    User2.firstName = "Simon";
    User2.lastName = "Robichaud";
    User2.password = "passwordUser2";

    m_users = {
        std::make_shared<User>(User1),
        std::make_shared<User>(User2),
    };

    m_usersMap[User1.id] = User1;
    m_usersMap[User2.id] = User2;

    ConversationUser currentConversationUser = {};
    currentConversationUser.userId = m_currentUser->id;
    currentConversationUser.isOpen = false;

    ConversationUser conversationUser1 = {};
    conversationUser1.userId = User1.id;
    conversationUser1.isOpen = false;

    ConversationUser conversationUser2 = {};
    conversationUser1.userId = User2.id;
    conversationUser1.isOpen = false;

    // Conversations
    Conversation Conversation1 = {};
    Conversation1.id = "Conversation1";
    Conversation1.users = { currentConversationUser, conversationUser1 };
    Conversation1.createdAt = std::time(0);

    Conversation Conversation2 = {};
    Conversation2.id = "Conversation2";
    Conversation2.users = { currentConversationUser, conversationUser2 };
    Conversation2.createdAt = std::time(0);

    m_conversations = {
        std::make_shared<Conversation>(Conversation1),
        std::make_shared<Conversation>(Conversation2)
    };

    m_selectedConversation = m_conversations[0];

    m_conversationMessagesMap[Conversation1.id] = {};
    m_conversationMessagesMap[Conversation2.id] = {};

    // Textures
    m_blankImageTexture.Load("../../assets/Blank.jpg", 0);
    m_blankImageTexture.Bind();

    m_closableImageTexture.Load("../../assets/Closable.png", 1);
    m_closableImageTexture.Bind();

    m_whiteLeftArrowImageTexture.Load("../../assets/WhiteLeftArrow.png", 2);
    m_whiteLeftArrowImageTexture.Bind();
}

void ChatLayer::OnDetach()
{
    m_authApi.GetUserAuthenticatedSubject().Unsubscribe(&m_userAuthenticatedObserver);
    m_messagesApi.GetMessageCreatedSubject().Unsubscribe(&m_messageCreatedObserver);
    Reset();
}

void ChatLayer::OnRender()
{
    // WINDOW
    Window ChatWindow = {};
    ChatWindow.Name = "ChatWindow";
    ChatWindow.Size = m_gui.GetViewportSize();
    ChatWindow.BgColor = Rgba(26, 30, 67, 255);
    ChatWindow.DrawContent = [this]() {
        const Vector2 MAIN_WINDOW_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

        // NAVBAR CONTAINER
        Container NavbarContainer = {};
        NavbarContainer.ID = "NavbarContainer";
        NavbarContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X, MAIN_WINDOW_AVAILABLE_SPACE.Y * 0.15f);
        NavbarContainer.Padding = Vector2(15.0f, 15.0f);
        // NOTE: Transparent background
        NavbarContainer.BgColor = Rgba(0, 0, 0, 0);
        NavbarContainer.DrawContent = [this](const ContainerState& State) {
            const Vector2 NAVBAR_CONTAINER_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

            // NAVBAR
            Container Navbar = {};
            Navbar.ID = "Navbar";
            Navbar.Size = Vector2(NAVBAR_CONTAINER_AVAILABLE_SPACE);
            Navbar.CornerRounding = 10.f;
            Navbar.BgColor = Rgba(50, 56, 102, 255);
            Navbar.DrawContent = [this](const ContainerState& State) {
                const Vector2 NAVBAR_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

                // SEARCH TEXT INPUT CONTAINER
                Container SearchTextInputContainer = {};
                SearchTextInputContainer.ID = "SearchTextInputContainer";
                SearchTextInputContainer.Size = Vector2(NAVBAR_AVAILABLE_SPACE.X * 0.50f, NAVBAR_AVAILABLE_SPACE.Y);
                SearchTextInputContainer.CornerRounding = 10.0f;
                // NOTE: Transparent background
                SearchTextInputContainer.BgColor = Rgba(0, 0, 0, 0);
                SearchTextInputContainer.DrawContent = [this](const ContainerState& State) {
                    const Vector2 SEARCH_TEXTINPUT_CONTAINER_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

                    // SEARCH MODAL
                    Modal SearchModal = {};
                    SearchModal.ID = "SearchModal";
                    SearchModal.Size = m_gui.GetViewportSize();

                    Container SearchModalHeaderContainer = {};
                    SearchModalHeaderContainer.ID = "SearchModalHeaderContainer";
                    SearchModalHeaderContainer.Size = Vector2(SearchModal.Size.X , 0.0f);
                    // NOTE: Transparent background
                    SearchModalHeaderContainer.BgColor = Rgba(0, 0, 0, 0);
                    SearchModalHeaderContainer.CornerRounding = SearchModal.CornerRounding;
                    SearchModalHeaderContainer.IsAutoResizableY = true;
                    SearchModalHeaderContainer.DrawContent = [this, &SearchModal](const ContainerState& State) {
                        const Vector2 SEARCH_MODAL_HEADER_CONTAINER_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

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
                            SearchModalCloseImageButtonImage.TextureID = m_whiteLeftArrowImageTexture.GetID();
                            SearchModalCloseImageButtonImage.Size = m_gui.GetAvailableSpace();
                            SearchModalCloseImageButtonImage.TintColor = Rgba(255, 255, 255, 255);

                            ImageButton SearchModalCloseImageButton = {};
                            SearchModalCloseImageButton.ID = "SearchModalCloseImageButton";
                            SearchModalCloseImageButton.Image = SearchModalCloseImageButtonImage;
                            SearchModalCloseImageButton.TintColorHovered = Rgba(200, 200, 0, 255);
                            SearchModalCloseImageButton.OnClick = [this, &SearchModal]() {
                                m_gui.CloseModal(SearchModal.ID);
                            };

                            m_gui.DrawImageButton(SearchModalCloseImageButton);
                        };

                        m_gui.DrawContainer(SearchModalCloseImageButtonContainer);

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

                        m_gui.DisplayInline();
                        m_gui.SetPositionX(SearchModalCloseImageButtonContainer.Size.X);
                        m_gui.DrawTextInputSingleline(m_searchValue, SearchModalTextInputSingleline);

                        Divider BottomDivider = {};
                        BottomDivider.Color = Rgba(100, 100, 100, 255);

                        m_gui.SetPositionY(SEARCH_MODAL_HEADER_CONTAINER_AVAILABLE_SPACE.Y - BottomDivider.Height);
                        m_gui.DrawDivider(BottomDivider);

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
                        if (m_searchValue.empty()) return;

                        const Vector2 SEARCH_MODAL_BODY_CONTAINER_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

                        std::function<std::vector<std::shared_ptr<User>>(std::string&)> Search = [this](std::string& Value) {
                            // Defines a lambda function as the predicate for filtering
                            std::function<bool(std::shared_ptr<User>)> IsFound = [&Value](std::shared_ptr<User> User) {
                                // Defines a lambda function for converting a char to a lowercase char
                                std::function<int(unsigned char)> ConvertCharToLowercase = [&Value](unsigned char Character){
                                    return std::tolower(Character);
                                };

                                std::string& LowercaseFirstName = User->firstName;
                                std::transform(LowercaseFirstName.begin(), LowercaseFirstName.end(), LowercaseFirstName.begin(), ConvertCharToLowercase);

                                std::string& LowercaseLastName = User->lastName;
                                std::transform(LowercaseLastName.begin(), LowercaseLastName.end(), LowercaseLastName.begin(), ConvertCharToLowercase);

                                // std::string& LowercaseValue = m_searchValue;
                                std::string& LowercaseValue = Value;
                                std::transform(LowercaseValue.begin(), LowercaseValue.end(), LowercaseValue.begin(), ConvertCharToLowercase);

                                return (
                                    LowercaseFirstName.find(LowercaseValue) != std::string::npos ||
                                    LowercaseLastName.find(LowercaseValue) != std::string::npos
                                );
                            };

                            // Copies elements that satisfy the predicate into the new vector
                            std::vector<std::shared_ptr<User>> FilteredUsers = {};
                            std::copy_if(m_users.begin(), m_users.end(), std::back_inserter(FilteredUsers), IsFound);

                            return std::move(FilteredUsers);
                        };

                        std::vector<std::shared_ptr<User>> Users = Search(m_searchValue);
                        if (Users.empty()) return;

                        for (int Index = 0; Index < Users.size(); Index++)
                        {
                            std::shared_ptr<User> User = Users[Index];

                            // USER CONTAINER
                            Container UserContainer = {};
                            UserContainer.ID = User->id;
                            UserContainer.Size = Vector2(SEARCH_MODAL_BODY_CONTAINER_AVAILABLE_SPACE.X, 40.0f);
                            UserContainer.CornerRounding = 10.f;
                            // NOTE: Transparent background
                            UserContainer.BgColor = Rgba(0, 0, 0, 0);
                            UserContainer.BgColorHovered = Rgba(50, 56, 102, 255);
                            UserContainer.IsAutoResizableY = true;
                            UserContainer.OnClick = [this, &SearchModal, &User]() {
                                m_logger.Info("selected User with ID " + User->id);
                                m_gui.CloseModal(SearchModal.ID);
                                m_searchValue = "";
                            };
                            UserContainer.DrawContent = [this, &User](const ContainerState& State) {
                                const Vector2 USER_CONTAINER_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

                                // USER IMAGE
                                Image UserImage = {};
                                UserImage.TextureID = m_blankImageTexture.GetID();
                                UserImage.Size = Vector2(USER_CONTAINER_AVAILABLE_SPACE.Y, USER_CONTAINER_AVAILABLE_SPACE.Y);
                                UserImage.CornerRounding = 10.0f;

                                m_gui.DrawImage(UserImage);

                                // USER TEXT
                                Text UserText = {};
                                UserText.Value = User->firstName + " " + User->lastName;

                                m_gui.DisplayInline();
                                m_gui.SetPositionX(UserImage.Size.X * 1.3f);
                                Vector2 TextSize = m_gui.GetTextSize(UserText.Value);
                                m_gui.AlignCenterY(TextSize.Y);
                                m_gui.DrawText(UserText);
                            };

                            m_gui.DrawContainer(UserContainer);
                        }
                    };

                    SearchModal.HeaderContainer = SearchModalHeaderContainer;
                    SearchModal.BodyContainer = SearchModalBodyContainer;

                    m_gui.DrawModal(SearchModal);

                    // SEARCH TEXT INPUT
                    Placeholder SearchTextInputPlaceholder = {};
                    SearchTextInputPlaceholder.Color = Rgba(120, 125, 172, 255);
                    if (!m_gui.AreAnyModalsOpen()) SearchTextInputPlaceholder.Text = "Enter search here...";

                    TextInput SearchTextInput = {};
                    SearchTextInput.ID = "SearchTextInput";
                    SearchTextInput.Padding = Vector2(15.0f, 15.0f);
                    SearchTextInput.CornerRounding = 10.f;
                    SearchTextInput.BgColor = Rgba(26, 30, 67, 255);
                    SearchTextInput.Placeholder = SearchTextInputPlaceholder;
                    SearchTextInput.OnClick = [this, &SearchModal]() {
                        m_gui.OpenModal(SearchModal.ID);
                    };

                    TextInputSingleline SearchTextInputSingleline = {};
                    SearchTextInputSingleline.TextInput = SearchTextInput;
                    SearchTextInputSingleline.Width = SEARCH_TEXTINPUT_CONTAINER_AVAILABLE_SPACE.X * 0.80f;

                    // NOTE: SearchTextInputSingleline is only a trigger to show SearchModal
                    std::string EmptyValue = "";
                    float SearchTextInputSinglelineHeight = m_gui.GetTextInputSinglelineHeight();
                    m_gui.AlignCenter(Vector2(SearchTextInputSingleline.Width, SearchTextInputSinglelineHeight));
                    m_gui.DrawTextInputSingleline(EmptyValue, SearchTextInputSingleline);
                };

                m_gui.DrawContainer(SearchTextInputContainer);

                // SETTINGS CONTAINER
                Container SettingsContainer = {};
                SettingsContainer.ID = "SettingsContainer";
                SettingsContainer.Size = Vector2(NAVBAR_AVAILABLE_SPACE.X * 0.50f, NAVBAR_AVAILABLE_SPACE.Y);
                SettingsContainer.CornerRounding = 10.0f;
                // NOTE: Transparent background
                SettingsContainer.BgColor = Rgba(0, 0, 0, 0);
                SettingsContainer.DrawContent = [this](const ContainerState& State) {
                    const Vector2 SETTINGS_CONTAINER_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

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
                        CurrentUserImage.TextureID = m_blankImageTexture.GetID();
                        CurrentUserImage.Size = m_gui.GetAvailableSpace();
                        CurrentUserImage.TintColor = Rgba(255, 255, 255, 255);
                        CurrentUserImage.CornerRounding = 10.0f;

                        ImageButton CurrentUserImageButton = {};
                        CurrentUserImageButton.ID = "CurrentUserImageButton";
                        CurrentUserImageButton.Image = CurrentUserImage;
                        CurrentUserImageButton.TintColorHovered = Rgba(200, 200, 0, 255);
                        CurrentUserImageButton.OnClick = [this]() {
                            m_isSettingsDropDownMenuOpen = !m_isSettingsDropDownMenuOpen;
                        };

                        m_gui.DrawImageButton(CurrentUserImageButton);

                        // Closes setting drop down menu if a modal is open
                        if (m_gui.AreAnyModalsOpen()) m_isSettingsDropDownMenuOpen = false;
                        if (!m_isSettingsDropDownMenuOpen) return;

                        // SETTINGS DROP DOWN MENU
                        DropDownMenuItem ProfileDropDownMenuItem = {};
                        ProfileDropDownMenuItem.Text = "Profile";
                        ProfileDropDownMenuItem.TextColor = Rgba(255, 255, 255, 255);
                        ProfileDropDownMenuItem.BgColorHovered = Rgba(50, 56, 102, 255);
                        ProfileDropDownMenuItem.OnClick = [this]() {
                            m_logger.Info("Profile clicked!");
                        };

                        DropDownMenuItem PreferencesDropDownMenuItem = {};
                        PreferencesDropDownMenuItem.Text = "Preferences";
                        PreferencesDropDownMenuItem.TextColor = Rgba(255, 255, 255, 255);
                        PreferencesDropDownMenuItem.BgColorHovered = Rgba(50, 56, 102, 255);
                        PreferencesDropDownMenuItem.OnClick = [this]() {
                            m_logger.Info("Preferences clicked!");
                        };

                        DropDownMenuItem LogoutDropDownMenuItem = {};
                        LogoutDropDownMenuItem.Text = "Logout";
                        LogoutDropDownMenuItem.TextColor = Rgba(255, 255, 255, 255);
                        LogoutDropDownMenuItem.BgColorHovered = Rgba(50, 56, 102, 255);
                        LogoutDropDownMenuItem.OnClick = [this]() {
                            HandleLogoutButtonClicked();
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
                            Vector2 TextSize = m_gui.GetTextSize(DropDownMenuItem->Text);

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

                        m_gui.DrawDropDownMenu(SettingsDropDownMenu);

                    };

                    m_gui.SetPositionX(SETTINGS_CONTAINER_AVAILABLE_SPACE.X - CurrentUserImageContainer.Size.X);
                    m_gui.DrawContainer(CurrentUserImageContainer);
                };

                m_gui.DisplayInline();
                m_gui.SetPositionX(SearchTextInputContainer.Size.X);
                m_gui.DrawContainer(SettingsContainer);
            };

            m_gui.DrawContainer(Navbar);
        };

        m_gui.DrawContainer(NavbarContainer);

        // CHATS CONTAINER
        Container ChatsContainer = {};
        ChatsContainer.ID = "ChatsContainer";
        ChatsContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.25f, MAIN_WINDOW_AVAILABLE_SPACE.Y - NavbarContainer.Size.Y);
        ChatsContainer.Padding = Vector2(15.0f, 15.0f);
        // NOTE: Transparent background
        ChatsContainer.BgColor = Rgba(0, 0, 0, 0);
        ChatsContainer.DrawContent = [this](const ContainerState& State) {
            const Vector2 CHATS_CONTAINER_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

            // CONVERSATIONS CONTAINER
            Container ConversationsContainer = {};
            ConversationsContainer.ID = "ConversationsContainer";
            ConversationsContainer.Size = Vector2(CHATS_CONTAINER_AVAILABLE_SPACE);
            ConversationsContainer.CornerRounding = 10.f;
            ConversationsContainer.BgColor = Rgba(50, 56, 102, 255);
            ConversationsContainer.DrawContent = [this](const ContainerState& State) {
                const Vector2 CONVERSATIONS_CONTAINER_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

                // CONVERSATIONS NODE
                Node ConversationsNode = {};
                ConversationsNode.Name = "Conversations";
                ConversationsNode.DrawContent = [this]() {
                    const Vector2 CONVERSATIONS_NODE_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

                    for (int Index = 0; Index < m_conversations.size(); Index++)
                    {
                        const std::shared_ptr<Conversation> Conversation = m_conversations[Index];

                        // CONVERSATION CONTAINER
                        Rgba BgColor = Rgba(50, 56, 102, 255);
                        if (Conversation->id == m_selectedConversation->id) BgColor = Rgba(100, 100, 100, 255);

                        Container ConversationContainer = {};
                        ConversationContainer.ID = "ConversationContainer" + Conversation->id;
                        ConversationContainer.Size = Vector2(CONVERSATIONS_NODE_AVAILABLE_SPACE.X, CONVERSATIONS_NODE_AVAILABLE_SPACE.Y * 0.05f);
                        ConversationContainer.CornerRounding = 10.f;
                        ConversationContainer.BgColor = BgColor;
                        ConversationContainer.BgColorHovered = Rgba(0, 0, 0, 255);
                        ConversationContainer.IsAutoResizableY = true;
                        ConversationContainer.OnClick = [this, &Conversation]() {
                            m_selectedConversation = Conversation;
                            m_logger.Info("SELECTED CONVERSATION ID: " + m_selectedConversation->id);
                        };
                        ConversationContainer.DrawContent = [this, &Conversation, Index](const ContainerState& State) {
                            const Vector2 CONVERSATION_CONTAINER_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

                            // CONVERSATION IMAGE
                            Image ConversationImage = {};
                            ConversationImage.TextureID = m_blankImageTexture.GetID();
                            ConversationImage.Size = Vector2(CONVERSATION_CONTAINER_AVAILABLE_SPACE.Y, CONVERSATION_CONTAINER_AVAILABLE_SPACE.Y);
                            ConversationImage.CornerRounding = 10.0f;
                            m_gui.DrawImage(ConversationImage);

                            // CONVERSATION TEXT
                            Text ConversationText = {};
                            ConversationText.Value = m_usersMap[Conversation->users[1].userId].firstName;

                            m_gui.SetPositionX(ConversationImage.Size.X + 10.0f);
                            m_gui.DrawText(ConversationText);

                            // CLOSE CONVERSATION IMAGE BUTTON CONTAINER
                            if (!State.IsHovered) return;

                            Container CloseConversationImageButtonContainer = {};
                            CloseConversationImageButtonContainer.ID = "CloseConversationImageButtonContainer" + Conversation->id;
                            CloseConversationImageButtonContainer.Size = ConversationImage.Size;
                            CloseConversationImageButtonContainer.Padding = Vector2(5.0f, 5.0f);
                            // NOTE: Transparent background
                            CloseConversationImageButtonContainer.BgColor = Rgba(0, 0, 0, 0);
                            CloseConversationImageButtonContainer.DrawContent = [this, &Conversation, Index](const ContainerState& State) {
                                // CLOSE CONVERSATION IMAGE BUTTON
                                Image CloseConversationImageButtonImage = {};
                                CloseConversationImageButtonImage.TextureID = m_closableImageTexture.GetID();
                                CloseConversationImageButtonImage.Size = m_gui.GetAvailableSpace();
                                CloseConversationImageButtonImage.TintColor = Rgba(255, 255, 255, 255);
                                CloseConversationImageButtonImage.CornerRounding = 0.0f;

                                ImageButton CloseConversationImageButton = {};
                                CloseConversationImageButton.ID = "CloseConversationImageButton" + Conversation->id;
                                CloseConversationImageButton.Image = CloseConversationImageButtonImage;
                                CloseConversationImageButton.TintColorHovered = Rgba(200, 200, 0, 255);
                                CloseConversationImageButton.OnClick = [this, Index]() {
                                    const std::string& ID = m_conversations[Index]->id;

                                    // Deletes conversation
                                    m_conversations.erase(m_conversations.begin() + Index);
                                    // Selects first conversation if deleted conversation is the selected one
                                    if (m_selectedConversation->id == ID) m_selectedConversation = m_conversations[0];

                                    m_logger.Info("DELETED CONVERSATION ID: " + ID);
                                };

                                m_gui.DrawImageButton(CloseConversationImageButton);
                            };

                            m_gui.DisplayInline();
                            m_gui.SetPositionX(CONVERSATION_CONTAINER_AVAILABLE_SPACE.X - CloseConversationImageButtonContainer.Size.X);
                            m_gui.DrawContainer(CloseConversationImageButtonContainer);
                        };

                        m_gui.DrawContainer(ConversationContainer);
                    }
                };

                m_gui.DrawNode(ConversationsNode);
            };

            m_gui.DrawContainer(ConversationsContainer);
        };

        m_gui.DrawContainer(ChatsContainer);

        // SELECTED CONVERSATION CONTAINER
        Container SelectedConversationContainer = {};
        SelectedConversationContainer.ID = "SelectedConversationContainer";
        SelectedConversationContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.75f, MAIN_WINDOW_AVAILABLE_SPACE.Y * 0.70f);
        SelectedConversationContainer.Padding = Vector2(15.0f, 15.0f);
        // NOTE: Transparent background
        SelectedConversationContainer.BgColor = Rgba(0, 0, 0, 0);
        SelectedConversationContainer.DrawContent = [this](const ContainerState& State) {
            const Vector2 SELECTED_CONVERSATION_CONTAINER_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

            // MESSSAGES CONTAINER
            Container MessagesContainer = {};
            MessagesContainer.ID = "MessagesContainer";
            MessagesContainer.Size = Vector2(SELECTED_CONVERSATION_CONTAINER_AVAILABLE_SPACE);
            MessagesContainer.CornerRounding = 10.f;
            MessagesContainer.BgColor = Rgba(50, 56, 102, 255);
            MessagesContainer.DrawContent = [this](const ContainerState& State) {
                const Vector2 MESSAGES_CONTAINER_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

                std::vector<Message> selectedConversationMessage = m_conversationMessagesMap[m_selectedConversation->id];
                for (int Index = 0; Index < selectedConversationMessage.size(); Index++)
                {
                    // MESSAGE CONTAINER

                    const Message& MESSAGE = selectedConversationMessage[Index];
                    const std::string& ID = "MessageContainer" + std::to_string(Index);

                    Container MessageContainer = {};
                    MessageContainer.ID = ID;
                    MessageContainer.Size = Vector2(MESSAGES_CONTAINER_AVAILABLE_SPACE.X, 0.0f);
                    MessageContainer.Padding = Vector2(10.0f, 10.0f);
                    // NOTE: Transparent background
                    MessageContainer.BgColor = Rgba(0, 0, 0, 0);
                    MessageContainer.IsAutoResizableY = true;
                    MessageContainer.DrawContent = [this, &MESSAGE](const ContainerState& State) {
                        const Vector2 MESSAGE_CONTAINER_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

                        // MESSAGE SENDER IMAGE
                        // NOTE: Images are drawn directly over elements so anything that needs to go beside will have to be postioned manually
                        Image MessageSenderImage = {};
                        MessageSenderImage.TextureID = m_blankImageTexture.GetID();
                        MessageSenderImage.Size = Vector2(MESSAGE_CONTAINER_AVAILABLE_SPACE.X * 0.05f, MESSAGE_CONTAINER_AVAILABLE_SPACE.X * 0.05f);
                        MessageSenderImage.CornerRounding = 10.0f;
                        m_gui.DrawImage(MessageSenderImage);

                        // MESSAGE DETAILS CONTAINER
                        Container MessageDetailsContainer = {};
                        MessageDetailsContainer.ID = "MessageDetailsContainer";
                        MessageDetailsContainer.Size = Vector2(MESSAGE_CONTAINER_AVAILABLE_SPACE.X - MessageSenderImage.Size.X, 0.0f);
                        MessageDetailsContainer.Padding = Vector2(m_gui.GetParentContainerPaddingSize().X / 1.5f, 0.0f);
                        // NOTE: Transparent background
                        MessageDetailsContainer.BgColor = Rgba(0, 0, 0, 0);
                        MessageDetailsContainer.IsAutoResizableY = true;
                        MessageDetailsContainer.DrawContent = [this, &MESSAGE](const ContainerState& State) {
                            // MESSAGE SENDER FIRSTNAME TEXT
                            Text MessageSenderFirstNameText = {};
                            MessageSenderFirstNameText.Value = "Firstname of " + MESSAGE.senderId;
                            m_gui.DrawText(MessageSenderFirstNameText);

                            // MESSAGE CREATED AT TEXT
                            std::tm* MessageCreatedAtDate = std::localtime(&MESSAGE.createdAt);
                            const std::string& MESSAGE_CREATED_AT_STRING_DATE = asctime(MessageCreatedAtDate);

                            Text MessageCreatedAtText = {};
                            MessageCreatedAtText.Value = MESSAGE_CREATED_AT_STRING_DATE;
                            m_gui.DisplayInline();
                            m_gui.DrawText(MessageCreatedAtText);

                            // MESSAGE TEXT
                            Text MessageText = {};
                            MessageText.Value = MESSAGE.text;
                            m_gui.DrawTextWrapped(MessageText);

                        };

                        m_gui.SetPositionX(MessageSenderImage.Size.X + 10.0f);
                        m_gui.DrawContainer(MessageDetailsContainer);

                    };

                    m_gui.DrawContainer(MessageContainer);
                }

                // Before drawing content, check if we are already at the bottom
                const bool IsAtBottom = m_gui.GetScrollPositionY() >= m_gui.GetMaxScrollPositionY();
                // Auto-scroll logic: only scroll if the user hasn't moved away from the bottom
                if (IsAtBottom)
                {
                    // Scrolls to the end
                    m_gui.ScrollToY(1.0f);
                }
            };

            m_gui.DrawContainer(MessagesContainer);
        };

        m_gui.DisplayInline();
        m_gui.SetPositionX(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.25f);
        m_gui.DrawContainer(SelectedConversationContainer);

        // MESSAGE TEXT INPUT CONTAINER
        Container MessageTextInputContainer = {};
        MessageTextInputContainer.ID = "TextInputContainer";
        MessageTextInputContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.60f, MAIN_WINDOW_AVAILABLE_SPACE.Y * 0.15f);
        MessageTextInputContainer.Padding = Vector2(15.0f, 15.0f);
        // NOTE: Transparent background
        MessageTextInputContainer.BgColor = Rgba(0, 0, 0, 0);
        MessageTextInputContainer.DrawContent = [this](const ContainerState& State) {
            const Vector2 MESSAGE_TEXTINPUT_CONTAINER_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

            // MESSAGE TEXT INPUT
            Placeholder MessageTextInputPlaceholder = {};
            MessageTextInputPlaceholder.Color = Rgba(120, 125, 172, 255);
            if (!m_gui.AreAnyModalsOpen()) MessageTextInputPlaceholder.Text = "Enter message here...";

            TextInput MessageTextInput = {};
            MessageTextInput.ID = "MessageTextInput";
            MessageTextInput.Padding = Vector2(15.0f, 15.0f);
            MessageTextInput.CornerRounding = 10.f;
            MessageTextInput.BgColor = Rgba(43, 50, 94, 255);
            MessageTextInput.Placeholder = MessageTextInputPlaceholder;

            TextInputMultiline MessageTextInputMultiline = {};
            MessageTextInputMultiline.TextInput = MessageTextInput;
            MessageTextInputMultiline.Size = Vector2(MESSAGE_TEXTINPUT_CONTAINER_AVAILABLE_SPACE);

            m_gui.DrawTextInputMultiline(m_messageValue, MessageTextInputMultiline);
        };

        m_gui.SetPositionX(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.25f);
        m_gui.SetPositionY(MAIN_WINDOW_AVAILABLE_SPACE.Y * 0.85f);
        m_gui.DrawContainer(MessageTextInputContainer);

        // SEND BUTTON CONTAINER
        Container SendButtonContainer = {};
        SendButtonContainer.ID = "SendButtonContainer";
        SendButtonContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.15f, MAIN_WINDOW_AVAILABLE_SPACE.Y * 0.15f);
        SendButtonContainer.Padding = Vector2(15.0f, 15.0f);
        // NOTE: Transparent background
        SendButtonContainer.BgColor = Rgba(0, 0, 0, 0);
        SendButtonContainer.DrawContent = [this](const ContainerState& State) {
            const Vector2 SEND_BUTTON_CONTAINER_AVAILABLE_SPACE = m_gui.GetAvailableSpace();

            // SEND BUTTON
            Button SendButton = {};
            SendButton.Label = "Send";
            SendButton.Size = Vector2(SEND_BUTTON_CONTAINER_AVAILABLE_SPACE.X * 0.70f, SEND_BUTTON_CONTAINER_AVAILABLE_SPACE.Y * 0.40f);
            SendButton.BgColor = Rgba(200, 30, 30, 255); // Red button
            SendButton.BgColorActive = Rgba(150, 0, 0, 255); // Darker red when active
            SendButton.BgColorHovered = Rgba(255, 100, 100, 255); // Lighter red on hover
            SendButton.CornerRounding = 10.0f;
            SendButton.IsDisabled = m_messageValue.empty();
            SendButton.OnClick = [this]() {
                Message newMessage = {};
                newMessage.id = "TempID";
                newMessage.conversationId = m_selectedConversation->id;
                newMessage.senderId = m_currentUser->id;
                newMessage.text = m_messageValue;
                newMessage.createdAt = std::time(0);

                std::vector<Message>& selectedConversationMessages = m_conversationMessagesMap[m_selectedConversation->id];
                selectedConversationMessages.push_back(newMessage);

                HandleSendMessageButtonClicked();
            };

            m_gui.AlignCenter(SendButton.Size);
            m_gui.DrawButton(SendButton);
        };

        m_gui.DisplayInline();
        m_gui.SetPositionX(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.85f);
        m_gui.DrawContainer(SendButtonContainer);
    };

    m_gui.DrawWindow(ChatWindow);
}

// ***********
// * PRIVATE *
// ***********
void ChatLayer::HandleMessageCreated(const MessageCreatedEvent& messageCreatedEvent)
{
    m_logger.Info("Message created id: " + messageCreatedEvent.message.id);
    m_logger.Info("Message created text: " + messageCreatedEvent.message.text);
}

void ChatLayer::HandleUserAuthenticated(const UserAuthenticatedEvent& userAuthenticatedEvent)
{
    // TODO: This function will be used to change the online status of a conversation user in an open conversation
    m_logger.Info("Authenticated user ID: " + userAuthenticatedEvent.user.id);
}

void ChatLayer::HandleLogoutButtonClicked()
{
    m_navigation.GoToLoginScreen();
}

void ChatLayer::HandleSendMessageButtonClicked()
{
    CreateMessageParams createMessageParams = {};
    createMessageParams.conversationId = m_selectedConversation->id;
    createMessageParams.text = m_messageValue;
    m_messagesApi.Create(createMessageParams);

    m_logger.Info("SENT: " + m_messageValue);
}

void ChatLayer::Reset()
{
    // Settings
    m_isSettingsDropDownMenuOpen = false;

    // Search
    m_searchValue.clear();

    // Messages
    m_messageValue.clear();

    // Users
    m_currentUser = nullptr;
    m_users.clear();

    // Conversations
    /**
     * NOTES:
     *  - Clearing m_conversations will also reset m_selectedConversation which is a shared pointer to one of the conversations in m_conversations
     *  - Resetting m_selectedConversation manually before clearing m_conversations would mke the app crash as clearing would try to release an
     *    already released resource
     */
    m_conversations.clear();

    // Textures
    m_blankImageTexture.Destroy();
    m_closableImageTexture.Destroy();
    m_whiteLeftArrowImageTexture.Destroy();
}
