#include "Gui.h"

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// PUBLIC
void Gui::Init(GLFWwindow* glfw_window) const
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.Fonts->AddFontFromFileTTF("../../assets/Audiowide-Regular.ttf");

     // Setup Dear ImGui style
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
}

void Gui::Render() const
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::Clear() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::Destroy() const
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::DrawButton(const Button& button) const
{
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, button.border_size);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ToImVec2(button.padding));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, button.corner_rounding);
    ImGui::PushStyleColor(ImGuiCol_Button, ToImVec4(button.color));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec4(button.color_active));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ToImVec4(button.color_hovered));

    if (button.is_disabled)  ImGui::BeginDisabled();

    if (ImGui::Button(button.label.c_str(), ImVec2(button.size.x, button.size.y))) button.OnClick();
    if (ImGui::IsItemHovered() && !!button.OnHover) button.OnHover();

    if (button.is_disabled)  ImGui::EndDisabled();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);
}

void Gui::DrawContainer(const Container& container) const
{
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, container.border_size);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ToImVec2(container.padding));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, container.corner_rounding);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ToImVec4(container.bg_color));

    if (ImGui::BeginChild(container.id.c_str(), ToImVec2(container.size), ImGuiChildFlags_AlwaysUseWindowPadding)) container.DrawContent();
    ImGui::EndChild();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(1);
}

 void Gui::DrawText(const Text& text) const
 {
    ImGui::TextUnformatted(text.value.c_str());
 }

void Gui::DrawTextInputMultiline(std::string& value, const TextInput& text_input) const
{
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, text_input.border_size);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ToImVec2(text_input.padding));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, text_input.corner_rounding);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ToImVec4(text_input.bg_color));
    ImGui::PushStyleColor(ImGuiCol_Text, ToImVec4(text_input.text_color));

    // NOTE: ## prefix tells ImGui to use the string for internal ID generation but not to display it as a visible label
    const std::string& id = "##" + text_input.id;
    ImGui::InputTextMultiline(id.c_str(), &value, ToImVec2(text_input.size));

    // Draws placeholder on top of text input multiline
    if (!text_input.placeholder.empty() && value.size() == 0)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec2 frame_padding = style.FramePadding;

        ImDrawList* foreground_draw_list = ImGui::GetForegroundDrawList();
        ImVec2 bounding_box_min_position = ImGui::GetItemRectMin();

        ImVec2 placeholder_position(
            bounding_box_min_position.x + frame_padding.x,
            bounding_box_min_position.y + frame_padding.y
        );
        ImU32 placeholder_color = IM_COL32(
            text_input.placeholder_color.r,
            text_input.placeholder_color.g,
            text_input.placeholder_color.b,
            text_input.placeholder_color.a
        );

        foreground_draw_list->AddText(placeholder_position, placeholder_color, text_input.placeholder.c_str());
    }

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

void Gui::DrawTreeNode(const TreeNode& node) const
{
    if (ImGui::TreeNode(node.name.c_str()))
    {
        for (const TreeNode& child : node.children)
        {
            DrawTreeNode(child);
        }
        ImGui::TreePop();
    }
}

void Gui::DrawWindow(const Window& window) const
{
    bool is_open = true;
    ImGuiWindowFlags flags = ImGuiWindowFlags_None;

    if (!window.can_save_settigs) flags |= ImGuiWindowFlags_NoSavedSettings;
    if (!window.is_titlebar_visible) flags |= ImGuiWindowFlags_NoTitleBar;
    if (!window.is_scrollbar_visible) flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    if (!window.is_resizable) flags |= ImGuiWindowFlags_NoResize;
    if (!window.is_collapsible) flags |= ImGuiWindowFlags_NoCollapse;
    if (!window.is_movable) flags |= ImGuiWindowFlags_NoMove;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ToImVec2(window.padding));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ToImVec4(window.bg_color));

    ImGui::SetNextWindowPos(ToImVec2(window.position));
    ImGui::SetNextWindowSize(ToImVec2(window.size));
    if (ImGui::Begin(window.name.c_str(), &is_open, flags)) window.DrawContent();
    ImGui::End();

    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(1);
}

void Gui::AlignCenter(Vector2 element_size) const
{
    const Vector2 AVAILABLE_SPACE = GetAvailableSpace();
    const Vector2 OFFSET = (AVAILABLE_SPACE - element_size) * 0.5f;
    SetPosition(GetPosition() + OFFSET);
}

void Gui::AlignCenterX(float element_width) const
{
    const Vector2 AVAILABLE_SPACE = GetAvailableSpace();
    const float OFFSET_X = (AVAILABLE_SPACE.x - element_width) * 0.5f;
    SetPositionX(GetPositionX() + OFFSET_X);
}

void Gui::AlignCenterY(float element_heigth) const
{
    const Vector2 AVAILABLE_SPACE = GetAvailableSpace();
    const float OFFSET_Y = (AVAILABLE_SPACE.y - element_heigth) * 0.5f;
    SetPositionY(GetPositionY() + OFFSET_Y);
}

void Gui::DisplayInline() const
{
    ImGui::SameLine();
}

void Gui::ScrollToY(float y) const
{
    ImGui::SetScrollHereY(y);
}

void Gui::ScrollToX(float x) const
{
    ImGui::SetScrollHereX(x);
}

float Gui::GetScrollPositionY() const
{
    return ImGui::GetScrollY();
}

float Gui::GetScrollPositionX() const
{
    return ImGui::GetScrollX();
}

float Gui::GetMaxScrollPositionY() const
{
    return ImGui::GetScrollMaxY();
}

float Gui::GetMaxScrollPositionX() const
{
    return ImGui::GetScrollMaxX();
}

const Vector2 Gui::GetAvailableSpace() const
{
    return ToVector2(ImGui::GetContentRegionAvail());
}

const Vector2 Gui::GetPosition() const
{
    return ToVector2(ImGui::GetCursorPos());
}

float Gui::GetPositionX() const
{
    return ImGui::GetCursorPosX();
}

float Gui::GetPositionY() const
{
    return ImGui::GetCursorPosY();
}

void Gui::SetPosition(Vector2 position) const
{
    ImGui::SetCursorPos(ToImVec2(position));
}

void Gui::SetPositionX(float x) const
{
    ImGui::SetCursorPosX(x);
}

void Gui::SetPositionY(float y) const
{
    ImGui::SetCursorPosY(y);
}

// PRIVATE
const Vector2 Gui::ToVector2(const ImVec2& imgui_vec_2) const
{
    return Vector2(imgui_vec_2.x, imgui_vec_2.y);
}

const Vector4 Gui::ToVector4(const ImVec4& imgui_vec_4) const
{
    return Vector4(imgui_vec_4.x, imgui_vec_4.y, imgui_vec_4.z, imgui_vec_4.w);
}

const ImVec2 Gui::ToImVec2(const Vector2& vector_2) const
{
    return ImVec2(vector_2.x, vector_2.y);
}

const ImVec4 Gui::ToImVec4(const Vector4& vector_4) const
{
    return ImVec4(vector_4.x, vector_4.y, vector_4.z, vector_4.w);
}
