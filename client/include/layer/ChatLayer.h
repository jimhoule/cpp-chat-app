#pragma once

#include "Gui.h"
#include "ScrollAnchor.h"
#include "auth/LoggedOutEvent.h"
#include "auth/UserOnlineEvent.h"
#include "conversations/AllOpenConversationsFoundEvent.h"
#include "conversations/ConversationClosedEvent.h"
#include "conversations/ConversationOpenedEvent.h"
#include "conversations/ConversationReopenedEvent.h"
#include "layer/Layer.h"
#include "messages/AllOpenMessagesReadStatesFoundEvent.h"
#include "messages/MessageHistories.h"
#include "messages/MessageCreatedEvent.h"
#include "messages/MessagesFirstUnreadMatcher.h"
#include "messages/MessagesMarkedReadEvent.h"
#include "messages/MessagesPageFoundEvent.h"
#include "messages/MessagesReadStateFoundEvent.h"
#include "messages/MessagesReadStates.h"
#include "observer/Observer.h"
#include "Texture.h"
#include "users/UsersSearchedEvent.h"
#include "utils/Debouncer.h"
#include "views/ConversationView.h"
#include "views/MessageView.h"
#include "views/UserView.h"

#include <string>
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
    Gui m_gui = {};
    
    Navigation& m_navigation;

    AuthApi& m_authApi;
    Observer<LoggedOutEvent, ChatLayer> m_loggedOutObserver;
    Observer<UserOnlineEvent, ChatLayer> m_userOnlineObserver;

    ConversationsApi& m_conversationsApi;
    Observer<AllOpenConversationsFoundEvent, ChatLayer> m_allOpenConversationsFoundObserver;
    Observer<ConversationClosedEvent, ChatLayer> m_conversationClosedObserver;
    Observer<ConversationOpenedEvent, ChatLayer> m_conversationOpenedObserver;
    Observer<ConversationReopenedEvent, ChatLayer> m_conversationReopenedObserver;

    MessagesApi& m_messagesApi;
    Observer<AllOpenMessagesReadStatesFoundEvent, ChatLayer> m_allOpenMessagesReadStatesFoundObserver;
    Observer<MessageCreatedEvent, ChatLayer> m_messageCreatedObserver;
    Observer<MessagesPageFoundEvent, ChatLayer> m_messagesPageFoundObserver;
    Observer<MessagesMarkedReadEvent, ChatLayer> m_messagesMarkedReadObserver;
    Observer<MessagesReadStateFoundEvent, ChatLayer> m_messagesReadStateFoundObserver;

    UsersApi& m_usersApi;
    Observer<UsersSearchedEvent, ChatLayer> m_usersSearchedObserver;

    // Settings
    bool m_isSettingsDropDownMenuOpen = false;

    // Search
    std::vector<UserView> m_searchedUsers = {};
    std::string m_searchUsersValue = "";
    Debouncer m_searchUsersDebouncer = Debouncer(std::chrono::milliseconds(300));

    // Messages
    std::string m_messageText = "";
    MessageHistories m_messageHistories = {};
    MessagesFirstUnreadMatcher m_messagesFirstUnreadMatcher = {};
    MessagesReadStates m_messagesReadStates = {};
    ScrollAnchor m_messagesScrollAnchor = {};

    // Conversations
    std::vector<ConversationView> m_conversations = {};
    std::string m_selectedConversationId = "";

    // Textures
    Texture m_blankImageTexture = {};
    Texture m_closableImageTexture = {};
    Texture m_whiteLeftArrowImageTexture = {};

    void HandleAllOpenConversationsFound(const AllOpenConversationsFoundEvent& allOpenConversationsFoundEvent);
    void HandleAllOpenMessagesReadStatesFound(const AllOpenMessagesReadStatesFoundEvent& allOpenMessagesReadStatesFoundEvent);
    void HandleConversationClosed(const ConversationClosedEvent& conversationClosedEvent);
    void HandleConversationOpened(const ConversationOpenedEvent& conversationOpenedEvent);
    void HandleConversationReopened(const ConversationReopenedEvent& conversationReopenedEvent);
    void HandleMessageCreated(const MessageCreatedEvent& messageCreatedEvent);
    void HandleMessagesMarkedRead(const MessagesMarkedReadEvent& messagesMarkedReadEvent);
    void HandleMessagesPageFound(const MessagesPageFoundEvent& messagesPageFoundEvent);
    void HandleMessagesReadStateFound(const MessagesReadStateFoundEvent& messagesReadStateFoundEvent);
    void HandleLoggedOut(const LoggedOutEvent& loggedOutEvent);
    void HandleUserOnline(const UserOnlineEvent& userOnlineEvent);
    void HandleUsersSearched(const UsersSearchedEvent& usersSearchedEvent);
    void HandleCloseConversationButtonClicked(const std::string& conversationId);
    void HandleLogoutButtonClicked();
    void HandleSendMessageButtonClicked();
    // NOTE: Only opens conversation with one user for now (2 users total with the current user added by the server)
    void HandleSearchedUserButtonClicked(const std::string& userId);

    const UserView* FindConversationUser(const std::string& conversationId, const std::string& userId) const;
    void FindMessagesReadState(const std::string& conversationId);
    bool IsConversationDisplayed(const std::string& conversationId) const;
    void MarkMessagesRead(const std::string& conversationId);
    void Reset();
    void SelectConversation(const std::string& conversationId);
    void UpsertConversation(const ConversationView& upsertedConversation);
};
