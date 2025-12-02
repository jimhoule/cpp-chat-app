#include "Gui.h"
#include "SocketClient.h"
#include "Texture.h"

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <algorithm>
#include <arpa/inet.h>
#include <ctime>
#include <cctype>
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
    std::string LastName;
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
    BlankImageTexture.Load("../../assets/Blank.jpg", 0);
    BlankImageTexture.Bind();

    Texture ClosableImageTexture = {};
    ClosableImageTexture.Load("../../assets/Closable.png", 1);
    ClosableImageTexture.Bind();

    Texture WhiteLeftArrowImageTexture = {};
    WhiteLeftArrowImageTexture.Load("../../assets/WhiteLeftArrow.png", 1);
    WhiteLeftArrowImageTexture.Bind();

    Gui ClientGui = {};
    ClientGui.Init(GlfwWindow);

    // Fake Users
    User CurrentUser = {};
    CurrentUser.ID = "CurrentUser";
    CurrentUser.FirstName = "Olivier";
    CurrentUser.LastName = "Perron";
    CurrentUser.ImageUrl = "http://fake.iamge.url";

    User User1 = {};
    User1.ID = "User1";
    User1.FirstName = "Marc";
    User1.LastName = "Bum";
    User1.ImageUrl = "http://fake.iamge.url";

    User User2 = {};
    User2.ID = "User2";
    User2.FirstName = "Simon";
    User2.LastName = "Robichaud";
    User2.ImageUrl = "http://fake.iamge.url";

    static std::vector<std::shared_ptr<User>> Users = {
        std::make_shared<User>(CurrentUser),
        std::make_shared<User>(User1),
        std::make_shared<User>(User2),
    };

    // Fake conversations
    Conversation Conversation1 = {};
    Conversation1.ID = "Conversation1";
    Conversation1.Users = { CurrentUser, User1 };
    Conversation1.Messages = {};
    Conversation1.CreatedAt = std::time(0);

    Conversation Conversation2 = {};
    Conversation2.ID = "Conversation2";
    Conversation2.Users = { CurrentUser, User2 };
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
        glfwPollEvents();

         // Clears ImGui state
        ClientGui.Clear();

        // DEBUG WINDOW
        static bool IsOpen = false;
        if (ImGui::IsKeyPressed(ImGuiKey_D)) IsOpen = !IsOpen;
        if (IsOpen) ImGui::ShowMetricsWindow(&IsOpen);

        // WINDOW
        Window MainWindow = {};
        MainWindow.Name = "MainWindow";
        MainWindow.Size = Vector2(WINDOW_WIDTH, WINDOW_HEIGHT);
        MainWindow.BgColor = Rgba(26, 30, 67, 255);
        MainWindow.DrawContent = [&ClientGui, &BlankImageTexture, &ClosableImageTexture, &WhiteLeftArrowImageTexture]() {
            const Vector2 MAIN_WINDOW_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

            // NAVBAR CONTAINER
            Container NavbarContainer = {};
            NavbarContainer.ID = "NavbarContainer";
            NavbarContainer.Size = Vector2(MAIN_WINDOW_AVAILABLE_SPACE.X, MAIN_WINDOW_AVAILABLE_SPACE.Y * 0.15f);
            NavbarContainer.Padding = Vector2(15.0f, 15.0f);
            // NOTE: Transparent background
            NavbarContainer.BgColor = Rgba(0, 0, 0, 0);
            NavbarContainer.DrawContent = [&ClientGui, &BlankImageTexture, &WhiteLeftArrowImageTexture](const ContainerState& State) {
                const Vector2 NAVBAR_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                // NAVBAR
                Container Navbar = {};
                Navbar.ID = "Navbar";
                Navbar.Size = Vector2(NAVBAR_CONTAINER_AVAILABLE_SPACE);
                Navbar.CornerRounding = 10.f;
                Navbar.BgColor = Rgba(50, 56, 102, 255);
                Navbar.DrawContent = [&ClientGui, &BlankImageTexture, &WhiteLeftArrowImageTexture](const ContainerState& State) {
                    const Vector2 NAVBAR_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                    // SEARCH TEXT INPUT CONTAINER
                    static std::string SearchText = "";

                    Container SearchTextInputContainer = {};
                    SearchTextInputContainer.ID = "SearchTextInputContainer";
                    SearchTextInputContainer.Size = Vector2(NAVBAR_AVAILABLE_SPACE.X * 0.50f, NAVBAR_AVAILABLE_SPACE.Y);
                    SearchTextInputContainer.CornerRounding = 10.0f;
                    // NOTE: Transparent background
                    SearchTextInputContainer.BgColor = Rgba(0, 0, 0, 0);
                    SearchTextInputContainer.DrawContent = [&ClientGui, &BlankImageTexture, &WhiteLeftArrowImageTexture](const ContainerState& State) {
                        const Vector2 SEARCH_TEXTINPUT_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                        // SEARCH MODAL
                        Modal SearchModal = {};
                        SearchModal.ID = "SearchModal";
                        SearchModal.Size = ClientGui.GetViewportSize();

                        Container SearchModalHeaderContainer = {};
                        SearchModalHeaderContainer.ID = "SearchModalHeaderContainer";
                        SearchModalHeaderContainer.Size = Vector2(SearchModal.Size.X , 0.0f);
                        // NOTE: Transparent background
                        SearchModalHeaderContainer.BgColor = Rgba(0, 0, 0, 0);
                        SearchModalHeaderContainer.CornerRounding = SearchModal.CornerRounding;
                        SearchModalHeaderContainer.IsAutoResizableY = true;
                        SearchModalHeaderContainer.DrawContent = [&ClientGui, &SearchModal, &WhiteLeftArrowImageTexture](const ContainerState& State) {
                            const Vector2 SEARCH_MODAL_HEADER_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                            Container SearchModalCloseImageButtonContainer = {};
                            SearchModalCloseImageButtonContainer.ID = "SearchModalCloseImageButtonContainer";
                            SearchModalCloseImageButtonContainer.Size = Vector2(
                                SEARCH_MODAL_HEADER_CONTAINER_AVAILABLE_SPACE.Y ,
                                SEARCH_MODAL_HEADER_CONTAINER_AVAILABLE_SPACE.Y
                            );
                            SearchModalCloseImageButtonContainer.Padding = Vector2(10.0f, 10.0f);
                            // NOTE: Transparent background
                            SearchModalCloseImageButtonContainer.BgColor = Rgba(0, 0, 0, 0);
                            //SearchModalCloseImageButtonContainer.CornerRounding = SearchModal.CornerRounding;
                            SearchModalCloseImageButtonContainer.DrawContent = [&ClientGui, &SearchModal, &WhiteLeftArrowImageTexture](const ContainerState& State) {
                                Image SearchModalCloseImageButtonImage = {};
                                SearchModalCloseImageButtonImage.TextureID = WhiteLeftArrowImageTexture.GetID();
                                SearchModalCloseImageButtonImage.Size = ClientGui.GetAvailableSpace();
                                SearchModalCloseImageButtonImage.TintColor = Rgba(255, 255, 255, 255);

                                ImageButton SearchModalCloseImageButton = {};
                                SearchModalCloseImageButton.ID = "SearchModalCloseImageButton";
                                SearchModalCloseImageButton.Image = SearchModalCloseImageButtonImage;
                                SearchModalCloseImageButton.TintColorHovered = Rgba(200, 200, 0, 255);
                                SearchModalCloseImageButton.OnClick = [&ClientGui, &SearchModal]() {
                                    ClientGui.CloseModal(SearchModal.ID);
                                };

                                ClientGui.DrawImageButton(SearchModalCloseImageButton);
                            };

                            ClientGui.DrawContainer(SearchModalCloseImageButtonContainer);

                            Placeholder SearchModalTextInputPlaceholder = {};
                            //SearchModalTextInputPlaceholder.Color = Rgba(120, 125, 172, 255);
                            SearchModalTextInputPlaceholder.Text = "Enter search here...";

                            TextInput SearchModalTextInput = {};
                            SearchModalTextInput.ID = "SearchModalTextInput";
                            SearchModalTextInput.Padding = Vector2(15.0f, 15.0f);
                            SearchModalTextInput.CornerRounding = 10.f;
                            //SearchModalTextInput.BgColor = Rgba(43, 50, 94, 255);
                            SearchModalTextInput.BgColor = Rgba(0, 0, 0, 0);
                            SearchModalTextInput.Placeholder  = SearchModalTextInputPlaceholder;

                            TextInputSingleline SearchModalTextInputSingleline = {};
                            SearchModalTextInputSingleline.TextInput = SearchModalTextInput;
                            SearchModalTextInputSingleline.Width = SEARCH_MODAL_HEADER_CONTAINER_AVAILABLE_SPACE.X - SEARCH_MODAL_HEADER_CONTAINER_AVAILABLE_SPACE.Y;

                            ClientGui.DisplayInline();
                            ClientGui.SetPositionX(SearchModalCloseImageButtonContainer.Size.X);
                            ClientGui.DrawTextInputSingleline(SearchText, SearchModalTextInputSingleline);

                            Divider BottomDivider = {};
                            BottomDivider.Color = Rgba(100, 100, 100, 255);

                            ClientGui.SetPositionY(SEARCH_MODAL_HEADER_CONTAINER_AVAILABLE_SPACE.Y - BottomDivider.Height);
                            ClientGui.DrawDivider(BottomDivider);

                        };

                        Container SearchModalBodyContainer = {};
                        SearchModalBodyContainer.ID = "SearchModalBodyContainer";
                        SearchModalBodyContainer.Size = Vector2(SearchModal.Size.X , 0.0f);
                        SearchModalBodyContainer.Padding = Vector2(15.0f, 15.0f);
                        // NOTE: Transparent background
                        SearchModalBodyContainer.BgColor = Rgba(0, 0, 0, 0);
                        SearchModalBodyContainer.CornerRounding = SearchModal.CornerRounding;
                        SearchModalBodyContainer.IsAutoResizableY = true;
                        SearchModalBodyContainer.DrawContent = [&ClientGui, &SearchModal, &BlankImageTexture](const ContainerState& State) {
                            if (SearchText.empty()) return;

                            const Vector2 SEARCH_MODAL_BODY_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                            std::function<std::vector<std::shared_ptr<User>>(std::string&)> Search = [](std::string& Value) {
                                // Defines a lambda function as the predicate for filtering
                                std::function<bool(std::shared_ptr<User>)> IsFound = [&Value](std::shared_ptr<User> User) {
                                    // Defines a lambda function for converting a char to a lowercase char
                                    std::function<int(unsigned char)> ConvertCharToLowercase = [&Value](unsigned char Character){
                                        return std::tolower(Character);
                                    };

                                    std::string& LowercaseFirstName = User->FirstName;
                                    std::transform(LowercaseFirstName.begin(), LowercaseFirstName.end(), LowercaseFirstName.begin(), ConvertCharToLowercase);

                                    std::string& LowercaseLastName = User->LastName;
                                    std::transform(LowercaseLastName.begin(), LowercaseLastName.end(), LowercaseLastName.begin(), ConvertCharToLowercase);

                                    // std::string& LowercaseValue = SearchText;
                                    std::string& LowercaseValue = Value;
                                    std::transform(LowercaseValue.begin(), LowercaseValue.end(), LowercaseValue.begin(), ConvertCharToLowercase);

                                    return (
                                        LowercaseFirstName.find(LowercaseValue) != std::string::npos ||
                                        LowercaseLastName.find(LowercaseValue) != std::string::npos
                                    );
                                };

                                // Copies elements that satisfy the predicate into the new vector
                                std::vector<std::shared_ptr<User>> FilteredUsers = {};
                                std::copy_if(Users.begin(), Users.end(), std::back_inserter(FilteredUsers), IsFound);

                                return std::move(FilteredUsers);
                            };

                            std::vector<std::shared_ptr<User>> Users = Search(SearchText);
                            if (Users.empty()) return;

                            for (int i = 0; i < Users.size(); i++)
                            {
                                std::shared_ptr<User> User = Users[i];

                                // USER CONTAINER
                                Container UserContainer = {};
                                UserContainer.ID = User->ID;
                                UserContainer.Size = Vector2(SEARCH_MODAL_BODY_CONTAINER_AVAILABLE_SPACE.X, 40.0f);
                                UserContainer.CornerRounding = 10.f;
                                // NOTE: Transparent background
                                UserContainer.BgColor = Rgba(0, 0, 0, 0);
                                UserContainer.BgColorHovered = Rgba(50, 56, 102, 255);
                                UserContainer.IsAutoResizableY = true;
                                UserContainer.OnClick = [&ClientGui, &SearchModal, &User]() {
                                    std::cout << "selected User with ID " << User->ID << std::endl;
                                    ClientGui.CloseModal(SearchModal.ID);
                                    SearchText = "";
                                };
                                UserContainer.DrawContent = [&ClientGui, &BlankImageTexture, &User](const ContainerState& State) {
                                    const Vector2 USER_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                                    // USER IMAGE
                                    Image UserImage = {};
                                    UserImage.TextureID = BlankImageTexture.GetID();
                                    UserImage.Size = Vector2(USER_CONTAINER_AVAILABLE_SPACE.Y, USER_CONTAINER_AVAILABLE_SPACE.Y);
                                    UserImage.CornerRounding = 10.0f;

                                    ClientGui.DrawImage(UserImage);

                                    // USER TEXT
                                    Text UserText = {};
                                    UserText.Value = User->FirstName + " " + User->LastName;

                                    ClientGui.DisplayInline();
                                    ClientGui.SetPositionX(UserImage.Size.X * 1.3f);
                                    Vector2 TextSize = ClientGui.GetTextSize(UserText.Value);
                                    ClientGui.AlignCenterY(TextSize.Y);
                                    ClientGui.DrawText(UserText);
                                };

                                ClientGui.DrawContainer(UserContainer);
                            }
                        };

                        SearchModal.HeaderContainer = SearchModalHeaderContainer;
                        SearchModal.BodyContainer = SearchModalBodyContainer;

                        ClientGui.DrawModal(SearchModal);

                        // SEARCH TEXT INPUT
                        Placeholder SearchTextInputPlaceholder = {};
                        SearchTextInputPlaceholder.Color = Rgba(120, 125, 172, 255);
                        if (!ClientGui.AreAnyModalsOpen()) SearchTextInputPlaceholder.Text = "Enter search here...";

                        TextInput SearchTextInput = {};
                        SearchTextInput.ID = "SearchTextInput";
                        SearchTextInput.Padding = Vector2(15.0f, 15.0f);
                        SearchTextInput.CornerRounding = 10.f;
                        SearchTextInput.BgColor = Rgba(26, 30, 67, 255);
                        SearchTextInput.Placeholder = SearchTextInputPlaceholder;
                        SearchTextInput.OnClick = [&ClientGui, &SearchModal]() {
                           ClientGui.OpenModal(SearchModal.ID);
                        };

                        TextInputSingleline SearchTextInputSingleline = {};
                        SearchTextInputSingleline.TextInput = SearchTextInput;
                        SearchTextInputSingleline.Width = SEARCH_TEXTINPUT_CONTAINER_AVAILABLE_SPACE.X * 0.80f;

                        // NOTE: SearchTextInputSingleline is only a trigger to show SearchModal
                        std::string EmptyValue = "";
                        float SearchTextInputSinglelineHeight = ClientGui.GetTextInputSinglelineHeight();
                        ClientGui.AlignCenter(Vector2(SearchTextInputSingleline.Width, SearchTextInputSinglelineHeight));
                        ClientGui.DrawTextInputSingleline(EmptyValue, SearchTextInputSingleline);
                    };

                    ClientGui.DrawContainer(SearchTextInputContainer);

                    // SETTINGS CONTAINER
                    Container SettingsContainer = {};
                    SettingsContainer.ID = "SettingsContainer";
                    SettingsContainer.Size = Vector2(NAVBAR_AVAILABLE_SPACE.X * 0.50f, NAVBAR_AVAILABLE_SPACE.Y);
                    SettingsContainer.CornerRounding = 10.0f;
                    // NOTE: Transparent background
                    SettingsContainer.BgColor = Rgba(0, 0, 0, 0);
                    SettingsContainer.DrawContent = [&ClientGui, &BlankImageTexture](const ContainerState& State) {
                        const Vector2 SETTINGS_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                        // CURRENT USER IMAGE CONTAINER
                        Container CurrentUserImageContainer = {};
                        CurrentUserImageContainer.ID = "CurrentUserImageContainer";
                        CurrentUserImageContainer.Size = Vector2(SETTINGS_CONTAINER_AVAILABLE_SPACE.Y, SETTINGS_CONTAINER_AVAILABLE_SPACE.Y);
                        CurrentUserImageContainer.CornerRounding = 10.f;
                        CurrentUserImageContainer.Padding = Vector2(10.0f, 10.0f);
                        // NOTE: Transparent background
                        CurrentUserImageContainer.BgColor = Rgba(0, 0, 0, 0);
                        CurrentUserImageContainer.DrawContent = [&ClientGui, &BlankImageTexture](const ContainerState& State) {
                            static bool IsSettingsDropDownMenuOpen = false;

                            // CURRENT USER IMAGE BUTTON
                            Image CurrentUserImage = {};
                            CurrentUserImage.TextureID = BlankImageTexture.GetID();
                            CurrentUserImage.Size = ClientGui.GetAvailableSpace();
                            CurrentUserImage.TintColor = Rgba(255, 255, 255, 255);
                            CurrentUserImage.CornerRounding = 10.0f;

                            ImageButton CurrentUserImageButton = {};
                            CurrentUserImageButton.ID = "CurrentUserImageButton";
                            CurrentUserImageButton.Image = CurrentUserImage;
                            CurrentUserImageButton.TintColorHovered = Rgba(200, 200, 0, 255);
                            CurrentUserImageButton.OnClick = []() {
                                IsSettingsDropDownMenuOpen = !IsSettingsDropDownMenuOpen;
                            };

                            ClientGui.DrawImageButton(CurrentUserImageButton);

                            // Closes setting drop down menu if a modal is open
                            if (ClientGui.AreAnyModalsOpen()) IsSettingsDropDownMenuOpen = false;
                            if (!IsSettingsDropDownMenuOpen) return;

                            // SETTINGS DROP DOWN MENU
                            DropDownMenuItem ProfileDropDownMenuItem = {};
                            ProfileDropDownMenuItem.Text = "Profile";
                            ProfileDropDownMenuItem.TextColor = Rgba(255, 255, 255, 255);
                            ProfileDropDownMenuItem.BgColorHovered = Rgba(50, 56, 102, 255);
                            ProfileDropDownMenuItem.OnClick = []() {
                                std::cout << "Profile clicked!" << std::endl;
                            };

                            DropDownMenuItem PreferencesDropDownMenuItem = {};
                            PreferencesDropDownMenuItem.Text = "Preferences";
                            PreferencesDropDownMenuItem.TextColor = Rgba(255, 255, 255, 255);
                            PreferencesDropDownMenuItem.BgColorHovered = Rgba(50, 56, 102, 255);
                            PreferencesDropDownMenuItem.OnClick = []() {
                                std::cout << "Preferences clicked!" << std::endl;
                            };

                            DropDownMenuItem DownloadssDropDownMenuItem = {};
                            DownloadssDropDownMenuItem.Text = "Downloads";
                            DownloadssDropDownMenuItem.TextColor = Rgba(255, 255, 255, 255);
                            DownloadssDropDownMenuItem.BgColorHovered = Rgba(50, 56, 102, 255);
                            DownloadssDropDownMenuItem.OnClick = []() {
                                std::cout << "Downloads clicked!" << std::endl;
                            };

                            std::vector<std::shared_ptr<DropDownMenuItem>> DropDownMenuItems = {
                                std::make_shared<DropDownMenuItem>(ProfileDropDownMenuItem),
                                std::make_shared<DropDownMenuItem>(PreferencesDropDownMenuItem),
                                std::make_shared<DropDownMenuItem>(DownloadssDropDownMenuItem),
                            };

                            // Calculates drop down menu size
                            Vector2 DropDownMenuSize = Vector2(0.0f, 0.0f);
                            for (std::shared_ptr<DropDownMenuItem> DropDownMenuItem : DropDownMenuItems)
                            {
                                Vector2 TextSize = ClientGui.GetTextSize(DropDownMenuItem->Text);

                                DropDownMenuSize.Y += TextSize.Y;
                                DropDownMenuSize.X = std::max(DropDownMenuSize.X, TextSize.X);
                            }

                            Border SettingsDropDownMenuBorder = {};
                            SettingsDropDownMenuBorder.Color = Rgba(255, 255, 255, 255);
                            SettingsDropDownMenuBorder.Height = 2.0f;

                            DropDownMenu SettingsDropDownMenu = {};
                            SettingsDropDownMenu.Border = SettingsDropDownMenuBorder;
                            SettingsDropDownMenu.Size = DropDownMenuSize;
                            SettingsDropDownMenu.OriginOffset = Vector2(-80.0f, 10.0f);
                            SettingsDropDownMenu.OuterPadding = Vector2(15.0f, 15.0f);
                            SettingsDropDownMenu.BgColor = Rgba(26, 30, 67, 255);
                            SettingsDropDownMenu.CornerRounding = 10.0f;
                            SettingsDropDownMenu.LineHeight = 10.0f;
                            SettingsDropDownMenu.Items = DropDownMenuItems;

                            ClientGui.DrawDropDownMenu(SettingsDropDownMenu);

                        };

                        ClientGui.SetPositionX(SETTINGS_CONTAINER_AVAILABLE_SPACE.X - CurrentUserImageContainer.Size.X);
                        ClientGui.DrawContainer(CurrentUserImageContainer);
                    };

                    ClientGui.DisplayInline();
                    ClientGui.SetPositionX(SearchTextInputContainer.Size.X);
                    ClientGui.DrawContainer(SettingsContainer);
                };

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
            ChatsContainer.DrawContent = [&ClientGui, &BlankImageTexture, &ClosableImageTexture](const ContainerState& State) {
                const Vector2 CHATS_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                // CONVERSATIONS CONTAINER
                Container ConversationsContainer = {};
                ConversationsContainer.ID = "ConversationsContainer";
                ConversationsContainer.Size = Vector2(CHATS_CONTAINER_AVAILABLE_SPACE);
                ConversationsContainer.CornerRounding = 10.f;
                ConversationsContainer.BgColor = Rgba(50, 56, 102, 255);
                ConversationsContainer.DrawContent = [&ClientGui, &BlankImageTexture, &ClosableImageTexture](const ContainerState& State) {
                    const Vector2 CONVERSATIONS_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                    // CONVERSATIONS NODE
                    Node ConversationsNode = {};
                    ConversationsNode.Name = "Conversations";
                    ConversationsNode.DrawContent = [&ClientGui, &BlankImageTexture, &ClosableImageTexture]() {
                        const Vector2 CONVERSATIONS_NODE_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                        for (int i = 0; i < Conversations.size(); i++)
                        {
                            const std::shared_ptr<Conversation> Conversation = Conversations[i];

                            // CONVERSATION CONTAINER
                            Rgba BgColor = Rgba(50, 56, 102, 255);
                            if (Conversation->ID == SelectedConversation->ID) BgColor = Rgba(100, 100, 100, 255);

                            Container ConversationContainer = {};
                            ConversationContainer.ID = "ConversationContainer" + Conversation->ID;
                            ConversationContainer.Size = Vector2(CONVERSATIONS_NODE_AVAILABLE_SPACE.X, CONVERSATIONS_NODE_AVAILABLE_SPACE.Y * 0.05f);
                            ConversationContainer.CornerRounding = 10.f;
                            ConversationContainer.BgColor = BgColor;
                            ConversationContainer.BgColorHovered = Rgba(0, 0, 0, 255);
                            ConversationContainer.IsAutoResizableY = true;
                            ConversationContainer.OnClick = [&Conversation]() {
                                SelectedConversation = Conversation;
                                std::cout << "SELECTED CONVERSATION ID: " << SelectedConversation->ID << std::endl;
                            };
                            ConversationContainer.DrawContent = [&ClientGui, &BlankImageTexture, &ClosableImageTexture, &Conversation, i](const ContainerState& State) {
                                const Vector2 CONVERSATION_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                                // CONVERSATION IMAGE
                                Image ConversationImage = {};
                                ConversationImage.TextureID = BlankImageTexture.GetID();
                                ConversationImage.Size = Vector2(CONVERSATION_CONTAINER_AVAILABLE_SPACE.Y, CONVERSATION_CONTAINER_AVAILABLE_SPACE.Y);
                                ConversationImage.CornerRounding = 10.0f;
                                ClientGui.DrawImage(ConversationImage);

                                // CONVERSATION TEXT
                                Text ConversationText = {};
                                ConversationText.Value = Conversation->Users[1].FirstName;

                                ClientGui.SetPositionX(ConversationImage.Size.X + 10.0f);
                                ClientGui.DrawText(ConversationText);

                                // CLOSE CONVERSATION IMAGE BUTTON CONTAINER
                                if (!State.IsHovered) return;

                                Container CloseConversationImageButtonContainer = {};
                                CloseConversationImageButtonContainer.ID = "CloseConversationImageButtonContainer" + Conversation->ID;
                                CloseConversationImageButtonContainer.Size = ConversationImage.Size;
                                CloseConversationImageButtonContainer.Padding = Vector2(5.0f, 5.0f);
                                // NOTE: Transparent background
                                CloseConversationImageButtonContainer.BgColor = Rgba(0, 0, 0, 0);
                                CloseConversationImageButtonContainer.DrawContent = [&ClientGui, &ClosableImageTexture, &Conversation, i](const ContainerState& State) {
                                    // CLOSE CONVERSATION IMAGE BUTTON
                                    Image CloseConversationImageButtonImage = {};
                                    CloseConversationImageButtonImage.TextureID = ClosableImageTexture.GetID();
                                    CloseConversationImageButtonImage.Size = ClientGui.GetAvailableSpace();
                                    CloseConversationImageButtonImage.TintColor = Rgba(255, 255, 255, 255);
                                    CloseConversationImageButtonImage.CornerRounding = 0.0f;

                                    ImageButton CloseConversationImageButton = {};
                                    CloseConversationImageButton.ID = "CloseConversationImageButton" + Conversation->ID;
                                    CloseConversationImageButton.Image = CloseConversationImageButtonImage;
                                    CloseConversationImageButton.TintColorHovered = Rgba(200, 200, 0, 255);
                                    CloseConversationImageButton.OnClick = [i]() {
                                        const std::string& ID = Conversations[i]->ID;

                                        // Deletes conversation
                                        Conversations.erase(Conversations.begin() + i);
                                        // Selects first conversation if deleted conversation is the selected one
                                        if (SelectedConversation->ID == ID) SelectedConversation = Conversations[0];

                                        std::cout << "DELETED CONVERSATION ID: " << ID << std::endl;
                                    };

                                    ClientGui.DrawImageButton(CloseConversationImageButton);
                                };

                                ClientGui.DisplayInline();
                                ClientGui.SetPositionX(CONVERSATION_CONTAINER_AVAILABLE_SPACE.X - CloseConversationImageButtonContainer.Size.X);
                                ClientGui.DrawContainer(CloseConversationImageButtonContainer);
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
            SelectedConversationContainer.DrawContent = [&ClientGui, &BlankImageTexture](const ContainerState& State) {
                const Vector2 SELECTED_CONVERSATION_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                // MESSSAGES CONTAINER
                Container MessagesContainer = {};
                MessagesContainer.ID = "MessagesContainer";
                MessagesContainer.Size = Vector2(SELECTED_CONVERSATION_CONTAINER_AVAILABLE_SPACE);
                MessagesContainer.CornerRounding = 10.f;
                MessagesContainer.BgColor = Rgba(50, 56, 102, 255);
                MessagesContainer.DrawContent = [&ClientGui, &BlankImageTexture](const ContainerState& State) {
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
                        MessageContainer.DrawContent = [&ClientGui, &BlankImageTexture, &MESSAGE](const ContainerState& State) {
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
                            MessageDetailsContainer.DrawContent = [&ClientGui, &MESSAGE](const ContainerState& State) {
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
            MessageTextInputContainer.DrawContent = [&ClientGui](const ContainerState& State) {
                const Vector2 MESSAGE_TEXTINPUT_CONTAINER_AVAILABLE_SPACE = ClientGui.GetAvailableSpace();

                // MESSAGE TEXT INPUT
                Placeholder MessageTextInputPlaceholder = {};
                MessageTextInputPlaceholder.Color = Rgba(120, 125, 172, 255);
                if (!ClientGui.AreAnyModalsOpen()) MessageTextInputPlaceholder.Text = "Enter message here...";

                TextInput MessageTextInput = {};
                MessageTextInput.ID = "MessageTextInput";
                MessageTextInput.Padding = Vector2(15.0f, 15.0f);
                MessageTextInput.CornerRounding = 10.f;
                MessageTextInput.BgColor = Rgba(43, 50, 94, 255);
                MessageTextInput.Placeholder = MessageTextInputPlaceholder;

                TextInputMultiline MessageTextInputMultiline = {};
                MessageTextInputMultiline.TextInput = MessageTextInput;
                MessageTextInputMultiline.Size = Vector2(MESSAGE_TEXTINPUT_CONTAINER_AVAILABLE_SPACE);

                ClientGui.DrawTextInputMultiline(MessageText, MessageTextInputMultiline);
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
            SendButtonContainer.DrawContent = [&ClientGui](const ContainerState& State) {
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

         // Clears screen
        glClearColor(250.0f / 255.0f, 119.0f / 255.0f, 110.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Rendering
        ClientGui.Render();

        glfwSwapBuffers(GlfwWindow);
    }

    BlankImageTexture.Destroy();
    ClosableImageTexture.Destroy();
    WhiteLeftArrowImageTexture.Destroy();
    ClientGui.Destroy();

    glfwDestroyWindow(GlfwWindow);
    glfwTerminate();

    return 0;
}
