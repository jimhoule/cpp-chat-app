#include "api/AuthApi.h"
#include "DebugOverlay.h"
#include "layer/LayerStack.h"
#include "layer/ChatLayer.h"
#include "layer/LoginLayer.h"
#include "layer/RegisterLayer.h"
#include "SocketClient.h"

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
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


    std::shared_ptr<LayerStack> AppLayerStack = std::make_shared<LayerStack>();

    // Apis
    AuthApi AppAuthApi;

    // Gui
    Gui AppGui = {};
    AppGui.Init(GlfwWindow);

    // Debug overlay
    DebugOverlay AppDebugOverlay(AppGui, AppLayerStack);

    // Chat layer
    std::shared_ptr<ChatLayer> Chat = std::make_shared<ChatLayer>("Chat", AppGui);
     Chat->OnLogoutButtonClick = [&AppLayerStack]() {
        AppLayerStack->Pop();
        AppLayerStack->Unsuspend("Login");
    };

    // Register layer
    std::shared_ptr<RegisterLayer> Register = std::make_shared<RegisterLayer>("Register", AppGui);
     Register->OnLoginButtonClick = [&AppLayerStack]() {
        AppLayerStack->Pop();
        AppLayerStack->Unsuspend("Login");
    };
    Register->OnRegisterButtonClick = [&AppAuthApi, &AppLayerStack, &Chat](const std::string& FirstName, const std::string& LastName, const std::string& Email, const std::string& Password) {
        RegisterParams RegisterParams = {};
        RegisterParams.FirstName = FirstName;
        RegisterParams.LastName = LastName;
        RegisterParams.Email = Email;
        RegisterParams.Password = Password;
        AppAuthApi.Register(RegisterParams);

        AppLayerStack->Pop();
        AppLayerStack->Push(Chat);
    };

    // Login layer
    std::shared_ptr<LoginLayer> Login = std::make_shared<LoginLayer>("Login", AppGui);
    Login->OnLoginButtonClick = [&AppAuthApi, &AppLayerStack, &Chat](const std::string& Email, const std::string& Password) {
        LoginParams LoginParams = {};
        LoginParams.Email = Email;
        LoginParams.Password = Password;
        AppAuthApi.Login(LoginParams);

        AppLayerStack->Push(Chat);
        AppLayerStack->Suspend("Login");
    };
    Login->OnRegisterButtonClick = [&AppLayerStack, &Register]() {
        AppLayerStack->Push(Register);
        AppLayerStack->Suspend("Login");
    };

    AppLayerStack->Push(Login);

    // SocketClient socket_client;
    // socket_client.Connect(SERVER_PORT, "127.0.0.1");

    // sleep(1);

    // std::string message = "Hello from Client Socket!";
    // socket_client.Send(message);
    // socket_client.Close();

    while(!glfwWindowShouldClose(GlfwWindow))
    {
        glfwPollEvents();

        AppLayerStack->Update();
        AppGui.Clear();

        // Clears screen
        glClearColor(250.0f / 255.0f, 119.0f / 255.0f, 110.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        AppLayerStack->Render();
        AppDebugOverlay.Render();
        AppGui.Render();

        glfwSwapBuffers(GlfwWindow);
    }

    AppLayerStack->Clear();
    AppGui.Destroy();

    glfwDestroyWindow(GlfwWindow);
    glfwTerminate();

    return 0;
}
