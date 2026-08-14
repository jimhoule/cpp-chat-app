#pragma once

#include "Gui.h"
#include "layer/Layer.h"
#include "models/Conversation.h"
#include "models/ConversationUser.h"
#include "models/Message.h"
#include "models/User.h"
#include "socket/SocketClient.h"
#include "Texture.h"

#include <functional>
#include <map>

class ChatLayer : public Layer
{
public:
    ChatLayer(const std::string& ID, std::shared_ptr<SocketClient> SocketClient, const Gui& Gui);
    ChatLayer(const std::string& ID, std::shared_ptr<SocketClient> SocketClient, const Gui& Gui, const std::shared_ptr<Logger>& Logger);

    void OnAttach() override;
    void OnDetach() override;
    void OnRender() override;

    std::function<void(const std::string&, const std::string&)> OnSendMessageButtonClick;
    std::function<void()> OnLogoutButtonClick;

private:
    // Gui
    Gui m_Gui = {};

    // Settings
    bool m_IsSettingsDropDownMenuOpen = false;

    // Search
    std::string m_SearchValue = "";

    // Messages
    std::string m_MessageValue = "";

    // Users
    std::shared_ptr<User> m_CurrentUser = nullptr;
    std::vector<std::shared_ptr<User>> m_Users = {};
    std::unordered_map<std::string, User> m_usersMap = {};

    // Conversations
    std::shared_ptr<Conversation> m_SelectedConversation = nullptr;
    std::vector<std::shared_ptr<Conversation>> m_Conversations = {};
    std::unordered_map<std::string, std::vector<Message>> m_conversationMessagesMap = {};


    // Socket
    std::shared_ptr<SocketClient> m_SocketClient = nullptr;

    // Textures
    Texture m_BlankImageTexture = {};
    Texture m_ClosableImageTexture = {};
    Texture m_WhiteLeftArrowImageTexture = {};

    void Reset();
};
