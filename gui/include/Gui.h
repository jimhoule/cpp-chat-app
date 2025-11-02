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
    std::string label;
    Vector2 size;
    Vector2 padding = Vector2(0.0f, 0.0f);
    Vector4 color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    Vector4 color_active = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
    Vector4 color_hovered = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    float border_size = 0.0f;
    float corner_rounding = 0.0f;
    bool is_disabled = false;

    std::function<void()> OnClick;
    std::function<void()> OnHover = {};
};

struct Container
{
    std::string id;
    Vector2 size;
    Vector2 padding = Vector2(0.0f, 0.0f);
    Vector4 bg_color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    float border_size = 0.0f;
    float corner_rounding = 0.0f;

    std::function<void()> DrawContent;
};

struct Text
{
    std::string value;
};

struct TextInput
{
    std::string id;
    std::string placeholder;
    Vector2 size;
    Vector2 padding = Vector2(0.0f, 0.0f);
    Vector4 bg_color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    Vector4 text_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    Rgba placeholder_color = Rgba(255, 255, 255, 255);
    float border_size = 0.0f;
    float corner_rounding = 0.0f;
};

struct TreeNode {
    std::string name;
    std::vector<TreeNode> children;
};

struct Window
{
    std::string name;
    Vector2 size;
    Vector2 position = Vector2(0.0f, 0.0f);
    Vector2 padding = Vector2(0.0f, 0.0f);
    Vector4 bg_color = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    bool can_save_settigs = false;
    bool is_titlebar_visible = false;
    bool is_scrollbar_visible = false;
    bool is_resizable = false;
    bool is_collapsible = false;
    bool is_movable = false;

    std::function<void()> DrawContent;
};

class Gui
{
public:
    Gui() = default;

    void Init(GLFWwindow* glfw_window) const;
    void Render() const;
    void Clear() const;
    void Destroy() const;

    void DrawButton(const Button& button) const;
    void DrawContainer(const Container& container) const;
    void DrawText(const Text& text) const;
    void DrawTextInputMultiline(std::string& value, const TextInput& text_input) const;
    void DrawTreeNode(const TreeNode& node) const;
    void DrawWindow(const Window& window) const;

    void AlignCenter(Vector2 element_size) const;
    void AlignCenterX(float element_width) const;
    void AlignCenterY(float element_heigth) const;
    void DisplayInline() const;
    void ScrollToY(float y) const;
    void ScrollToX(float x) const;
    float GetScrollPositionY() const;
    float GetScrollPositionX() const;
    float GetMaxScrollPositionY() const;
    float GetMaxScrollPositionX() const;
    const Vector2 GetAvailableSpace() const;
    const Vector2 GetPosition() const;
    float GetPositionX() const;
    float GetPositionY() const;
    void SetPosition(Vector2 position) const;
    void SetPositionX(float x) const;
    void SetPositionY(float y) const;

private:
    const Vector2 ToVector2(const ImVec2& vector_2) const;
    const Vector4 ToVector4(const ImVec4& vector_4) const;
    const ImVec2 ToImVec2(const Vector2& vector_2) const;
    const ImVec4 ToImVec4(const Vector4& vector_4) const;
};
