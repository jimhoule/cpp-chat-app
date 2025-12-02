#pragma once

#include "Color.h"
#include "Vector.h"
// NOTE: Not inluding header files to avoid conflict with GLAD when imported into client.cpp
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <functional>
#include <map>
#include <memory>
#include <string>

struct Border
{
    Rgba Color;
    float Height;
};

struct Button
{
    std::string Label;
    Border Border = { Rgba(255, 255, 255, 255), 0.0f };
    Vector2 Size;
    Vector2 Padding = Vector2(0.0f, 0.0f);
    Rgba BgColor = Rgba(255, 0, 0, 255);
    Rgba BgColorActive = {};
    Rgba BgColorHovered = {};
    Rgba TextColor = Rgba(255, 255, 255, 255);
    float CornerRounding = 0.0f;
    bool IsDisabled = false;

    std::function<void()> OnClick;
    std::function<void()> OnHover = {};
};

struct ContainerState
{
    bool IsHovered = false;
};

struct Container
{
    std::string ID;
    Border Border = { Rgba(255, 255, 255, 255), 0.0f };
    Vector2 Size;
    Vector2 Padding = Vector2(0.0f, 0.0f);
    Rgba BgColor = Rgba(0, 0, 0, 255);
    Rgba BgColorHovered = {};
    float CornerRounding = 0.0f;
    bool IsAutoResizableY = false;
    bool IsAutoResizableX = false;

    std::function<void(const ContainerState&)> DrawContent;
    std::function<void()> OnClick = {};
    std::function<void()> OnHover = {};
};

struct Divider
{
    Rgba Color;
    float Height = 1.0f;
};

struct DropDownMenuItem
{
    std::string Text;
    Rgba TextColor = Rgba(255, 255, 255, 255);
    Rgba BgColorHovered;
    Rgba TextColorHovered;

    std::function<void()> OnClick;
    std::function<void()> OnHover = {};
};

struct DropDownMenu
{
    Border Border = { Rgba(255, 255, 255, 255), 0.0f };
    Vector2 Size;
    Vector2 OuterPadding = Vector2(0.0f, 0.0f);
    Rgba BgColor = Rgba(0, 0, 0, 255);
    float LineHeight = 0.0f;
    std::vector<std::shared_ptr<DropDownMenuItem>> Items;
};

struct Text
{
    std::string Value;
};

struct Image
{
    unsigned int TextureID;
    Vector2 Size;
    Rgba TintColor = Rgba(255, 255, 255, 255);
    float CornerRounding = 0.0f;
};

struct ImageButton
{
    std::string ID;
    Image Image;
    Rgba TintColorHovered = {};

    std::function<void()> OnClick;
};

struct Modal
{
    std::string ID;
    Container HeaderContainer;
    Container BodyContainer;
    Vector2 Size;
    Vector2 Padding = Vector2(0.0f, 0.0f);
    Rgba BgColor = Rgba(0, 0, 0, 255);
    float CornerRounding = 0.0f;
    bool CanSaveSettigs = false;
    bool IsTitlebarVisible = false;
    bool IsScrollbarVisible = false;
    bool IsResizable = false;
    bool IsCollapsible = false;
    bool IsMovable = false;
};

struct Node
{
    std::string Name;

    std::function<void()> DrawContent;
};

struct Placeholder
{
    Rgba Color = Rgba(255, 255, 255, 255);
    std::string Text;
};

struct TextInput
{
    std::string ID;
    Border Border = { Rgba(255, 255, 255, 255), 0.0f };
    Placeholder Placeholder;
    Vector2 Padding = Vector2(0.0f, 0.0f);
    Rgba BgColor = Rgba(0, 0, 0, 255);
    Rgba TextColor = Rgba(255, 255, 255, 255);
    float CornerRounding = 0.0f;

    std::function<void()> OnClick = {};
};

struct TextInputMultiline
{
    TextInput TextInput;
    Vector2 Size;
};

struct TextInputSingleline
{
    TextInput TextInput;
    float Width;
};

template<typename T>
struct SearchInput
{
    TextInputSingleline TextInputSingleline;

    std::function<std::string(std::shared_ptr<T>)> ExtractLabel;
    std::function<std::vector<std::shared_ptr<T>()>> OnSearch;
    std::function<void(std::shared_ptr<T>)> OnSelect;
};

struct TreeNode
{
    std::string Name;
    std::vector<TreeNode> Children;
};

struct Window
{
    std::string Name;
    Vector2 Size;
    Vector2 Position = Vector2(0.0f, 0.0f);
    Vector2 Padding = Vector2(0.0f, 0.0f);
    Rgba BgColor = Rgba(0, 0, 0, 255);
    bool CanSaveSettigs = false;
    bool IsTitlebarVisible = false;
    bool IsScrollbarVisible = false;
    bool IsResizable = false;
    bool IsCollapsible = false;
    bool IsMovable = false;

    std::function<void()> DrawContent;
};

class Gui
{
public:
    Gui() = default;

