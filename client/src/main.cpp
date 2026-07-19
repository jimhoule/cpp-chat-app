#include "auth/AuthApi.h"
#include "debug/DebugLogger.h"
#include "debug/DebugOverlay.h"
#include "deserializer/LoggedinSocketEventPayloadDeserializer.h"
#include "deserializer/RegisteredSocketEventPayloadDeserializer.h"
#include "layer/LayerStack.h"
#include "layer/ChatLayer.h"
#include "layer/LoginLayer.h"
#include "layer/RegisterLayer.h"
#include "messages/MessagesApi.h"
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
    std::shared_ptr<SocketClient> socketClient = std::make_shared<SocketClient>();
    const int SERVER_PORT = 5000;
    socketClient->Connect(SERVER_PORT, "127.0.0.1");

    // Layer stack
    std::shared_ptr<LayerStack> layerStack = std::make_shared<LayerStack>();

    // Apis
    AuthApi authApi(socketClient);
    MessagesApi messagesApi(socketClient);

    // Gui
    Gui gui = {};
    gui.Init(GlfwWindow);

    // Debug overlay
    std::shared_ptr<DebugOverlay> debugOverlay =  std::make_shared<DebugOverlay>(gui, layerStack);

    // Chat layer
    std::shared_ptr<DebugLogger> chatLogger = std::make_shared<DebugLogger>("CHAT", "client/src/layer/ChatLayer", debugOverlay);
    std::shared_ptr<ChatLayer> Chat = std::make_shared<ChatLayer>("Chat", socketClient, gui, chatLogger);
    Chat->OnLogoutButtonClick = [&layerStack]() {
        layerStack->Pop();
        layerStack->Unsuspend("Login");
    };
    Chat->OnSendMessageButtonClick = [&messagesApi](const std::string& conversationID, const std::string& messageText) {
        CreateMessageParams createMessageParams = {};
        createMessageParams.conversationID = conversationID;
        createMessageParams.text = messageText;
        messagesApi.Create(createMessageParams);
    };

    // Register layer
    std::shared_ptr<DebugLogger> registerLogger = std::make_shared<DebugLogger>("REGISTER", "client/src/layer/RegisterLayer", debugOverlay);
    std::shared_ptr<RegisterLayer> Register = std::make_shared<RegisterLayer>("Register", gui, registerLogger);
    Register->OnLoginButtonClick = [&layerStack]() {
        layerStack->Pop();
        layerStack->Unsuspend("Login");
    };
    Register->OnRegisterButtonClick = [&authApi](const std::string& FirstName, const std::string& LastName, const std::string& Email, const std::string& Password) {
        RegisterParams RegisterParams = {};
        RegisterParams.FirstName = FirstName;
        RegisterParams.LastName = LastName;
        RegisterParams.Email = Email;
        RegisterParams.Password = Password;
        authApi.Register(RegisterParams);
    };

    // Login layer
    std::shared_ptr<DebugLogger> loginLogger = std::make_shared<DebugLogger>("LOGIN", "client/src/layer/LoginLayer", debugOverlay);
    std::shared_ptr<LoginLayer> Login = std::make_shared<LoginLayer>("Login", gui, loginLogger);
    Login->OnLoginButtonClick = [&authApi](const std::string& Email, const std::string& Password) {
        LoginParams LoginParams = {};
        LoginParams.Email = Email;
        LoginParams.Password = Password;
        authApi.Login(LoginParams);
    };
    Login->OnRegisterButtonClick = [&layerStack, &Register]() {
        layerStack->Push(Register);
        layerStack->Suspend("Login");
    };

    layerStack->Push(Login);

    // Socket client logged in event handlers
    LoggedinSocketEventPayloadDeserializer loggedinSocketEventPayloadDeserializer = {};
    SocketClientEventHandler HandleLoggedinSocketEvent = [&layerStack, &Chat, &loggedinSocketEventPayloadDeserializer](const std::string& SerializedLoggedinSocketEventPayload) {
        // Gets logged in socket event payload
        const LoggedinSocketEventPayload& loggedinSocketEventPayload = loggedinSocketEventPayloadDeserializer.Deserialize(SerializedLoggedinSocketEventPayload);
        std::cout << "Logged in access token: " << loggedinSocketEventPayload.AccessToken << std::endl;

        layerStack->Push(Chat);
        layerStack->Suspend("Login");
    };

    // Socket client registered event handlers
    RegisteredSocketEventPayloadDeserializer registeredSocketEventPayloadDeserializer = {};
    SocketClientEventHandler HandleRegisteredSocketEvent = [&layerStack, &Chat, &registeredSocketEventPayloadDeserializer](const std::string& SerializedRegisteredSocketEventPayload) {
        // Gets registered socket event payload
        const RegisteredSocketEventPayload& registeredSocketEventPayload = registeredSocketEventPayloadDeserializer.Deserialize(SerializedRegisteredSocketEventPayload);
        std::cout << "Registered access token: " << registeredSocketEventPayload.AccessToken << std::endl;

        layerStack->Pop();
        layerStack->Push(Chat);
    };

    socketClient->On(SocketEventName::LOGGEDIN, HandleLoggedinSocketEvent);
    socketClient->On(SocketEventName::REGISTERED, HandleRegisteredSocketEvent);

    while(!glfwWindowShouldClose(GlfwWindow))
    {
        socketClient->Read();

        glfwPollEvents();

        layerStack->Update();
        gui.Clear();

        // Clears screen
        glClearColor(250.0f / 255.0f, 119.0f / 255.0f, 110.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        layerStack->Render();
        debugOverlay->Render();
        gui.Render();

        glfwSwapBuffers(GlfwWindow);
    }

    socketClient->Close();

    layerStack->Clear();
    gui.Destroy();

    glfwDestroyWindow(GlfwWindow);
    glfwTerminate();

    return 0;
}
