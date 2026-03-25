#include "api/AuthApi.h"
#include "debug/DebugLogger.h"
#include "debug/DebugOverlay.h"
#include "deserializer/LoggedinSocketEventPayloadDeserializer.h"
#include "deserializer/RegisteredSocketEventPayloadDeserializer.h"
#include "layer/LayerStack.h"
#include "layer/ChatLayer.h"
#include "layer/LoginLayer.h"
#include "layer/RegisterLayer.h"
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
    const std::string& WindowTitle = "Login";

    GLFWwindow* GlfwWindow = glfwCreateWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        WindowTitle.c_str(),
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

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Socket client
    SocketClient SocketClient = {};
    const int SERVER_PORT = 5000;
    SocketClient.Connect(SERVER_PORT, "127.0.0.1");

    // Layer stack
    std::shared_ptr<LayerStack> AppLayerStack = std::make_shared<LayerStack>();

    // Apis
    AuthApi AppAuthApi(SocketClient);

    // Gui
    Gui AppGui = {};
    AppGui.Init(GlfwWindow);

    // Debug overlay
    std::shared_ptr<DebugOverlay> AppDebugOverlay =  std::make_shared<DebugOverlay>(AppGui, AppLayerStack);

    // Chat layer
    std::shared_ptr<DebugLogger> ChatLogger = std::make_shared<DebugLogger>("CHAT", "client/src/layer/ChatLayer", AppDebugOverlay);
    std::shared_ptr<ChatLayer> Chat = std::make_shared<ChatLayer>("Chat", SocketClient, AppGui, ChatLogger);
    Chat->OnLogoutButtonClick = [&AppLayerStack]() {
        AppLayerStack->Pop();
        AppLayerStack->Unsuspend("Login");
    };

    // Register layer
    std::shared_ptr<DebugLogger> RegisterLogger = std::make_shared<DebugLogger>("REGISTER", "client/src/layer/RegisterLayer", AppDebugOverlay);
    std::shared_ptr<RegisterLayer> Register = std::make_shared<RegisterLayer>("Register", AppGui, RegisterLogger);
    Register->OnLoginButtonClick = [&AppLayerStack]() {
        AppLayerStack->Pop();
        AppLayerStack->Unsuspend("Login");
    };
    Register->OnRegisterButtonClick = [&AppAuthApi](const std::string& FirstName, const std::string& LastName, const std::string& Email, const std::string& Password) {
        RegisterParams RegisterParams = {};
        RegisterParams.FirstName = FirstName;
        RegisterParams.LastName = LastName;
        RegisterParams.Email = Email;
        RegisterParams.Password = Password;
        AppAuthApi.Register(RegisterParams);
    };

    // Login layer
    std::shared_ptr<DebugLogger> LoginLogger = std::make_shared<DebugLogger>("LOGIN", "client/src/layer/LoginLayer", AppDebugOverlay);
    std::shared_ptr<LoginLayer> Login = std::make_shared<LoginLayer>("Login", AppGui, LoginLogger);
    Login->OnLoginButtonClick = [&AppAuthApi](const std::string& Email, const std::string& Password) {
        LoginParams LoginParams = {};
        LoginParams.Email = Email;
        LoginParams.Password = Password;
        AppAuthApi.Login(LoginParams);
    };
    Login->OnRegisterButtonClick = [&AppLayerStack, &Register]() {
        AppLayerStack->Push(Register);
        AppLayerStack->Suspend("Login");
    };

    AppLayerStack->Push(Login);

    // Socket client logged in event handlers
    LoggedinSocketEventPayloadDeserializer LoggedinSocketEventPayloadDeserializer = {};
    SocketClientEventHandler HandleLoggedinSocketEvent = [&AppLayerStack, &Chat, &LoggedinSocketEventPayloadDeserializer](const std::string& SerializedLoggedinSocketEventPayload) {
        // Gets logged in socket event payload
        const LoggedinSocketEventPayload& LoggedinSocketEventPayload = LoggedinSocketEventPayloadDeserializer.Deserialize(SerializedLoggedinSocketEventPayload);
        std::cout << "Logged in access token: " << LoggedinSocketEventPayload.AccessToken << std::endl;

        AppLayerStack->Push(Chat);
        AppLayerStack->Suspend("Login");
    };

    // Socket client registered event handlers
    RegisteredSocketEventPayloadDeserializer RegisteredSocketEventPayloadDeserializer = {};
    SocketClientEventHandler HandleRegisteredSocketEvent = [&AppLayerStack, &Chat, &RegisteredSocketEventPayloadDeserializer](const std::string& SerializedRegisteredSocketEventPayload) {
        // Gets registered socket event payload
        const RegisteredSocketEventPayload& RegisteredSocketEventPayload = RegisteredSocketEventPayloadDeserializer.Deserialize(SerializedRegisteredSocketEventPayload);
        std::cout << "Registered access token: " << RegisteredSocketEventPayload.AccessToken << std::endl;

        AppLayerStack->Pop();
        AppLayerStack->Push(Chat);
    };

    SocketClient.On(SocketEventName::LOGGEDIN, HandleLoggedinSocketEvent);
    SocketClient.On(SocketEventName::REGISTERED, HandleRegisteredSocketEvent);

    while(!glfwWindowShouldClose(GlfwWindow))
    {
        SocketClient.Read();

        glfwPollEvents();

        AppLayerStack->Update();
        AppGui.Clear();

        // Clears screen
        glClearColor(250.0f / 255.0f, 119.0f / 255.0f, 110.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        AppLayerStack->Render();
        AppDebugOverlay->Render();
        AppGui.Render();

        glfwSwapBuffers(GlfwWindow);
    }

    SocketClient.Close();

    AppLayerStack->Clear();
    AppGui.Destroy();

    glfwDestroyWindow(GlfwWindow);
    glfwTerminate();

    return 0;
}
