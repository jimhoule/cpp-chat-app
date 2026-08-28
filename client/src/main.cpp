#include "auth/AuthApi.h"
#include "debug/DebugLogger.h"
#include "debug/DebugOverlay.h"
#include "deserializer/LoggedinSocketEventPayloadDeserializer.h"
#include "deserializer/RegisteredSocketEventPayloadDeserializer.h"
#include "layer/LayerStack.h"
#include "layer/ChatLayer.h"
#include "layer/LoginLayer.h"
#include "layer/RegisterLayer.h"
#include "log/Logger.h"
#include "messages/MessagesApi.h"
#include "navigation/Navigation.h"
#include "socket/SocketClient.h"

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <utility>

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
    const std::string windowTitle = "Login";

    GLFWwindow* glfwWindow = glfwCreateWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        windowTitle.c_str(),
        NULL,
        NULL
    );

    if(glfwWindow == nullptr)
    {
        std::cout << "Failed to open GLFW Window" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(glfwWindow);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Socket client
    SocketClient socketClient = {};
    const int SERVER_PORT = 5000;
    socketClient.Connect(SERVER_PORT, "127.0.0.1");

    // Gui
    Gui gui = {};
    gui.Init(glfwWindow);

    // Layer stack
    LayerStack layerStack = {};

    // Debug overlay
    DebugOverlay debugOverlay(gui, layerStack);

    // Loggers
    Logger authServiceLogger("AUTH", "client/src/auth/AuthApi");
    Logger messagesServiceLogger("MESSAGES", "client/src/auth/AuthApi");
    Logger navigationLogger("NAVIGATION", "client/src/navigation/Navigation");
    DebugLogger chatLogger("CHAT", "client/src/layer/ChatLayer", debugOverlay);
    DebugLogger registerLogger("REGISTER", "client/src/layer/RegisterLayer", debugOverlay);
    DebugLogger loginLogger("LOGIN", "client/src/layer/LoginLayer", debugOverlay);

    // Apis
    AuthApi authApi(socketClient, authServiceLogger);
    MessagesApi messagesApi(socketClient, messagesServiceLogger);

    // Navigation
    Navigation navigation(layerStack, navigationLogger);

    Navigation::Screen chatScreen = {};
    chatScreen.layer = std::make_unique<ChatLayer>("Chat", gui, navigation, authApi, messagesApi, chatLogger);
    Navigation::Screen loginScreen = {};
    loginScreen.layer = std::make_unique<LoginLayer>("Login", gui, navigation, authApi, loginLogger);
    Navigation::Screen registerScreen = {};
    registerScreen.layer = std::make_unique<RegisterLayer>("Register", gui, navigation, authApi, registerLogger);

    navigation.AddScreen(Navigation::ScreenName::CHAT, std::move(chatScreen));
    navigation.AddScreen(Navigation::ScreenName::LOGIN, std::move(loginScreen));
    navigation.AddScreen(Navigation::ScreenName::REGISTER, std::move(registerScreen));

    navigation.SetInitialScreen(Navigation::ScreenName::LOGIN);

    while(!glfwWindowShouldClose(glfwWindow))
    {
        socketClient.Read();

        glfwPollEvents();

        layerStack.Update();
        gui.Clear();

        // Clears screen
        glClearColor(250.0f / 255.0f, 119.0f / 255.0f, 110.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        layerStack.Render();
        debugOverlay.Render();
        gui.Render();

        glfwSwapBuffers(glfwWindow);
    }

    socketClient.Close();

    layerStack.Clear();
    gui.Destroy();

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();

    return 0;
}
