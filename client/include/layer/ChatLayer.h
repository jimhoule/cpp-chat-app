#pragma once

#include "Gui.h"

#include "auth/LoggedOutEvent.h"
#include "auth/UserOnlineEvent.h"
#include "conversations/AllOpenConversationsFoundEvent.h"
#include "layer/Layer.h"
#include "messages/MessageCreatedEvent.h"
#include "models/Message.h"
#include "observer/Observer.h"
#include "Texture.h"
#include "views/ConversationView.h"
#include "views/UserView.h"

#include <functional>
#include <map>

// Forward declarations
class Navigation;
class AuthApi;
class ConversationsApi;
class MessagesApi;
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
        Logger& logger
    );

    void OnAttach() override;
    void OnDetach() override;
    void OnRender() override;

private:
    // Gui
    Gui m_gui = {};
    
    Navigation& m_navigation;

    AuthApi& m_authApi;
    Observer<LoggedOutEvent, ChatLayer> m_loggedOutObserver;
    Observer<UserOnlineEvent, ChatLayer> m_userOnlineObserver;

    ConversationsApi& m_conversationsApi;
    Observer<AllOpenConversationsFoundEvent, ChatLayer> m_allOpenConversationsFoundObserver; 

    MessagesApi& m_messagesApi;
    Observer<MessageCreatedEvent, ChatLayer> m_messageCreatedObserver; 

    // Settings
    bool m_isSettingsDropDownMenuOpen = false;

    // Search
    std::string m_searchValue = "";

    // Messages
    std::string m_messageValue = "";

    // Users
    std::vector<std::shared_ptr<UserView>> m_users = {};

    // Conversations
    std::vector<ConversationView> m_conversations = {};
    std::string m_selectedConversationId = "";
    std::unordered_map<std::string, std::vector<Message>> m_conversationMessagesMap = {};

    // Textures
    Texture m_blankImageTexture = {};
    Texture m_closableImageTexture = {};
    Texture m_whiteLeftArrowImageTexture = {};

    void HandleAllOpenConversationsFound(const AllOpenConversationsFoundEvent& allOpenConversationsFoundEvent);
    void HandleMessageCreated(const MessageCreatedEvent& messageCreatedEvent);
    void HandleLoggedOut(const LoggedOutEvent& loggedOutEvent);
    void HandleUserOnline(const UserOnlineEvent& userOnlineEvent);
    void HandleLogoutButtonClicked();
    void HandleSendMessageButtonClicked();
    void Reset();
};
