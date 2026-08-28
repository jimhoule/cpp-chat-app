#pragma once

#include "Gui.h"

#include "auth/UserAuthenticatedEvent.h"
#include "layer/Layer.h"
#include "messages/MessageCreatedEvent.h"
#include "models/Conversation.h"
#include "models/ConversationUser.h"
#include "models/Message.h"
#include "models/User.h"
#include "observer/Observer.h"
#include "Texture.h"

#include <functional>
#include <map>

// Forward declarations
class Navigation;
class AuthApi;
class MessagesApi;
class Logger;

class ChatLayer : public Layer
{
public:
    ChatLayer(const std::string& id, const Gui& gui, Navigation& navigation, AuthApi& authApi, MessagesApi& messagesApi, Logger& logger);

    void OnAttach() override;
    void OnDetach() override;
    void OnRender() override;

private:
    // Gui
    Gui m_gui = {};
    
    Navigation& m_navigation;

    AuthApi& m_authApi;
    Observer<UserAuthenticatedEvent, ChatLayer> m_userAuthenticatedObserver;

    MessagesApi& m_messagesApi;
    Observer<MessageCreatedEvent, ChatLayer> m_messageCreatedObserver; 

    // Settings
    bool m_isSettingsDropDownMenuOpen = false;

    // Search
    std::string m_searchValue = "";

    // Messages
    std::string m_messageValue = "";

    // Users
    std::shared_ptr<User> m_currentUser = nullptr;
    std::vector<std::shared_ptr<User>> m_users = {};
    std::unordered_map<std::string, User> m_usersMap = {};

    // Conversations
    std::shared_ptr<Conversation> m_selectedConversation = nullptr;
    std::vector<std::shared_ptr<Conversation>> m_conversations = {};
    std::unordered_map<std::string, std::vector<Message>> m_conversationMessagesMap = {};


    // Textures
    Texture m_blankImageTexture = {};
    Texture m_closableImageTexture = {};
    Texture m_whiteLeftArrowImageTexture = {};

    void HandleMessageCreated(const MessageCreatedEvent& messageCreatedEvent);
    void HandleUserAuthenticated(const UserAuthenticatedEvent& userAuthenticatedEvent);
    void HandleLogoutButtonClicked();
    void HandleSendMessageButtonClicked();
    void Reset();
};
