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
    Gui::Window chatWindow = {};
    chatWindow.name = "chatWindow";
    chatWindow.size = m_gui.GetViewportSize();
    chatWindow.bgColor = Rgba(26, 30, 67, 255);
    chatWindow.DrawContent = [this]() {
        const Vector2 mainWindowAvailableSpace = m_gui.GetAvailableSpace();

        // NAVBAR CONTAINER
        Gui::Container navbarContainer = {};
        navbarContainer.id = "navbarContainer";
        navbarContainer.size = Vector2(mainWindowAvailableSpace.x, mainWindowAvailableSpace.y * 0.15f);
        navbarContainer.padding = Vector2(15.0f, 15.0f);
        // NOTE: Transparent background
        navbarContainer.bgColor = Rgba(0, 0, 0, 0);
        navbarContainer.DrawContent = [this](const Gui::ContainerState& state) {
            const Vector2 navbarContainerAvailableSpace = m_gui.GetAvailableSpace();

            // NAVBAR
            Gui::Container navbar = {};
            navbar.id = "navbar";
            navbar.size = Vector2(navbarContainerAvailableSpace);
            navbar.cornerRounding = 10.f;
            navbar.bgColor = Rgba(50, 56, 102, 255);
            navbar.DrawContent = [this](const Gui::ContainerState& state) {
                const Vector2 navbarAvailableSpace = m_gui.GetAvailableSpace();

                // SEARCH TEXT INPUT CONTAINER
                Gui::Container searchTextInputContainer = {};
                searchTextInputContainer.id = "searchTextInputContainer";
                searchTextInputContainer.size = Vector2(navbarAvailableSpace.x * 0.50f, navbarAvailableSpace.y);
                searchTextInputContainer.cornerRounding = 10.0f;
                // NOTE: Transparent background
                searchTextInputContainer.bgColor = Rgba(0, 0, 0, 0);
                searchTextInputContainer.DrawContent = [this](const Gui::ContainerState& state) {
                    const Vector2 searchTextInputContainerAvailableSpace = m_gui.GetAvailableSpace();

                    // SEARCH MODAL
                    Gui::Modal searchModal = {};
                    searchModal.id = "searchModal";
                    searchModal.size = m_gui.GetViewportSize();

                    Gui::Container searchModalHeaderContainer = {};
                    searchModalHeaderContainer.id = "searchModalHeaderContainer";
                    searchModalHeaderContainer.size = Vector2(searchModal.size.x , 0.0f);
                    // NOTE: Transparent background
                    searchModalHeaderContainer.bgColor = Rgba(0, 0, 0, 0);
                    searchModalHeaderContainer.cornerRounding = searchModal.cornerRounding;
                    searchModalHeaderContainer.isAutoResizableY = true;
                    searchModalHeaderContainer.DrawContent = [this, &searchModal](const Gui::ContainerState& state) {
                        const Vector2 searchModalHeaderContainerAvailableSpace = m_gui.GetAvailableSpace();

                        Gui::Container searchModalCloseImageButtonContainer = {};
                        searchModalCloseImageButtonContainer.id = "searchModalCloseImageButtonContainer";
                        searchModalCloseImageButtonContainer.size = Vector2(
                            searchModalHeaderContainerAvailableSpace.y ,
                            searchModalHeaderContainerAvailableSpace.y
                        );
                        searchModalCloseImageButtonContainer.padding = Vector2(10.0f, 10.0f);
                        // NOTE: Transparent background
                        searchModalCloseImageButtonContainer.bgColor = Rgba(0, 0, 0, 0);
                        searchModalCloseImageButtonContainer.DrawContent = [this, &searchModal](const Gui::ContainerState& state) {
                            Gui::Image SearchModalCloseImageButtonImage = {};
                            SearchModalCloseImageButtonImage.textureId = m_whiteLeftArrowImageTexture.GetID();
                            SearchModalCloseImageButtonImage.size = m_gui.GetAvailableSpace();
                            SearchModalCloseImageButtonImage.tintColor = Rgba(255, 255, 255, 255);

                            Gui::ImageButton searchModalCloseImageButton = {};
                            searchModalCloseImageButton.id = "searchModalCloseImageButton";
                            searchModalCloseImageButton.image = SearchModalCloseImageButtonImage;
                            searchModalCloseImageButton.tintColorHovered = Rgba(200, 200, 0, 255);
                            searchModalCloseImageButton.OnClick = [this, &searchModal]() {
                                m_gui.CloseModal(searchModal.id);
                            };

                            m_gui.DrawImageButton(searchModalCloseImageButton);
                        };

                        m_gui.DrawContainer(searchModalCloseImageButtonContainer);

                        Gui::Placeholder searchModalTextInputPlaceholder = {};
                        searchModalTextInputPlaceholder.text = "Enter search here...";

                        Gui::TextInput searchModalTextInput = {};
                        searchModalTextInput.id = "searchModalTextInput";
                        searchModalTextInput.padding = Vector2(15.0f, 15.0f);
                        searchModalTextInput.cornerRounding = 10.f;
                        searchModalTextInput.bgColor = Rgba(0, 0, 0, 0);
                        searchModalTextInput.placeholder  = searchModalTextInputPlaceholder;

                        Gui::TextInputSingleline searchModalTextInputSingleline = {};
                        searchModalTextInputSingleline.textInput = searchModalTextInput;
                        searchModalTextInputSingleline.width = searchModalHeaderContainerAvailableSpace.x - searchModalHeaderContainerAvailableSpace.y;

                        m_gui.DisplayInline();
                        m_gui.SetPositionX(searchModalCloseImageButtonContainer.size.x);
                        m_gui.DrawTextInputSingleline(m_searchValue, searchModalTextInputSingleline);

                        Gui::Divider bottomDivider = {};
                        bottomDivider.color = Rgba(100, 100, 100, 255);

                        m_gui.SetPositionY(searchModalHeaderContainerAvailableSpace.y - bottomDivider.height);
                        m_gui.DrawDivider(bottomDivider);

                    };

                    Gui::Container searchModalBodyContainer = {};
                    searchModalBodyContainer.id = "searchModalBodyContainer";
                    searchModalBodyContainer.size = Vector2(searchModal.size.x , 0.0f);
                    searchModalBodyContainer.padding = Vector2(15.0f, 15.0f);
                    // NOTE: Transparent background
                    searchModalBodyContainer.bgColor = Rgba(0, 0, 0, 0);
                    searchModalBodyContainer.cornerRounding = searchModal.cornerRounding;
                    searchModalBodyContainer.isAutoResizableY = true;
                    searchModalBodyContainer.DrawContent = [this, &searchModal](const Gui::ContainerState& state) {
                        if (m_searchValue.empty()) return;

                        const Vector2 searchModalBodyContainerAvailableSpace = m_gui.GetAvailableSpace();

                        std::function<std::vector<std::shared_ptr<User>>(std::string&)> search = [this](std::string& value) {
                            // Defines a lambda function as the predicate for filtering
                            std::function<bool(std::shared_ptr<User>)> isFound = [&value](std::shared_ptr<User> user) {
                                // Defines a lambda function for converting a char to a lowercase char
                                std::function<int(unsigned char)> convertCharToLowercase = [&value](unsigned char character){
                                    return std::tolower(character);
                                };

                                std::string& lowercaseFirstName = user->firstName;
                                std::transform(lowercaseFirstName.begin(), lowercaseFirstName.end(), lowercaseFirstName.begin(), convertCharToLowercase);

                                std::string& lowercaseLastName = user->lastName;
                                std::transform(lowercaseLastName.begin(), lowercaseLastName.end(), lowercaseLastName.begin(), convertCharToLowercase);

                                // std::string& lowercaseValue = m_searchValue;
                                std::string& lowercaseValue = value;
                                std::transform(lowercaseValue.begin(), lowercaseValue.end(), lowercaseValue.begin(), convertCharToLowercase);

                                return (
                                    lowercaseFirstName.find(lowercaseValue) != std::string::npos ||
                                    lowercaseLastName.find(lowercaseValue) != std::string::npos
                                );
                            };

                            // Copies elements that satisfy the predicate into the new vector
                            std::vector<std::shared_ptr<User>> filteredUsers = {};
                            std::copy_if(m_users.begin(), m_users.end(), std::back_inserter(filteredUsers), isFound);

                            return std::move(filteredUsers);
                        };

                        std::vector<std::shared_ptr<User>> users = search(m_searchValue);
                        if (users.empty()) return;

                        for (int index = 0; index < users.size(); index++)
                        {
                            std::shared_ptr<User> user = users[index];

                            // USER CONTAINER
                            Gui::Container userContainer = {};
                            userContainer.id = user->id;
                            userContainer.size = Vector2(searchModalBodyContainerAvailableSpace.x, 40.0f);
                            userContainer.cornerRounding = 10.f;
                            // NOTE: Transparent background
                            userContainer.bgColor = Rgba(0, 0, 0, 0);
                            userContainer.bgColorHovered = Rgba(50, 56, 102, 255);
                            userContainer.isAutoResizableY = true;
                            userContainer.OnClick = [this, &searchModal, &user]() {
                                m_logger.Info("selected User with id " + user->id);
                                m_gui.CloseModal(searchModal.id);
                                m_searchValue = "";
                            };
                            userContainer.DrawContent = [this, &user](const Gui::ContainerState& state) {
                                const Vector2 userContainerAvailableSpace = m_gui.GetAvailableSpace();

                                // USER IMAGE
                                Gui::Image userImage = {};
                                userImage.textureId = m_blankImageTexture.GetID();
                                userImage.size = Vector2(userContainerAvailableSpace.y, userContainerAvailableSpace.y);
                                userImage.cornerRounding = 10.0f;

                                m_gui.DrawImage(userImage);

                                // USER TEXT
                                Gui::Text userText = {};
                                userText.value = user->firstName + " " + user->lastName;

                                m_gui.DisplayInline();
                                m_gui.SetPositionX(userImage.size.x * 1.3f);
                                Vector2 textSize = m_gui.GetTextSize(userText.value);
                                m_gui.AlignCenterY(textSize.y);
                                m_gui.DrawText(userText);
                            };

                            m_gui.DrawContainer(userContainer);
                        }
                    };

                    searchModal.headerContainer = searchModalHeaderContainer;
                    searchModal.bodyContainer = searchModalBodyContainer;

                    m_gui.DrawModal(searchModal);

                    // SEARCH TEXT INPUT
                    Gui::Placeholder searchTextInputPlaceholder = {};
                    searchTextInputPlaceholder.color = Rgba(120, 125, 172, 255);
                    if (!m_gui.AreAnyModalsOpen()) searchTextInputPlaceholder.text = "Enter search here...";

                    Gui::TextInput searchTextInput = {};
                    searchTextInput.id = "searchTextInput";
                    searchTextInput.padding = Vector2(15.0f, 15.0f);
                    searchTextInput.cornerRounding = 10.f;
                    searchTextInput.bgColor = Rgba(26, 30, 67, 255);
                    searchTextInput.placeholder = searchTextInputPlaceholder;
                    searchTextInput.OnClick = [this, &searchModal]() {
                        m_gui.OpenModal(searchModal.id);
                    };

                    Gui::TextInputSingleline searchTextInputSingleline = {};
                    searchTextInputSingleline.textInput = searchTextInput;
                    searchTextInputSingleline.width = searchTextInputContainerAvailableSpace.x * 0.80f;

                    // NOTE: searchTextInputSingleline is only a trigger to show searchModal
                    std::string emptyValue = "";
                    float searchTextInputSinglelineHeight = m_gui.GetTextInputSinglelineHeight();
                    m_gui.AlignCenter(Vector2(searchTextInputSingleline.width, searchTextInputSinglelineHeight));
                    m_gui.DrawTextInputSingleline(emptyValue, searchTextInputSingleline);
                };

                m_gui.DrawContainer(searchTextInputContainer);

                // SETTINGS CONTAINER
                Gui::Container settingsContainer = {};
                settingsContainer.id = "settingsContainer";
                settingsContainer.size = Vector2(navbarAvailableSpace.x * 0.50f, navbarAvailableSpace.y);
                settingsContainer.cornerRounding = 10.0f;
                // NOTE: Transparent background
                settingsContainer.bgColor = Rgba(0, 0, 0, 0);
                settingsContainer.DrawContent = [this](const Gui::ContainerState& state) {
                    const Vector2 settingsContainerAvailableSpace = m_gui.GetAvailableSpace();

                    // CURRENT USER IMAGE CONTAINER
                    Gui::Container currentUserImageContainer = {};
                    currentUserImageContainer.id = "currentUserImageContainer";
                    currentUserImageContainer.size = Vector2(settingsContainerAvailableSpace.y, settingsContainerAvailableSpace.y);
                    currentUserImageContainer.cornerRounding = 10.f;
                    currentUserImageContainer.padding = Vector2(10.0f, 10.0f);
                    // NOTE: Transparent background
                    currentUserImageContainer.bgColor = Rgba(0, 0, 0, 0);
                    currentUserImageContainer.DrawContent = [this](const Gui::ContainerState& state) {
                        // CURRENT USER IMAGE BUTTON
                        Gui::Image currentUserImage = {};
                        currentUserImage.textureId = m_blankImageTexture.GetID();
                        currentUserImage.size = m_gui.GetAvailableSpace();
                        currentUserImage.tintColor = Rgba(255, 255, 255, 255);
                        currentUserImage.cornerRounding = 10.0f;

                        Gui::ImageButton currentUserImageButton = {};
                        currentUserImageButton.id = "currentUserImageButton";
                        currentUserImageButton.image = currentUserImage;
                        currentUserImageButton.tintColorHovered = Rgba(200, 200, 0, 255);
                        currentUserImageButton.OnClick = [this]() {
                            m_isSettingsDropDownMenuOpen = !m_isSettingsDropDownMenuOpen;
                        };

                        m_gui.DrawImageButton(currentUserImageButton);

                        // Closes setting drop down menu if a modal is open
                        if (m_gui.AreAnyModalsOpen()) m_isSettingsDropDownMenuOpen = false;
                        if (!m_isSettingsDropDownMenuOpen) return;

                        // SETTINGS DROP DOWN MENU
                        Gui::DropDownMenuItem profileDropDownMenuItem = {};
                        profileDropDownMenuItem.text = "Profile";
                        profileDropDownMenuItem.textColor = Rgba(255, 255, 255, 255);
                        profileDropDownMenuItem.bgColorHovered = Rgba(50, 56, 102, 255);
                        profileDropDownMenuItem.OnClick = [this]() {
                            m_logger.Info("Profile clicked!");
                        };

                        Gui::DropDownMenuItem preferencesDropDownMenuItem = {};
                        preferencesDropDownMenuItem.text = "Preferences";
                        preferencesDropDownMenuItem.textColor = Rgba(255, 255, 255, 255);
                        preferencesDropDownMenuItem.bgColorHovered = Rgba(50, 56, 102, 255);
                        preferencesDropDownMenuItem.OnClick = [this]() {
                            m_logger.Info("Preferences clicked!");
                        };

                        Gui::DropDownMenuItem logoutDropDownMenuItem = {};
                        logoutDropDownMenuItem.text = "Logout";
                        logoutDropDownMenuItem.textColor = Rgba(255, 255, 255, 255);
                        logoutDropDownMenuItem.bgColorHovered = Rgba(50, 56, 102, 255);
                        logoutDropDownMenuItem.OnClick = [this]() {
                            HandleLogoutButtonClicked();
                        };

                        std::vector<std::shared_ptr<Gui::DropDownMenuItem>> dropDownMenuItems = {
                            std::make_shared<Gui::DropDownMenuItem>(profileDropDownMenuItem),
                            std::make_shared<Gui::DropDownMenuItem>(preferencesDropDownMenuItem),
                            std::make_shared<Gui::DropDownMenuItem>(logoutDropDownMenuItem),
                        };

                        // Calculates drop down menu size
                        Vector2 dropDownMenuSize = Vector2(0.0f, 0.0f);
                        for (std::shared_ptr<Gui::DropDownMenuItem> dropDownMenuItem : dropDownMenuItems)
                        {
                            Vector2 textSize = m_gui.GetTextSize(dropDownMenuItem->text);

                            dropDownMenuSize.y += textSize.y;
                            dropDownMenuSize.x = std::max(dropDownMenuSize.x, textSize.x);
                        }

                        Gui::Border settingsDropDownMenuBorder = {};
                        settingsDropDownMenuBorder.color = Rgba(255, 255, 255, 255);
                        settingsDropDownMenuBorder.height = 2.0f;

                        Gui::DropDownMenu settingsDropDownMenu = {};
                        settingsDropDownMenu.border = settingsDropDownMenuBorder;
                        settingsDropDownMenu.size = dropDownMenuSize;
                        settingsDropDownMenu.originOffset = Vector2(-80.0f, 10.0f);
                        settingsDropDownMenu.outerPadding = Vector2(15.0f, 15.0f);
                        settingsDropDownMenu.bgColor = Rgba(26, 30, 67, 255);
                        settingsDropDownMenu.cornerRounding = 10.0f;
                        settingsDropDownMenu.lineHeight = 10.0f;
                        settingsDropDownMenu.items = dropDownMenuItems;

                        m_gui.DrawDropDownMenu(settingsDropDownMenu);

                    };

                    m_gui.SetPositionX(settingsContainerAvailableSpace.x - currentUserImageContainer.size.x);
                    m_gui.DrawContainer(currentUserImageContainer);
                };

                m_gui.DisplayInline();
                m_gui.SetPositionX(searchTextInputContainer.size.x);
                m_gui.DrawContainer(settingsContainer);
            };

            m_gui.DrawContainer(navbar);
        };

        m_gui.DrawContainer(navbarContainer);

        // CHATS CONTAINER
        Gui::Container chatsContainer = {};
        chatsContainer.id = "chatsContainer";
        chatsContainer.size = Vector2(mainWindowAvailableSpace.x * 0.25f, mainWindowAvailableSpace.y - navbarContainer.size.y);
        chatsContainer.padding = Vector2(15.0f, 15.0f);
        // NOTE: Transparent background
        chatsContainer.bgColor = Rgba(0, 0, 0, 0);
        chatsContainer.DrawContent = [this](const Gui::ContainerState& state) {
            const Vector2 chatsContainerAvailableSpace = m_gui.GetAvailableSpace();

            // CONVERSATIONS CONTAINER
            Gui::Container conversationsContainer = {};
            conversationsContainer.id = "conversationsContainer";
            conversationsContainer.size = Vector2(chatsContainerAvailableSpace);
            conversationsContainer.cornerRounding = 10.f;
            conversationsContainer.bgColor = Rgba(50, 56, 102, 255);
            conversationsContainer.DrawContent = [this](const Gui::ContainerState& state) {
                const Vector2 conversationsContainerAvailableSpace = m_gui.GetAvailableSpace();

                // CONVERSATIONS NODE
                Gui::Node conversationsNode = {};
                conversationsNode.name = "Conversations";
                conversationsNode.DrawContent = [this]() {
                    const Vector2 conversationsNodeAvailableSpace = m_gui.GetAvailableSpace();

                    for (int index = 0; index < m_conversations.size(); index++)
                    {
                        const std::shared_ptr<Conversation> conversation = m_conversations[index];

                        // CONVERSATION CONTAINER
                        Rgba bgColor = Rgba(50, 56, 102, 255);
                        if (conversation->id == m_selectedConversation->id) bgColor = Rgba(100, 100, 100, 255);

                        Gui::Container conversationContainer = {};
                        conversationContainer.id = "conversationContainer" + conversation->id;
                        conversationContainer.size = Vector2(conversationsNodeAvailableSpace.x, conversationsNodeAvailableSpace.y * 0.05f);
                        conversationContainer.cornerRounding = 10.f;
                        conversationContainer.bgColor = bgColor;
                        conversationContainer.bgColorHovered = Rgba(0, 0, 0, 255);
                        conversationContainer.isAutoResizableY = true;
                        conversationContainer.OnClick = [this, &conversation]() {
                            m_selectedConversation = conversation;
                            m_logger.Info("SELECTED CONVERSATION id: " + m_selectedConversation->id);
                        };
                        conversationContainer.DrawContent = [this, &conversation, index](const Gui::ContainerState& state) {
                            const Vector2 conversationContainerAvailableSpace = m_gui.GetAvailableSpace();

                            // CONVERSATION IMAGE
                            Gui::Image conversationImage = {};
                            conversationImage.textureId = m_blankImageTexture.GetID();
                            conversationImage.size = Vector2(conversationContainerAvailableSpace.y, conversationContainerAvailableSpace.y);
                            conversationImage.cornerRounding = 10.0f;
                            m_gui.DrawImage(conversationImage);

                            // CONVERSATION TEXT
                            Gui::Text conversationText = {};
                            conversationText.value = m_usersMap[conversation->users[1].userId].firstName;

                            m_gui.SetPositionX(conversationImage.size.x + 10.0f);
                            m_gui.DrawText(conversationText);

                            // CLOSE CONVERSATION IMAGE BUTTON CONTAINER
                            if (!state.isHovered) return;

                            Gui::Container closeConversationImageButtonContainer = {};
                            closeConversationImageButtonContainer.id = "closeConversationImageButtonContainer" + conversation->id;
                            closeConversationImageButtonContainer.size = conversationImage.size;
                            closeConversationImageButtonContainer.padding = Vector2(5.0f, 5.0f);
                            // NOTE: Transparent background
                            closeConversationImageButtonContainer.bgColor = Rgba(0, 0, 0, 0);
                            closeConversationImageButtonContainer.DrawContent = [this, &conversation, index](const Gui::ContainerState& state) {
                                // CLOSE CONVERSATION IMAGE BUTTON
                                Gui::Image closeConversationImageButtonImage = {};
                                closeConversationImageButtonImage.textureId = m_closableImageTexture.GetID();
                                closeConversationImageButtonImage.size = m_gui.GetAvailableSpace();
                                closeConversationImageButtonImage.tintColor = Rgba(255, 255, 255, 255);
                                closeConversationImageButtonImage.cornerRounding = 0.0f;

                                Gui::ImageButton closeConversationImageButton = {};
                                closeConversationImageButton.id = "closeConversationImageButton" + conversation->id;
                                closeConversationImageButton.image = closeConversationImageButtonImage;
                                closeConversationImageButton.tintColorHovered = Rgba(200, 200, 0, 255);
                                closeConversationImageButton.OnClick = [this, index]() {
                                    const std::string& id = m_conversations[index]->id;

                                    // Deletes conversation
                                    m_conversations.erase(m_conversations.begin() + index);
                                    // Selects first conversation if deleted conversation is the selected one
                                    if (m_selectedConversation->id == id) m_selectedConversation = m_conversations[0];

                                    m_logger.Info("DELETED CONVERSATION id: " + id);
                                };

                                m_gui.DrawImageButton(closeConversationImageButton);
                            };

                            m_gui.DisplayInline();
                            m_gui.SetPositionX(conversationContainerAvailableSpace.x - closeConversationImageButtonContainer.size.x);
                            m_gui.DrawContainer(closeConversationImageButtonContainer);
                        };

                        m_gui.DrawContainer(conversationContainer);
                    }
                };

                m_gui.DrawNode(conversationsNode);
            };

            m_gui.DrawContainer(conversationsContainer);
        };

        m_gui.DrawContainer(chatsContainer);

        // SELECTED CONVERSATION CONTAINER
        Gui::Container selectedConversationContainer = {};
        selectedConversationContainer.id = "selectedConversationContainer";
        selectedConversationContainer.size = Vector2(mainWindowAvailableSpace.x * 0.75f, mainWindowAvailableSpace.y * 0.70f);
        selectedConversationContainer.padding = Vector2(15.0f, 15.0f);
        // NOTE: Transparent background
        selectedConversationContainer.bgColor = Rgba(0, 0, 0, 0);
        selectedConversationContainer.DrawContent = [this](const Gui::ContainerState& state) {
            const Vector2 selectedConversationContainerAvailableSpace = m_gui.GetAvailableSpace();

            // MESSSAGES CONTAINER
            Gui::Container messagesContainer = {};
            messagesContainer.id = "messagesContainer";
            messagesContainer.size = Vector2(selectedConversationContainerAvailableSpace);
            messagesContainer.cornerRounding = 10.f;
            messagesContainer.bgColor = Rgba(50, 56, 102, 255);
            messagesContainer.DrawContent = [this](const Gui::ContainerState& state) {
                const Vector2 messagesContainerAvailableSpace = m_gui.GetAvailableSpace();

                std::vector<Message> selectedConversationMessage = m_conversationMessagesMap[m_selectedConversation->id];
                for (int index = 0; index < selectedConversationMessage.size(); index++)
                {
                    // MESSAGE CONTAINER
                    const Message& message = selectedConversationMessage[index];
                    const std::string& id = "messageContainer" + std::to_string(index);

                    Gui::Container messageContainer = {};
                    messageContainer.id = id;
                    messageContainer.size = Vector2(messagesContainerAvailableSpace.x, 0.0f);
                    messageContainer.padding = Vector2(10.0f, 10.0f);
                    // NOTE: Transparent background
                    messageContainer.bgColor = Rgba(0, 0, 0, 0);
                    messageContainer.isAutoResizableY = true;
                    messageContainer.DrawContent = [this, &message](const Gui::ContainerState& state) {
                        const Vector2 messageContainerAvailableSpace = m_gui.GetAvailableSpace();

                        // MESSAGE SENDER IMAGE
                        // NOTE: Images are drawn directly over elements so anything that needs to go beside will have to be postioned manually
                        Gui::Image messageSenderImage = {};
                        messageSenderImage.textureId = m_blankImageTexture.GetID();
                        messageSenderImage.size = Vector2(messageContainerAvailableSpace.x * 0.05f, messageContainerAvailableSpace.x * 0.05f);
                        messageSenderImage.cornerRounding = 10.0f;
                        m_gui.DrawImage(messageSenderImage);

                        // MESSAGE DETAILS CONTAINER
                        Gui::Container messageDetailsContainer = {};
                        messageDetailsContainer.id = "messageDetailsContainer";
                        messageDetailsContainer.size = Vector2(messageContainerAvailableSpace.x - messageSenderImage.size.x, 0.0f);
                        messageDetailsContainer.padding = Vector2(m_gui.GetParentContainerPaddingSize().x / 1.5f, 0.0f);
                        // NOTE: Transparent background
                        messageDetailsContainer.bgColor = Rgba(0, 0, 0, 0);
                        messageDetailsContainer.isAutoResizableY = true;
                        messageDetailsContainer.DrawContent = [this, &message](const Gui::ContainerState& state) {
                            // MESSAGE SENDER FIRSTNAME TEXT
                            Gui::Text messageSenderFirstNameText = {};
                            messageSenderFirstNameText.value = "Firstname of " + message.senderId;
                            m_gui.DrawText(messageSenderFirstNameText);

                            // MESSAGE CREATED AT TEXT
                            std::tm* messageCreatedAtDate = std::localtime(&message.createdAt);
                            const std::string& messageCreatedAtStringDate = asctime(messageCreatedAtDate);

                            Gui::Text messageCreatedAtText = {};
                            messageCreatedAtText.value = messageCreatedAtStringDate;
                            m_gui.DisplayInline();
                            m_gui.DrawText(messageCreatedAtText);

                            // MESSAGE TEXT
                            Gui::Text messageText = {};
                            messageText.value = message.text;
                            m_gui.DrawTextWrapped(messageText);

                        };

                        m_gui.SetPositionX(messageSenderImage.size.x + 10.0f);
                        m_gui.DrawContainer(messageDetailsContainer);

                    };

                    m_gui.DrawContainer(messageContainer);
                }

                // Before drawing content, check if we are already at the bottom
                const bool isAtBottom = m_gui.GetScrollPositionY() >= m_gui.GetMaxScrollPositionY();
                // Auto-scroll logic: only scroll if the user hasn't moved away from the bottom
                if (isAtBottom)
                {
                    // Scrolls to the end
                    m_gui.ScrollToY(1.0f);
                }
            };

            m_gui.DrawContainer(messagesContainer);
        };

        m_gui.DisplayInline();
        m_gui.SetPositionX(mainWindowAvailableSpace.x * 0.25f);
        m_gui.DrawContainer(selectedConversationContainer);

        // MESSAGE TEXT INPUT CONTAINER
        Gui::Container messageTextInputContainer = {};
        messageTextInputContainer.id = "TextInputContainer";
        messageTextInputContainer.size = Vector2(mainWindowAvailableSpace.x * 0.60f, mainWindowAvailableSpace.y * 0.15f);
        messageTextInputContainer.padding = Vector2(15.0f, 15.0f);
        // NOTE: Transparent background
        messageTextInputContainer.bgColor = Rgba(0, 0, 0, 0);
        messageTextInputContainer.DrawContent = [this](const Gui::ContainerState& state) {
            const Vector2 messageTextInputContainerAvailableSpace = m_gui.GetAvailableSpace();

            // MESSAGE TEXT INPUT
            Gui::Placeholder messageTextInputPlaceholder = {};
            messageTextInputPlaceholder.color = Rgba(120, 125, 172, 255);
            if (!m_gui.AreAnyModalsOpen()) messageTextInputPlaceholder.text = "Enter message here...";

            Gui::TextInput messageTextInput = {};
            messageTextInput.id = "messageTextInput";
            messageTextInput.padding = Vector2(15.0f, 15.0f);
            messageTextInput.cornerRounding = 10.f;
            messageTextInput.bgColor = Rgba(43, 50, 94, 255);
            messageTextInput.placeholder = messageTextInputPlaceholder;

            Gui::TextInputMultiline messageTextInputMultiline = {};
            messageTextInputMultiline.textInput = messageTextInput;
            messageTextInputMultiline.size = Vector2(messageTextInputContainerAvailableSpace);

            m_gui.DrawTextInputMultiline(m_messageValue, messageTextInputMultiline);
        };

        m_gui.SetPositionX(mainWindowAvailableSpace.x * 0.25f);
        m_gui.SetPositionY(mainWindowAvailableSpace.y * 0.85f);
        m_gui.DrawContainer(messageTextInputContainer);

        // SEND BUTTON CONTAINER
        Gui::Container sendButtonContainer = {};
        sendButtonContainer.id = "sendButtonContainer";
        sendButtonContainer.size = Vector2(mainWindowAvailableSpace.x * 0.15f, mainWindowAvailableSpace.y * 0.15f);
        sendButtonContainer.padding = Vector2(15.0f, 15.0f);
        // NOTE: Transparent background
        sendButtonContainer.bgColor = Rgba(0, 0, 0, 0);
        sendButtonContainer.DrawContent = [this](const Gui::ContainerState& state) {
            const Vector2 sendButtonContainerAvailableSpace = m_gui.GetAvailableSpace();

            // SEND BUTTON
            Gui::Button sendButton = {};
            sendButton.label = "Send";
            sendButton.size = Vector2(sendButtonContainerAvailableSpace.x * 0.70f, sendButtonContainerAvailableSpace.y * 0.40f);
            sendButton.bgColor = Rgba(200, 30, 30, 255); // Red button
            sendButton.bgColorActive = Rgba(150, 0, 0, 255); // Darker red when active
            sendButton.bgColorHovered = Rgba(255, 100, 100, 255); // Lighter red on hover
            sendButton.cornerRounding = 10.0f;
            sendButton.isDisabled = m_messageValue.empty();
            sendButton.OnClick = [this]() {
                Message newMessage = {};
                newMessage.id = "tempId";
                newMessage.conversationId = m_selectedConversation->id;
                newMessage.senderId = m_currentUser->id;
                newMessage.text = m_messageValue;
                newMessage.createdAt = std::time(0);

                std::vector<Message>& selectedConversationMessages = m_conversationMessagesMap[m_selectedConversation->id];
                selectedConversationMessages.push_back(newMessage);

                HandleSendMessageButtonClicked();
            };

            m_gui.AlignCenter(sendButton.size);
            m_gui.DrawButton(sendButton);
        };

        m_gui.DisplayInline();
        m_gui.SetPositionX(mainWindowAvailableSpace.x * 0.85f);
        m_gui.DrawContainer(sendButtonContainer);
    };

    m_gui.DrawWindow(chatWindow);
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
    m_logger.Info("Authenticated user id: " + userAuthenticatedEvent.user.id);
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

    // search
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
