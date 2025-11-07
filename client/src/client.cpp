#include "Gui.h"
#include "SocketClient.h"
#include "Texture.h"

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <arpa/inet.h>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <unistd.h>
#include <vector>

constexpr int SERVER_PORT =  5000;

struct User
{
    std::string ID;
    std::string FirstName;
    std::string ImageUrl;
};

struct Message
{
    std::string ID;
    std::string ConversationID;
    std::string SenderID;
    std::string SenderFirstName;
    std::string SenderImageUrl;
    std::string Text;
    std::time_t CreatedAt;
};

struct Conversation
{
    std::string ID;
    std::vector<Message> Messages;
    std::vector<User> Users;
    std::time_t CreatedAt;
};

void framebuffer_size_callback(GLFWwindow* Window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void myKeyCallback(GLFWwindow* Window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        printf("You pressed the e key!\n");
    }
}

int main()
{
    if(!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac

    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    const std::string& window_title = "Chat Client";

    GLFWwindow* GlfwWindow = glfwCreateWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        window_title.c_str(),
        NULL,
        NULL
    );

    if(GlfwWindow == nullptr)
    {
        std::cout << "Failed to open GLFW Window" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(GlfwWindow);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //Register Key Callback -> glfwPollEvents
    glfwSetKeyCallback(GlfwWindow, myKeyCallback);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glfwSetFramebufferSizeCallback(GlfwWindow, framebuffer_size_callback);

    Texture BlankImageTexture = {};
    BlankImageTexture.Load("../../assets/blank.jpg", 0);
    BlankImageTexture.Bind();

    Gui ClientGui = {};
    ClientGui.Init(GlfwWindow);

    // Fake Users
    User User1 = {};
    User1.ID = "User1";
    User1.FirstName = "Olivier";
    User1.ImageUrl = "http://fake.iamge.url";

    User User2 = {};
    User1.ID = "User2";
    User1.FirstName = "Marc";
    User1.ImageUrl = "http://fake.iamge.url";

    User User3 = {};
    User1.ID = "User3";
    User1.FirstName = "Simon";
    User1.ImageUrl = "http://fake.iamge.url";

    // Fake conversations
    Conversation Conversation1 = {};
    Conversation1.ID = "Conversation1";
    Conversation1.Users = { User1, User2 };
    Conversation1.Messages = {};
    Conversation1.CreatedAt = std::time(0);

    Conversation Conversation2 = {};
    Conversation2.ID = "Conversation2";
    Conversation2.Users = { User1, User3 };
    Conversation2.Messages = {};
    Conversation2.CreatedAt = std::time(0);

    static std::vector<std::shared_ptr<Conversation>> Conversations = {
        std::make_shared<Conversation>(Conversation1),
        std::make_shared<Conversation>(Conversation2)
    };
    static std::shared_ptr<Conversation> SelectedConversation = Conversations[0];

    // SocketClient socket_client;
    // socket_client.Connect(SERVER_PORT, "127.0.0.1");

    // sleep(1);

    // std::string message = "Hello from Client Socket!";
    // socket_client.Send(message);
    // socket_client.Close();

    while(!glfwWindowShouldClose(GlfwWindow))
    {
        // Clears screen
        glClearColor(250.0f / 255.0f, 119.0f / 255.0f, 110.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

         // Clears ImGui state
        ClientGui.Clear();

        // WINDOW
        Window MainWindow = {};
        MainWindow.Name = "MainWindow";
        MainWindow.Size = Vector2(WINDOW_WIDTH, WINDOW_HEIGHT);
        MainWindow.BgColor = Rgba(26, 30, 67, 255);
        MainWindow.DrawContent = [&ClientGui, &BlankImageTexture]() {
            const Vector2 MAIN_WINDOW_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

            // NAVBAR CONTAINER
            Container NavbarContainer = {};
            NavbarContainer.ID = "NavbarContainer";
            NavbarContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X, MAIN_WINDOW_AVAILABLE_SPACE.Y * 0.15f);
            NavbarContainer.Padding = Vector2(15.0f, 15.0f);
            // NOTE: Transparent background
            NavbarContainer.BgColor = Rgba(0, 0, 0, 0);
            NavbarContainer.DrawContent = [&ClientGui]() {
                const Vector2 NAVBAR_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                // NAVBAR
                Container Navbar = {};
                Navbar.ID = "Navbar";
                Navbar.Size = Vector2(NAVBAR_CONTAINER_AVAILABLE_SPACE);
                Navbar.CornerRounding = 10.f;
                Navbar.BgColor = Rgba(50, 56, 102, 255);
                Navbar.DrawContent = [&ClientGui]() {};

                ClientGui.DrawContainer(Navbar);
            };

            ClientGui.DrawContainer(NavbarContainer);

            // CHATS CONTAINER
            Container ChatsContainer = {};
            ChatsContainer.ID = "ChatsContainer";
            ChatsContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.25f, MAIN_WINDOW_AVAILABLE_SPACE.Y - NavbarContainer.Size.Y);
            ChatsContainer.Padding = Vector2(15.0f, 15.0f);
            // NOTE: Transparent background
            ChatsContainer.BgColor = Rgba(0, 0, 0, 0);
            ChatsContainer.DrawContent = [&ClientGui, &BlankImageTexture]() {
                const Vector2 CHATS_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                // CONVERSATIONS CONTAINER
                Container ConversationsContainer = {};
                ConversationsContainer.ID = "ConversationsContainer";
                ConversationsContainer.Size = Vector2(CHATS_CONTAINER_AVAILABLE_SPACE);
                ConversationsContainer.CornerRounding = 10.f;
                ConversationsContainer.BgColor = Rgba(50, 56, 102, 255);
                ConversationsContainer.DrawContent = [&ClientGui, &BlankImageTexture]() {
                    const Vector2 CONVERSATIONS_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                    // CONVERSATIONS NODE
                    Node ConversationsNode = {};
                    ConversationsNode.Name = "Conversations";
                    ConversationsNode.DrawContent = [&ClientGui, &BlankImageTexture]() {
                        const Vector2 CONVERSATIONS_NODE_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                        for (int i = 0; i < Conversations.size(); i++)
                        {
                            const std::shared_ptr<Conversation> Conversation = Conversations[i];
                            const std::string& ID = "ConversationContainer" + std::to_string(i);

                            // CONVERSATION CONTAINER
                            Rgba BgColor = Rgba(50, 56, 102, 255);
                            if (Conversation->ID == SelectedConversation->ID) BgColor = Rgba(100, 100, 100, 255);

                            Container ConversationContainer = {};
                            ConversationContainer.ID = ID;
                            ConversationContainer.Size = Vector2(CONVERSATIONS_NODE_AVAILABLE_SPACE.X, CONVERSATIONS_NODE_AVAILABLE_SPACE.Y * 0.05f);
                            ConversationContainer.CornerRounding = 10.f;
                            ConversationContainer.BgColor = BgColor;
                            ConversationContainer.BgColorHovered = Rgba(0, 0, 0, 255);
                            ConversationContainer.IsAutoResizableY = true;
                            ConversationContainer.DrawContent = [&ClientGui, &BlankImageTexture, &Conversation]() {
                                const Vector2 CONVERSATION_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                                // CONVERSATION IMAGE
                                Image ConversationImage = {};
                                ConversationImage.TextureID = BlankImageTexture.GetID();
                                ConversationImage.Size = Vector2(CONVERSATION_CONTAINER_AVAILABLE_SPACE.Y, CONVERSATION_CONTAINER_AVAILABLE_SPACE.Y);
                                ConversationImage.CornerRounding = 10.0f;
                                ClientGui.DrawImage(ConversationImage);

                                // SELECT CONVERSATION BUTTON
                                ImVec2 TextSize = ImGui::CalcTextSize(Conversation->ID.c_str());

                                Button SelectConversationButton = {};
                                SelectConversationButton.Label = Conversation->ID;
                                SelectConversationButton.Size = Vector2(TextSize.x, TextSize.y);
                                // NOTE: Transparent background
                                SelectConversationButton.BgColor = Rgba(0, 0, 0, 0);
                                // NOTE: Transparent background
                                SelectConversationButton.BgColorActive = Rgba(0, 0, 0, 0);
                                // NOTE: Transparent background
                                SelectConversationButton.BgColorHovered = Rgba(0, 0, 0, 0);
                                SelectConversationButton.OnClick = [&Conversation]() {
                                    SelectedConversation = Conversation;
                                    std::cout << "SELECTED CONVERSATION ID: " << SelectedConversation->ID << std::endl;
                                };

                                ClientGui.SetPositionX(ConversationImage.Size.X + 10.0f);
                                ClientGui.DrawButton(SelectConversationButton);
                            };

                            ClientGui.DrawContainer(ConversationContainer);
                        }
                    };

                    ClientGui.DrawNode(ConversationsNode);
                };

                ClientGui.DrawContainer(ConversationsContainer);
            };

            ClientGui.DrawContainer(ChatsContainer);

            // SELECTED CONVERSATION CONTAINER
            Container SelectedConversationContainer = {};
            SelectedConversationContainer.ID = "SelectedConversationContainer";
            SelectedConversationContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.75f, MAIN_WINDOW_AVAILABLE_SPACE.Y * 0.70f);
            SelectedConversationContainer.Padding = Vector2(15.0f, 15.0f);
            // NOTE: Transparent background
            SelectedConversationContainer.BgColor = Rgba(0, 0, 0, 0);
            SelectedConversationContainer.DrawContent = [&ClientGui, &BlankImageTexture]() {
                const Vector2 SELECTED_CONVERSATION_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                // MESSSAGES CONTAINER
                Container MessagesContainer = {};
                MessagesContainer.ID = "MessagesContainer";
                MessagesContainer.Size = Vector2(SELECTED_CONVERSATION_CONTAINER_AVAILABLE_SPACE);
                MessagesContainer.CornerRounding = 10.f;
                MessagesContainer.BgColor = Rgba(50, 56, 102, 255);
                MessagesContainer.DrawContent = [&ClientGui, &BlankImageTexture]() {
                    const Vector2 MESSAGES_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                    for (int i = 0; i < SelectedConversation->Messages.size(); i++)
                    {
                        // MESSAGE CONTAINER
                        const Message& MESSAGE = SelectedConversation->Messages[i];
                        const std::string& ID = "MessageContainer" + std::to_string(i);

                        Container MessageContainer = {};
                        MessageContainer.ID = ID;
                        MessageContainer.Size = Vector2(MESSAGES_CONTAINER_AVAILABLE_SPACE.X, 0.0f);
                        MessageContainer.Padding = Vector2(10.0f, 10.0f);
                        // NOTE: Transparent background
                        MessageContainer.BgColor = Rgba(0, 0, 0, 0);
                        MessageContainer.IsAutoResizableY = true;
                        MessageContainer.DrawContent = [&ClientGui, &BlankImageTexture, &MESSAGE]() {
                            const Vector2 MESSAGE_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                            // MESSAGE SENDER IMAGE
                            // NOTE: Images are drawn directly over elements so anything that needs to go beside will have to be postioned manually
                            Image MessageSenderImage = {};
                            MessageSenderImage.TextureID = BlankImageTexture.GetID();
                            MessageSenderImage.Size = Vector2(MESSAGE_CONTAINER_AVAILABLE_SPACE.X * 0.05f, MESSAGE_CONTAINER_AVAILABLE_SPACE.X * 0.05f);
                            MessageSenderImage.CornerRounding = 10.0f;
                            ClientGui.DrawImage(MessageSenderImage);

                            // MESSAGE DETAILS CONTAINER
                            ImGuiStyle& Style = ImGui::GetStyle();

                            Container MessageDetailsContainer = {};
                            MessageDetailsContainer.ID = "MessageDetailsContainer";
                            MessageDetailsContainer.Size = Vector2(MESSAGE_CONTAINER_AVAILABLE_SPACE.X - MessageSenderImage.Size.X, 0.0f);
                            MessageDetailsContainer.Padding = Vector2(Style.WindowPadding.x / 1.5f, 0.0f);
                            // NOTE: Transparent background
                            MessageDetailsContainer.BgColor = Rgba(0, 0, 0, 0);
                            MessageDetailsContainer.IsAutoResizableY = true;
                            MessageDetailsContainer.DrawContent = [&ClientGui, &MESSAGE]() {
                                // MESSAGE SENDER FIRSTNAME TEXT
                                Text MessageSenderFirstNameText = {};
                                MessageSenderFirstNameText.Value = MESSAGE.SenderFirstName;
                                ClientGui.DrawText(MessageSenderFirstNameText);

                                // MESSAGE CREATED AT TEXT
                                std::tm* MessageCreatedAtDate = std::localtime(&MESSAGE.CreatedAt);
                                const std::string& MESSAGE_CREATED_AT_STRING_DATE = asctime(MessageCreatedAtDate);

                                Text MessageCreatedAtText = {};
                                MessageCreatedAtText.Value = MESSAGE_CREATED_AT_STRING_DATE;
                                ClientGui.DisplayInline();
                                ClientGui.DrawText(MessageCreatedAtText);

                                // MESSAGE TEXT
                                Text MessageText = {};
                                MessageText.Value = MESSAGE.Text;
                                ClientGui.DrawTextWrapped(MessageText);

                            };

                            ClientGui.SetPositionX(MessageSenderImage.Size.X + 10.0f);
                            ClientGui.DrawContainer(MessageDetailsContainer);

                        };

                        ClientGui.DrawContainer(MessageContainer);
                    }

                    // Before drawing content, check if we are already at the bottom
                    const bool IsAtBottom = ClientGui.GetScrollPositionY() >= ClientGui.GetMaxScrollPositionY();
                    // Auto-scroll logic: only scroll if the user hasn't moved away from the bottom
                    if (IsAtBottom)
                    {
                        // Scrolls to the end
                        ClientGui.ScrollToY(1.0f);
                    }
                };

                ClientGui.DrawContainer(MessagesContainer);
            };

            ClientGui.DisplayInline();
            ClientGui.SetPositionX(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.25f);
            ClientGui.DrawContainer(SelectedConversationContainer);

            // MESSAGE TEXT INPUT CONTAINER
            static std::string MessageText = "";

            Container MessageTextInputContainer = {};
            MessageTextInputContainer.ID = "TextInputContainer";
            MessageTextInputContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.60f, MAIN_WINDOW_AVAILABLE_SPACE.Y * 0.15f);
            MessageTextInputContainer.Padding = Vector2(15.0f, 15.0f);
            // NOTE: Transparent background
            MessageTextInputContainer.BgColor = Rgba(0, 0, 0, 0);
            MessageTextInputContainer.DrawContent = [&ClientGui]() {
                const Vector2 MESSAGE_TEXTINPUT_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                // MESSAGE TEXT INPUT
                TextInput MessageTextInput = {};
                MessageTextInput.ID = "MessageTextInput";
                MessageTextInput.Placeholder = "Enter message here...";
                MessageTextInput.Size = Vector2(MESSAGE_TEXTINPUT_CONTAINER_AVAILABLE_SPACE);
                MessageTextInput.Padding = Vector2(15.0f, 15.0f);
                MessageTextInput.CornerRounding = 10.f;
                MessageTextInput.BgColor = Rgba(43, 50, 94, 255);
                MessageTextInput.PlaceholderColor = Rgba(120, 125, 172, 255);

                ClientGui.DrawTextInputMultiline(MessageText, MessageTextInput);
            };

            ClientGui.SetPositionX(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.25f);
            ClientGui.SetPositionY(MAIN_WINDOW_AVAILABLE_SPACE.Y * 0.85f);
            ClientGui.DrawContainer(MessageTextInputContainer);

            // SEND BUTTON CONTAINER
            Container SendButtonContainer = {};
            SendButtonContainer.ID = "SendButtonContainer";
            SendButtonContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.15f, MAIN_WINDOW_AVAILABLE_SPACE.Y * 0.15f);
            SendButtonContainer.Padding = Vector2(15.0f, 15.0f);
            // NOTE: Transparent background
            SendButtonContainer.BgColor = Rgba(0, 0, 0, 0);
            SendButtonContainer.DrawContent = [&ClientGui]() {
                const Vector2 SEND_BUTTON_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                // SEND BUTTON
                Button SendButton = {};
                SendButton.Label = "Send";
                SendButton.Size = Vector2(SEND_BUTTON_CONTAINER_AVAILABLE_SPACE.X * 0.70f, SEND_BUTTON_CONTAINER_AVAILABLE_SPACE.Y * 0.40f);
                SendButton.BgColor = Rgba(200, 30, 30, 255); // Red button
                SendButton.BgColorActive = Rgba(150, 0, 0, 255); // Darker red when active
                SendButton.BgColorHovered = Rgba(255, 100, 100, 255); // Lighter red on hover
                SendButton.CornerRounding = 10.0f;
                SendButton.IsDisabled  = MessageText.empty();
                SendButton.OnClick = []() {
                    Message NewMessage = {};
                    NewMessage.ID = "FakeID";
                    NewMessage.ConversationID = SelectedConversation->ID;
                    NewMessage.SenderID = "FakeSenderID";
                    NewMessage.SenderFirstName = "Olivier";
                    NewMessage.SenderImageUrl = "https://fakeimageurl.com";
                    NewMessage.Text = MessageText;
                    NewMessage.CreatedAt = std::time(0);

                    // TODO: Server call to persist message will go there

                    SelectedConversation->Messages.push_back(NewMessage);
                    std::cout << "SENT: " << MessageText << std::endl;
                };

                ClientGui.AlignCenter(SendButton.Size);
                ClientGui.DrawButton(SendButton);
            };

            ClientGui.DisplayInline();
            ClientGui.SetPositionX(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.85f);
            ClientGui.DrawContainer(SendButtonContainer);
        };

        ClientGui.DrawWindow(MainWindow);

        // Rendering
        ClientGui.Render();

        glfwSwapBuffers(GlfwWindow);
        glfwPollEvents();
    }

    BlankImageTexture.Destroy();
    ClientGui.Destroy();

    glfwDestroyWindow(GlfwWindow);
    glfwTerminate();

    return 0;
}
