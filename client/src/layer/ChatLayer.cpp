#include "layer/ChatLayer.h"

#include "auth/AuthApi.h"
#include "conversations/ConversationsApi.h"
#include "deserializer/MessageCreatedSocketEventPayloadDeserializer.h"
#include "deserializer/UserOnlineSocketEventPayloadDeserializer.h"
#include "log/Logger.h"
#include "navigation/Navigation.h"
#include "messages/MessagesApi.h"
#include "users/UsersApi.h"

constexpr int SERVER_PORT =  5000;

// **********
// * PUBLIC *
// **********

ChatLayer::ChatLayer(
    const std::string& id,
    const Gui& gui,
    Navigation& navigation,
    AuthApi& authApi,
    ConversationsApi& conversationsApi,
    MessagesApi& messagesApi,
    UsersApi& usersApi,
    Logger& logger
)
    : Layer(id, logger)
    , m_gui(gui)
    , m_navigation(navigation)
    , m_authApi(authApi)
    , m_conversationsApi(conversationsApi)
    , m_messagesApi(messagesApi)
    , m_usersApi(usersApi)
    , m_loggedOutObserver(*this, &ChatLayer::HandleLoggedOut)
    , m_userOnlineObserver(*this, &ChatLayer::HandleUserOnline)
    , m_allOpenConversationsFoundObserver(*this, &ChatLayer::HandleAllOpenConversationsFound)
    , m_conversationClosedObserver(*this, &ChatLayer::HandleConversationClosed)
    , m_conversationOpenedObserver(*this, &ChatLayer::HandleConversationOpened)
    , m_conversationReopenedObserver(*this, &ChatLayer::HandleConversationReopened)
    , m_messageCreatedObserver(*this, &ChatLayer::HandleMessageCreated)
    , m_usersSearchedObserver(*this, &ChatLayer::HandleUsersSearched)
{}

