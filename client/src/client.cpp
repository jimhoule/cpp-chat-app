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
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

constexpr int SERVER_PORT =  5000;

struct Message {
    std::string ID;
    std::string conversation_id;
    std::string sender_id;
    std::string text;
    std::string created_at;
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

            // CONVERSATIONS CONTAINER
            Container ConversationsContainer = {};
            ConversationsContainer.ID = "ConversationsContainer";
            ConversationsContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X * 0.25f, MAIN_WINDOW_AVAILABLE_SPACE.Y - NavbarContainer.Size.Y);
            ConversationsContainer.Padding = Vector2(15.0f, 15.0f);
            // NOTE: Transparent background
            ConversationsContainer.BgColor = Rgba(0, 0, 0, 0);
            ConversationsContainer.DrawContent = [&ClientGui]() {
                const Vector2 CONVERSATIONS_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                // DIRECT MESSAGES CONTAINER
                Container DirectMessagesContainer = {};
                DirectMessagesContainer.ID = "DirectMessagessContainer";
                DirectMessagesContainer.Size = Vector2(CONVERSATIONS_CONTAINER_AVAILABLE_SPACE);
                DirectMessagesContainer.CornerRounding = 10.f;
                DirectMessagesContainer.BgColor = Rgba(50, 56, 102, 255);
                DirectMessagesContainer.DrawContent = [&ClientGui]() {
                    const Vector2 DIRECT_MESSAGES_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                    TreeNode direct_messages_root_node = {};
                    direct_messages_root_node.Name = "Direct Messages";
                    direct_messages_root_node.Children = {
                        { "Direct Message 1" },
                        { "Direct Message 2" },
                        { "Direct Message 3" },
                    };

                    ClientGui.DrawTreeNode(direct_messages_root_node);
                };

                ClientGui.DrawContainer(DirectMessagesContainer);
            };

            ClientGui.DrawContainer(ConversationsContainer);

            // SELECTED CONVERSATION CONTAINER
            static std::vector<std::string> Messages;

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

                    for (int i = 0; i < Messages.size(); i++)
                    {
                        // MESSAGE CONTAINER
                        const std::string& MESSAGE = Messages[i];
                        const std::string& ID = "MessageContainer" + std::to_string(i);

                        Container MessageContainer = {};
                        MessageContainer.ID = ID;
                        MessageContainer.Size = Vector2(MESSAGES_CONTAINER_AVAILABLE_SPACE.X, MESSAGES_CONTAINER_AVAILABLE_SPACE.Y * 0.10f);
                        MessageContainer.BgColor = Rgba(200, 200, 0, 1);
                        MessageContainer.DrawContent = [&ClientGui, &BlankImageTexture, &MESSAGE]() {
                            const Vector2 MESSAGE_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                            // NOTE: Images are drawn directly over elements so anything that needs to go beside will have to be postioned manually
                            Image MessageSenderImage = {};
                            MessageSenderImage.TextureId = BlankImageTexture.GetID();
                            MessageSenderImage.Size = Vector2(MESSAGE_CONTAINER_AVAILABLE_SPACE.Y, MESSAGE_CONTAINER_AVAILABLE_SPACE.Y);
                            MessageSenderImage.CornerRounding = 10.0f;
                            ClientGui.DrawImage(MessageSenderImage);

                            Text MessageText = {};
                            MessageText.Value = MESSAGE;
                            ClientGui.SetPositionX(MessageSenderImage.Size.X);
                            ClientGui.DrawText(MessageText);
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
            static std::string Message = "";

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

                ClientGui.DrawTextInputMultiline(Message, MessageTextInput);
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
                SendButton.Color = Rgba(200, 30, 30, 1); // Red button
                SendButton.ColorActive = Rgba(150, 0, 0, 1); // Darker red when active
                SendButton.ColorHovered = Rgba(255, 100, 100, 1); // Lighter red on hover
                SendButton.CornerRounding = 10.0f;
                SendButton.IsDisabled  = Message.empty();
                SendButton.OnClick = []() {
                    Messages.push_back(Message);
                    std::cout << "SENT: " << Message << std::endl;
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