    void Init(GLFWwindow* GlfwWindow) const;
    void Destroy() const;
    void Render() const;
    void Clear() const;

    void DrawButton(Button& Button) const;
    void DrawContainer(Container& Container) const;
    void DrawDivider(const Divider& Divider) const;
    void DrawDropDownMenu(const DropDownMenu& DropDownMenu) const;
    void DrawImage(const Image& Image) const;
    void DrawImageButton(ImageButton& ImageButton) const;
    void DrawNode(const Node& Node) const;
    void DrawModal(Modal& Modal) const;
    void DrawText(const Text& Text) const;
    void DrawTextWrapped(const Text& Text) const;
    void DrawTextInputMultiline(std::string& Value, TextInputMultiline& TextInputMultiline) const;
    void DrawTextInputSingleline(std::string& Value, TextInputSingleline& TextInputSingleline) const;
    void DrawTreeNode(const TreeNode& RootTreeNode) const;
    void DrawWindow(Window& Window) const;

    template<typename T>
    void DrawSearchInput(std::string& Value, SearchInput<T>& SearchInput) const
    {
        DrawTextInputSingleline(Value, SearchInput.TextInputSingleline);
        if (Value.empty()) return;

        std::vector<std::shared_ptr<T>> Suggestions = SearchInput.OnSearch();
        if (Suggestions.empty()) return;

        std::vector<std::shared_ptr<DropDownMenuItem>> DropDownMenuItems = {};
        for(std::shared_ptr<T> Suggestion: Suggestions)
        {
            const std::string& Label = SearchInput.ExtractLabel(Suggestion);

            DropDownMenuItem Item = {};
            Item.Text = Label;
            Item.TextColor = SearchInput.TextInputSingleline.TextInput.TextColor;
            Item.BgColorHovered = Rgba(50, 56, 102, 255);
            Item.OnClick = [&SearchInput, &Suggestion]() {
                SearchInput.OnSelect(Suggestion);
            };

            DropDownMenuItems.push_back(std::make_shared<DropDownMenuItem>(Item));
        }

        // Calculates drop down menu size
        Vector2 DropDownMenuSize = Vector2(SearchInput.TextInputSingleline.Width, 0.0f);
        for (std::shared_ptr<DropDownMenuItem> DropDownMenuItem : DropDownMenuItems)
        {
            ImVec2 TextSize = ImGui::CalcTextSize(DropDownMenuItem->Text.c_str());
            DropDownMenuSize.Y += TextSize.y;
        }

        Border SettingsDropDownMenuBorder = {};
        SettingsDropDownMenuBorder.Color = SearchInput.TextInputSingleline.TextInput.Border.Color;
        SettingsDropDownMenuBorder.Height = 2.0f;

        DropDownMenu SettingsDropDownMenu = {};
        SettingsDropDownMenu.Border = SettingsDropDownMenuBorder;
        SettingsDropDownMenu.Size = DropDownMenuSize;
        SettingsDropDownMenu.OuterPadding = Vector2(15.0f, 15.0f);
        SettingsDropDownMenu.BgColor = SearchInput.TextInputSingleline.TextInput.BgColor;
        SettingsDropDownMenu.LineHeight = 10.0f;
        SettingsDropDownMenu.Items = DropDownMenuItems;

        DrawDropDownMenu(SettingsDropDownMenu);
    }

    bool AreAnyModalsOpen() const;
    void OpenModal(const std::string& ID);
    void CloseModal(const std::string& ID);

    void AlignCenter(Vector2 ElementSize) const;
    void AlignCenterX(float ElementWidth) const;
    void AlignCenterY(float ElementHeigth) const;
    void DisplayInline() const;
    const Vector2 GetTextSize(const std::string& Text) const;
    float GetTextInputSinglelineHeight() const;
    void ScrollToY(float X) const;
    void ScrollToX(float Y) const;
    float GetScrollPositionY() const;
    float GetScrollPositionX() const;
    float GetMaxScrollPositionY() const;
    float GetMaxScrollPositionX() const;
    const Vector2 GetAvailableSpace() const;
    const Vector2 GetViewportSize() const;
    const Vector2 GetPosition() const;
    float GetPositionX() const;
    float GetPositionY() const;
    void SetPosition(Vector2 Position) const;
    void SetPositionX(float X) const;
    void SetPositionY(float Y) const;

private:
    // NOTE: Keeps track of all open modals
    std::map<std::string, bool> OpenModalIds = {};

    struct ImagePositioned
    {
        Image Image;
        Vector2 Position;
    };

    void DrawImagePositioned(const ImagePositioned& ImagePositioned) const;
    void DrawPlaceholder(const Placeholder& Placeholder) const;
    const Vector2 ToVector2(const ImVec2& Vector2) const;
    const Vector4 ToVector4(const ImVec4& Vector4) const;
    const ImVec2 ToImVec2(const Vector2& Vector2) const;
    const ImVec4 ToImVec4(const Vector4& Vector4) const;
};