void ChatLayer::OnAttach()
{
    m_authApi.GetLoggedOutSubject().Subscribe(&m_loggedOutObserver);
    m_authApi.GetUserOnlineSubject().Subscribe(&m_userOnlineObserver);
    m_conversationsApi.GetAllOpenConversationsFoundSubject().Subscribe(&m_allOpenConversationsFoundObserver);
    m_conversationsApi.GetConversationClosedSubject().Subscribe(&m_conversationClosedObserver);
    m_conversationsApi.GetConversationOpenedSubject().Subscribe(&m_conversationOpenedObserver);
    m_conversationsApi.GetConversationReopenedSubject().Subscribe(&m_conversationReopenedObserver);
    m_messagesApi.GetMessageCreatedSubject().Subscribe(&m_messageCreatedObserver);
    m_usersApi.GetUsersSearchedSubject().Subscribe(&m_usersSearchedObserver);

    m_conversationsApi.FindAllOpen();

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
    m_authApi.GetLoggedOutSubject().Unsubscribe(&m_loggedOutObserver);
    m_authApi.GetUserOnlineSubject().Unsubscribe(&m_userOnlineObserver);
    m_conversationsApi.GetAllOpenConversationsFoundSubject().Unsubscribe(&m_allOpenConversationsFoundObserver);
    m_conversationsApi.GetConversationClosedSubject().Unsubscribe(&m_conversationClosedObserver);
    m_conversationsApi.GetConversationOpenedSubject().Unsubscribe(&m_conversationOpenedObserver);
    m_conversationsApi.GetConversationReopenedSubject().Unsubscribe(&m_conversationReopenedObserver);
    m_messagesApi.GetMessageCreatedSubject().Unsubscribe(&m_messageCreatedObserver);
    m_usersApi.GetUsersSearchedSubject().Unsubscribe(&m_usersSearchedObserver);
    
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
                            SearchModalCloseImageButtonImage.textureId = m_whiteLeftArrowImageTexture.GetId();
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
                        m_gui.DrawTextInputSingleline(m_searchUsersValue, searchModalTextInputSingleline);

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
                        if (m_searchUsersValue.empty())
                        {
                            return;
                        }

                        const Vector2 searchModalBodyContainerAvailableSpace = m_gui.GetAvailableSpace();

                        for (std::size_t index = 0; index < m_searchedUsers.size(); index++)
                        {
                            // NOTE: Nothing erases during the loop so using a const reference is fine here
                            const UserView& user = m_searchedUsers[index];

                            // USER CONTAINER
                            Gui::Container userContainer = {};
                            userContainer.id = user.id;
                            userContainer.size = Vector2(searchModalBodyContainerAvailableSpace.x, 40.0f);
                            userContainer.cornerRounding = 10.f;
                            // NOTE: Transparent background
                            userContainer.bgColor = Rgba(0, 0, 0, 0);
                            userContainer.bgColorHovered = Rgba(50, 56, 102, 255);
                            userContainer.isAutoResizableY = true;
                            userContainer.OnClick = [this, &searchModal, &user]() {
                                HandleSearchedUserButtonClicked(user.id);
                                
                                m_gui.CloseModal(searchModal.id);
                                m_searchUsersValue = "";
                            };
                            userContainer.DrawContent = [this, &user](const Gui::ContainerState& state) {
                                const Vector2 userContainerAvailableSpace = m_gui.GetAvailableSpace();

                                // USER IMAGE
                                Gui::Image userImage = {};
                                userImage.textureId = m_blankImageTexture.GetId();
                                userImage.size = Vector2(userContainerAvailableSpace.y, userContainerAvailableSpace.y);
                                userImage.cornerRounding = 10.0f;

                                m_gui.DrawImage(userImage);

                                // USER TEXT
                                Gui::Text userText = {};
                                userText.value = user.firstName + " " + user.lastName;

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
                    if (!m_gui.AreAnyModalsOpen())
                    {
                        searchTextInputPlaceholder.text = "Enter search here...";
                    }

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
                        currentUserImage.textureId = m_blankImageTexture.GetId();
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
                        if (m_gui.AreAnyModalsOpen())
                        {
                            m_isSettingsDropDownMenuOpen = false;
                        }

                        if (!m_isSettingsDropDownMenuOpen)
                        {
                            return;
                        }

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

                    for (std::size_t index = 0; index < m_conversations.size(); index++)
                    {
                        const ConversationView conversation = m_conversations[index];

                        // CONVERSATION CONTAINER
                        Rgba bgColor = Rgba(50, 56, 102, 255);
                        if (conversation.id == m_selectedConversationId) bgColor = Rgba(100, 100, 100, 255);

                        Gui::Container conversationContainer = {};
                        conversationContainer.id = "conversationContainer" + conversation.id;
                        conversationContainer.size = Vector2(conversationsNodeAvailableSpace.x, conversationsNodeAvailableSpace.y * 0.05f);
                        conversationContainer.cornerRounding = 10.f;
                        conversationContainer.bgColor = bgColor;
                        conversationContainer.bgColorHovered = Rgba(0, 0, 0, 255);
                        conversationContainer.isAutoResizableY = true;
                        conversationContainer.OnClick = [this, &conversation]() {
                            m_selectedConversationId = conversation.id;
                            m_logger.Info("SELECTED CONVERSATION id: " + m_selectedConversationId);
                        };
                        conversationContainer.DrawContent = [this, &conversation, index](const Gui::ContainerState& state) {
                            const Vector2 conversationContainerAvailableSpace = m_gui.GetAvailableSpace();

                            // CONVERSATION IMAGE
                            Gui::Image conversationImage = {};
                            conversationImage.textureId = m_blankImageTexture.GetId();
                            conversationImage.size = Vector2(conversationContainerAvailableSpace.y, conversationContainerAvailableSpace.y);
                            conversationImage.cornerRounding = 10.0f;
                            m_gui.DrawImage(conversationImage);

                            // CONVERSATION TEXT
                            Gui::Text conversationText = {};
                            // Selects name of first user that is not the current user
                            for (const ConversationUserView& conversationUserView : conversation.users)
                            {
                                if (conversationUserView.user.id != m_authApi.GetCurrentUser()->id)
                                {
                                    conversationText.value = conversationUserView.user.firstName;
                                    break;
                                }
                            }

                            /**
                             * NOTES:
                             *  - Falls back to current user for a conversation with oneself
                             *  - If conversation text is empty, it means it is a self conversation
                             */
                            if (conversationText.value.empty())
                            {
                                conversationText.value = m_authApi.GetCurrentUser()->firstName + " (you)";
                            }

                            m_gui.SetPositionX(conversationImage.size.x + 10.0f);
                            m_gui.DrawText(conversationText);

                            // CLOSE CONVERSATION IMAGE BUTTON CONTAINER
                            if (!state.isHovered)
                            {
                                return;
                            }

                            Gui::Container closeConversationImageButtonContainer = {};
                            closeConversationImageButtonContainer.id = "closeConversationImageButtonContainer" + conversation.id;
                            closeConversationImageButtonContainer.size = conversationImage.size;
                            closeConversationImageButtonContainer.padding = Vector2(5.0f, 5.0f);
                            // NOTE: Transparent background
                            closeConversationImageButtonContainer.bgColor = Rgba(0, 0, 0, 0);
                            closeConversationImageButtonContainer.DrawContent = [this, &conversation, index](const Gui::ContainerState& state) {
                                // CLOSE CONVERSATION IMAGE BUTTON
                                Gui::Image closeConversationImageButtonImage = {};
                                closeConversationImageButtonImage.textureId = m_closableImageTexture.GetId();
                                closeConversationImageButtonImage.size = m_gui.GetAvailableSpace();
                                closeConversationImageButtonImage.tintColor = Rgba(255, 255, 255, 255);
                                closeConversationImageButtonImage.cornerRounding = 0.0f;

                                Gui::ImageButton closeConversationImageButton = {};
                                closeConversationImageButton.id = "closeConversationImageButton" + conversation.id;
                                closeConversationImageButton.image = closeConversationImageButtonImage;
                                closeConversationImageButton.tintColorHovered = Rgba(200, 200, 0, 255);
                                closeConversationImageButton.OnClick = [this, index]() {
                                    const std::string id = m_conversations[index].id;
                                    HandleCloseConversationButtonClicked(id);
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

                const std::vector<MessageView>& selectedConversationMessage = m_conversationMessagesMap[m_selectedConversationId];
                for (std::size_t index = 0; index < selectedConversationMessage.size(); index++)
                {
                    // MESSAGE CONTAINER
                    const MessageView& message = selectedConversationMessage[index];
                    const std::string id = "messageContainer" + std::to_string(index);

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
                        messageSenderImage.textureId = m_blankImageTexture.GetId();
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
            if (!m_gui.AreAnyModalsOpen())
            {
                messageTextInputPlaceholder.text = "Enter message here...";
            }

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
            sendButton.isDisabled = m_messageValue.empty() || m_selectedConversationId.empty();
            sendButton.OnClick = [this]() {
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

void ChatLayer::OnUpdate()
{
    // Debounces search users
    const bool isSearchValueDebounced = m_searchUsersDebouncer.Update(m_searchUsersValue);
    if (isSearchValueDebounced)
    {
        UsersApi::SearchUsersParams searchUsersParams = {};
        searchUsersParams.search = m_searchUsersValue;
        m_usersApi.Search(searchUsersParams);
    }
}

// ***********
// * PRIVATE *
// ***********
void ChatLayer::HandleAllOpenConversationsFound(const AllOpenConversationsFoundEvent& allOpenConversationsFoundEvent)
{
    m_conversations = allOpenConversationsFoundEvent.conversations;
}

void ChatLayer::HandleConversationClosed(const ConversationClosedEvent& conversationClosedEvent)
{
    for (std::size_t index = 0; index < m_conversations.size(); index++)
    {
        const ConversationView& conversation = m_conversations[index];
        if (conversation.id == conversationClosedEvent.conversation.id)
        {
            m_conversations.erase(m_conversations.begin() + index);
            break;
        }
    }

    // Checks if list is now empty
    if (m_conversations.empty())
    {
        m_selectedConversationId = "";
        return;
    }

    // Selects first conversation if deleted conversation is the selected one
    if (m_selectedConversationId == conversationClosedEvent.conversation.id)
    {
        m_selectedConversationId = m_conversations[0].id;
    }
}

void ChatLayer::HandleConversationOpened(const ConversationOpenedEvent& conversationOpenedEvent)
{
    const ConversationView& conversation = conversationOpenedEvent.conversation;

    UpsertConversation(conversation);
    // NOTE: Client asked to open it so it gets the focus
    m_selectedConversationId = conversation.id;
}

void ChatLayer::HandleConversationReopened(const ConversationReopenedEvent& conversationReopenedEvent)
{
    // NOTE: Reopened by someone else's message so it is added without taking the focus
    UpsertConversation(conversationReopenedEvent.conversation);
}

void ChatLayer::HandleMessageCreated(const MessageCreatedEvent& messageCreatedEvent)
{
    const MessageView& message = messageCreatedEvent.message;

    // NOTE: A message in a conversation a user had closed comes after CONVERSATION_REOPENED so the conversation is already open again
    std::vector<MessageView>& conversationMessages = m_conversationMessagesMap[message.conversationId];
    conversationMessages.push_back(message);
}

void ChatLayer::HandleLoggedOut(const LoggedOutEvent& loggedOutEvent)
{
    m_navigation.GoToLoginScreen();
}

void ChatLayer::HandleUserOnline(const UserOnlineEvent& userOnlineEvent)
{
    // TODO: This function will be used to change the online status of a conversation user in an open conversation
    m_logger.Info("Online user id: " + userOnlineEvent.user.id);
}

void ChatLayer::HandleUsersSearched(const UsersSearchedEvent& usersSearchedEvent)
{
    m_searchedUsers = usersSearchedEvent.users;
}

void ChatLayer::HandleCloseConversationButtonClicked(const std::string& conversationId)
{
    ConversationsApi::CloseConversationParams closeConversationParams = {};
    closeConversationParams.conversationId = conversationId;
    m_conversationsApi.Close(closeConversationParams);
}

void ChatLayer::HandleLogoutButtonClicked()
{
    m_authApi.Logout();
}

void ChatLayer::HandleSendMessageButtonClicked()
{
    MessagesApi::CreateMessageParams createMessageParams = {};
    createMessageParams.conversationId = m_selectedConversationId;
    createMessageParams.text = m_messageValue;
    m_messagesApi.Create(createMessageParams);

    m_logger.Info("SENT: " + m_messageValue);
}

// NOTE: Only opens conversation with one user for now (2 users total with the current user added by the server)
void ChatLayer::HandleSearchedUserButtonClicked(const std::string& userId)
{
    const std::vector<std::string> userIds = { userId };

    ConversationsApi::OpenConversationParams openConversationParams = {};
    openConversationParams.userIds = userIds;
    m_conversationsApi.Open(openConversationParams);
}

void ChatLayer::Reset()
{
    // Settings
    m_isSettingsDropDownMenuOpen = false;

    // search
    m_searchedUsers.clear();
    m_searchUsersValue.clear();
    m_searchUsersDebouncer.Reset();

    // Conversations
    m_conversations.clear();
    m_selectedConversationId = "";
    m_conversationMessagesMap = {};

    // Messages
    m_messageValue.clear();

    // Textures
    m_blankImageTexture.Destroy();
    m_closableImageTexture.Destroy();
    m_whiteLeftArrowImageTexture.Destroy();
}

void ChatLayer::UpsertConversation(const ConversationView& upsertedConversation)
{
    /**
     * NOTES:
     *  - Replaces conversation with most recent one if already listed (Open() returns the existing one for the same users)
     *  - Replaces conversation because other participants properties may have been updated (ex: name)
     */
    for (ConversationView& conversation : m_conversations)
    {
        if (conversation.id == upsertedConversation.id)
        {
            conversation = upsertedConversation;

            return;
        }
    }

    m_conversations.push_back(upsertedConversation);
}
