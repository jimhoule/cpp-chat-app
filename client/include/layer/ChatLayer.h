#pragma once

#include "Gui.h"

#include "auth/LoggedOutEvent.h"
#include "auth/UserOnlineEvent.h"
#include "conversations/AllOpenConversationsFoundEvent.h"
#include "conversations/ConversationClosedEvent.h"
#include "conversations/ConversationOpenedEvent.h"
#include "layer/Layer.h"
#include "messages/MessageCreatedEvent.h"
#include "models/Message.h"
#include "observer/Observer.h"
#include "Texture.h"
#include "users/UsersSearchedEvent.h"
#include "utils/Debouncer.h"
#include "views/ConversationView.h"
#include "views/UserView.h"

#include <string>
#include <unordered_map>
#include <vector>

// Forward declarations
class Navigation;
class AuthApi;
class ConversationsApi;
class MessagesApi;
class UsersApi;
class Logger;

class ChatLayer : public Layer
{
public:
    ChatLayer(
        const std::string& id,
        const Gui& gui,
        Navigation& navigation,
        AuthApi& authApi,
        ConversationsApi& conversationsApi,
        MessagesApi& messagesApi,
        UsersApi& usersApi,
        Logger& logger
    );

    void OnAttach() override;
    void OnDetach() override;
    void OnRender() override;
    void OnUpdate() override;

private:
    // Gui
    Gui m_gui = {};
    
    Navigation& m_navigation;

    AuthApi& m_authApi;
    Observer<LoggedOutEvent, ChatLayer> m_loggedOutObserver;
    Observer<UserOnlineEvent, ChatLayer> m_userOnlineObserver;

    ConversationsApi& m_conversationsApi;
    Observer<AllOpenConversationsFoundEvent, ChatLayer> m_allOpenConversationsFoundObserver;
    Observer<ConversationClosedEvent, ChatLayer> m_conversationClosedObserver;
    Observer<ConversationOpenedEvent, ChatLayer> m_conversationOpenedObserver;

    MessagesApi& m_messagesApi;
    Observer<MessageCreatedEvent, ChatLayer> m_messageCreatedObserver;

    UsersApi& m_usersApi;
    Observer<UsersSearchedEvent, ChatLayer> m_usersSearchedObserver;

    // Settings
    bool m_isSettingsDropDownMenuOpen = false;

    // Search
    std::string m_searchUsersValue = "";
    Debouncer m_searchUsersDebouncer = Debouncer(std::chrono::milliseconds(300));

    // Messages
    std::string m_messageValue = "";

    // Users
    std::vector<UserView> m_searchedUsers = {};

    // Conversations
    std::vector<ConversationView> m_conversations = {};
    std::string m_selectedConversationId = "";
    std::unordered_map<std::string, std::vector<Message>> m_conversationMessagesMap = {};

    // Textures
    Texture m_blankImageTexture = {};
    Texture m_closableImageTexture = {};
    Texture m_whiteLeftArrowImageTexture = {};

    void HandleAllOpenConversationsFound(const AllOpenConversationsFoundEvent& allOpenConversationsFoundEvent);
    void HandleConversationClosed(const ConversationClosedEvent& conversationClosedEvent);
    void HandleConversationOpened(const ConversationOpenedEvent& conversationOpenedEvent);
    void HandleMessageCreated(const MessageCreatedEvent& messageCreatedEvent);
    void HandleLoggedOut(const LoggedOutEvent& loggedOutEvent);
    void HandleUserOnline(const UserOnlineEvent& userOnlineEvent);
    void HandleUsersSearched(const UsersSearchedEvent& usersSearchedEvent);
    void HandleCloseConversationButtonClicked(const std::string& conversationId);
    void HandleLogoutButtonClicked();
    void HandleSendMessageButtonClicked();
    // NOTE: Only opens conversation with one user for now (2 users total with the current user added by the server)
    void HandleSearchedUserButtonClicked(const std::string& userId);
    void Reset();
};
