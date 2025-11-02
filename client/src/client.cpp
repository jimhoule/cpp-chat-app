#include "Gui.h"
#include "SocketClient.h"

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
    std::string id;
    std::string conversation_id;
    std::string sender_id;
    std::string text;
    std::string created_at;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void myKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac

    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    const std::string& window_title = "Chat Client";

    GLFWwindow* window = glfwCreateWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        window_title.c_str(),
        NULL,
        NULL
    );

    if(window == nullptr)
    {
        std::cout << "Failed to open GLFW window" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //Register Key Callback -> glfwPollEvents
    glfwSetKeyCallback(window, myKeyCallback);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Gui gui;
    gui.Init(window);

    // SocketClient socket_client;
    // socket_client.Connect(SERVER_PORT, "127.0.0.1");

    // sleep(1);

    // std::string message = "Hello from Client Socket!";
    // socket_client.Send(message);
    // socket_client.Close();

    while(!glfwWindowShouldClose(window))
    {
        // Clears screen
        glClearColor(250.0f / 255.0f, 119.0f / 255.0f, 110.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

         // Clears ImGui state
        gui.Clear();

        // WINDOW
        Window main_window = {};
        main_window.name = "MainWindow";
        main_window.size = Vector2(WINDOW_WIDTH, WINDOW_HEIGHT);
        main_window.bg_color = Rgba(26, 30, 67, 255).ToVector4();
        main_window.DrawContent = [&gui]() {
            const Vector2 MAIN_WINDOW_AVAILABLE_SPACE = gui.GetAvailableSpace();

            // NAVBAR CONTAINER
            Container navbar_container = {};
            navbar_container.id = "NavbarContainer";
            navbar_container.size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.x, MAIN_WINDOW_AVAILABLE_SPACE.y * 0.15f);
            navbar_container.padding = Vector2(15.0f, 15.0f);
            // NOTE: Transparent background
            navbar_container.bg_color = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
            navbar_container.DrawContent = [&gui]() {
                const Vector2 NAVBAR_CONTAINER_AVAILABLE_SPACE = gui.GetAvailableSpace();

                // NAVBAR
                Container navbar = {};
                navbar.id = "Navbar";
                navbar.size = Vector2(NAVBAR_CONTAINER_AVAILABLE_SPACE);
                navbar.corner_rounding = 10.f;
                navbar.bg_color = Rgba(50, 56, 102, 255).ToVector4();
                navbar.DrawContent = [&gui]() {};

                gui.DrawContainer(navbar);
            };

            gui.DrawContainer(navbar_container);

            // CONVERSATIONS CONTAINER
            Container conversations_container = {};
            conversations_container.id = "ConversationsContainer";
            conversations_container.size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.x * 0.25f, MAIN_WINDOW_AVAILABLE_SPACE.y - navbar_container.size.y);
            conversations_container.padding = Vector2(15.0f, 15.0f);
            // NOTE: Transparent background
            conversations_container.bg_color = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
            conversations_container.DrawContent = [&gui]() {
                const Vector2 CONVERSATIONS_CONTAINER_AVAILABLE_SPACE = gui.GetAvailableSpace();

                // DIRECT MESSAGES CONTAINER
                Container direct_messages_container = {};
                direct_messages_container.id = "DirectMessagessContainer";
                direct_messages_container.size = Vector2(CONVERSATIONS_CONTAINER_AVAILABLE_SPACE);
                direct_messages_container.corner_rounding = 10.f;
                direct_messages_container.bg_color = Rgba(50, 56, 102, 255).ToVector4();
                direct_messages_container.DrawContent = [&gui]() {
                    const Vector2 DIRECT_MESSAGES_CONTAINER_AVAILABLE_SPACE = gui.GetAvailableSpace();

                    TreeNode direct_messages_root_node = {};
                    direct_messages_root_node.name = "Direct Messages";
                    direct_messages_root_node.children = {
                        { "Direct Message 1" },
                        { "Direct Message 2" },
                        { "Direct Message 3" },
                    };

                    gui.DrawTreeNode(direct_messages_root_node);
                };

                gui.DrawContainer(direct_messages_container);
            };

            gui.DrawContainer(conversations_container);

            // SELECTED CONVERSATION CONTAINER
            gui.DisplayInline();
            gui.SetPositionX(MAIN_WINDOW_AVAILABLE_SPACE.x * 0.25f);

            static std::vector<std::string> messages;

            Container selected_conversation_container = {};
            selected_conversation_container.id = "SelectedConversationContainer";
            selected_conversation_container.size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.x * 0.75f, MAIN_WINDOW_AVAILABLE_SPACE.y * 0.70f);
            selected_conversation_container.padding = Vector2(15.0f, 15.0f);
            // NOTE: Transparent background
            selected_conversation_container.bg_color = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
            selected_conversation_container.DrawContent = [&gui]() {
                const Vector2 SELECTED_CONVERSATION_CONTAINER_AVAILABLE_SPACE = gui.GetAvailableSpace();

                // MESSSAGES CONTAINER
                Container messages_container = {};
                messages_container.id = "MessagesContainer";
                messages_container.size = Vector2(SELECTED_CONVERSATION_CONTAINER_AVAILABLE_SPACE);
                messages_container.corner_rounding = 10.f;
                messages_container.bg_color = Rgba(50, 56, 102, 255).ToVector4();
                messages_container.DrawContent = [&gui]() {
                    const Vector2 MESSAGES_CONTAINER_AVAILABLE_SPACE = gui.GetAvailableSpace();

                    for (int i = 0; i < messages.size(); i++)
                    {
                        // MESSAGE CONTAINER
                        const std::string& MESSAGE = messages[i];
                        const std::string& ID = "MessageContainer" + std::to_string(i);

                        Container message_container = {};
                        message_container.id = ID;
                        message_container.size = Vector2(MESSAGES_CONTAINER_AVAILABLE_SPACE.x, MESSAGES_CONTAINER_AVAILABLE_SPACE.y * 0.10f);
                        message_container.bg_color = Vector4(0.8f, 0.8f, 0.0f, 1.0f);
                        message_container.DrawContent = [&gui, &MESSAGE]() {
                            Text message_text = {};
                            message_text.value = MESSAGE;

                            gui.DrawText(message_text);
                        };

                        gui.DrawContainer(message_container);
                    }

                    // Before drawing content, check if we are already at the bottom
                    const bool is_at_bottom = gui.GetScrollPositionY() >= gui.GetMaxScrollPositionY();
                    // Auto-scroll logic: only scroll if the user hasn't moved away from the bottom
                    if (is_at_bottom)
                    {
                        // Scrolls to the end
                        gui.ScrollToY(1.0f);
                    }
                };

                gui.DrawContainer(messages_container);
            };

            gui.DrawContainer(selected_conversation_container);

            // MESSAGE TEXT INPUT CONTAINER
            gui.SetPositionX(MAIN_WINDOW_AVAILABLE_SPACE.x * 0.25f);
            gui.SetPositionY(MAIN_WINDOW_AVAILABLE_SPACE.y * 0.85f);

            static std::string message = "";

            Container message_textinput_container = {};
            message_textinput_container.id = "TextInputContainer";
            message_textinput_container.size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.x * 0.60f, MAIN_WINDOW_AVAILABLE_SPACE.y * 0.15f);
            message_textinput_container.padding = Vector2(15.0f, 15.0f);
            // NOTE: Transparent background
            message_textinput_container.bg_color = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
            message_textinput_container.DrawContent = [&gui]() {
                const Vector2 MESSAGE_TEXTINPUT_CONTAINER_AVAILABLE_SPACE = gui.GetAvailableSpace();

                // MESSAGE TEXT INPUT
                TextInput message_textinput = {};
                message_textinput.id = "MessageTextInput";
                message_textinput.placeholder = "Enter message here...";
                message_textinput.size = Vector2(MESSAGE_TEXTINPUT_CONTAINER_AVAILABLE_SPACE);
                message_textinput.padding = Vector2(15.0f, 15.0f);
                message_textinput.corner_rounding = 10.f;
                message_textinput.bg_color = Rgba(43, 50, 94, 255).ToVector4();
                message_textinput.placeholder_color = Rgba(120, 125, 172, 255);

                gui.DrawTextInputMultiline(message, message_textinput);
            };

            gui.DrawContainer(message_textinput_container);

            // SEND BUTTON CONTAINER
            gui.DisplayInline();
            gui.SetPositionX(MAIN_WINDOW_AVAILABLE_SPACE.x * 0.85f);

            Container send_button_container = {};
            send_button_container.id = "SendButtonContainer";
            send_button_container.size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.x * 0.15f, MAIN_WINDOW_AVAILABLE_SPACE.y * 0.15f);
            send_button_container.padding = Vector2(15.0f, 15.0f);
            // NOTE: Transparent background
            send_button_container.bg_color = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
            send_button_container.DrawContent = [&gui]() {
                const Vector2 SEND_BUTTON_CONTAINER_AVAILABLE_SPACE = gui.GetAvailableSpace();

                // SEND BUTTON
                const Vector2 SEND_BUTTON_SIZE = Vector2(SEND_BUTTON_CONTAINER_AVAILABLE_SPACE.x * 0.70f, SEND_BUTTON_CONTAINER_AVAILABLE_SPACE.y * 0.40f);
                gui.AlignCenter(SEND_BUTTON_SIZE);

                Button send_button = {};
                send_button.label = "Send";
                send_button.size = SEND_BUTTON_SIZE;
                send_button.color = Vector4(0.8f, 0.2f, 0.2f, 1.0f); // Red button
                send_button.color_active = Vector4(0.6f, 0.0f, 0.0f, 1.0f); // Darker red when active
                send_button.color_hovered = Vector4(1.0f, 0.4f, 0.4f, 1.0f); // Lighter red on hover
                send_button.corner_rounding = 10.0f;
                send_button.is_disabled  = message.empty();
                send_button.OnClick = []() {
                    messages.push_back(message);
                    std::cout << "SENT: " << message << std::endl;
                };

                gui.DrawButton(send_button);
            };

            gui.DrawContainer(send_button_container);
        };

        gui.DrawWindow(main_window);

        // Rendering
        gui.Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // imgui cleanup
    gui.Destroy();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
