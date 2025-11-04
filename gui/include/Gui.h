#pragma once

#include "Color.h"
#include "Vector.h"
// NOTE: Not inluding header files to aoid conflict with GLAD when imported into client.cpp
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

#include <functional>
#include <optional>
#include <string>

struct Button
{
    std::string Label;
    Vector2 Size;
    Vector2 Padding = Vector2(0.0f, 0.0f);
    Rgba Color = Rgba(255, 0, 0, 1);
    Rgba ColorActive = Rgba(0, 255, 0, 1);
    Rgba ColorHovered = Rgba(0, 0, 255, 1);
    float BorderSize = 0.0f;
    float CornerRounding = 0.0f;
    bool IsDisabled = false;

    std::function<void()> OnClick;
    std::function<void()> OnHover = {};
};

struct Container
{
    std::string ID;
    Vector2 Size;
    Vector2 Padding = Vector2(0.0f, 0.0f);
    Rgba BgColor = Rgba(0, 0, 0, 1);
    float BorderSize = 0.0f;
    float CornerRounding = 0.0f;

    std::function<void()> DrawContent;
};

struct Text
{
    std::string Value;
};

struct Image
{
    unsigned int TextureId;
    Vector2 Size;
    float CornerRounding = 0.0f;
};

struct TextInput
{
    std::string ID;
    std::string Placeholder;
    Vector2 Size;
    Vector2 Padding = Vector2(0.0f, 0.0f);
    Rgba BgColor = Rgba(0, 0, 0, 1);
    Rgba TextColor = Rgba(255, 255, 255, 1);
    Rgba PlaceholderColor = Rgba(255, 255, 255, 1);
    float BorderSize = 0.0f;
    float CornerRounding = 0.0f;
};

struct TreeNode {
    std::string Name;
    std::vector<TreeNode> Children;
};

struct Window
{
    std::string Name;
    Vector2 Size;
    Vector2 Position = Vector2(0.0f, 0.0f);
    Vector2 Padding = Vector2(0.0f, 0.0f);
    Rgba BgColor = Rgba(0, 0, 0, 1);
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
    void DrawImage(const Image& Image) const;
    void DrawText(const Text& Text) const;
    void DrawTextInputMultiline(std::string& Value, TextInput& TextInput) const;
    void DrawTreeNode(const TreeNode& Node) const;
    void DrawWindow(Window& Window) const;

    void AlignCenter(Vector2 ElementSize) const;
    void AlignCenterX(float ElementWidth) const;
    void AlignCenterY(float ElementHeigth) const;
    void DisplayInline() const;
    void ScrollToY(float X) const;
    void ScrollToX(float Y) const;
    float GetScrollPositionY() const;
    float GetScrollPositionX() const;
    float GetMaxScrollPositionY() const;
    float GetMaxScrollPositionX() const;
    const Vector2 GetAvailableSpace() const;
    const Vector2 GetPosition() const;
    float GetPositionX() const;
    float GetPositionY() const;
    void SetPosition(Vector2 Position) const;
    void SetPositionX(float X) const;
    void SetPositionY(float Y) const;

private:
    const Vector2 ToVector2(const ImVec2& Vector2) const;
    const Vector4 ToVector4(const ImVec4& Vector4) const;
    const ImVec2 ToImVec2(const Vector2& Vector2) const;
    const ImVec4 ToImVec4(const Vector4& Vector4) const;
};
