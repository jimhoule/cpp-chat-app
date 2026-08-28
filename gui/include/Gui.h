#pragma once

#include "color/Rgba.h"
#include "Vector.h"
// NOTE: Not inluding header files to avoid conflict with GLAD when imported into client.cpp
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <functional>
#include <map>
#include <memory>
#include <string>

class Gui
{
public:
    // Elements
    struct Border
    {
        Rgba color;
        float height;
    };

    struct Button
    {
        std::string label;
        Border border = { Rgba(255, 255, 255, 255), 0.0f };
        Vector2 size;
        Vector2 padding = Vector2(0.0f, 0.0f);
        Rgba bgColor = Rgba(255, 0, 0, 255);
        Rgba bgColorActive = {};
        Rgba bgColorHovered = {};
        Rgba textColor = Rgba(255, 255, 255, 255);
        float cornerRounding = 0.0f;
        bool isDisabled = false;

        std::function<void()> OnClick;
        std::function<void()> OnHover = {};
    };

    struct ContainerState
    {
        bool isHovered = false;
    };

    struct Container
    {
        std::string id;
        Border border = { Rgba(255, 255, 255, 255), 0.0f };
        Vector2 size;
        Vector2 padding = Vector2(0.0f, 0.0f);
        Rgba bgColor = Rgba(0, 0, 0, 255);
        Rgba bgColorHovered = {};
        float cornerRounding = 0.0f;
        bool isAutoResizableY = false;
        bool isAutoResizableX = false;

        std::function<void(const ContainerState&)> DrawContent;
        std::function<void()> OnClick = {};
        std::function<void()> OnHover = {};
    };

    struct Divider
    {
        Rgba color;
        float height = 1.0f;
    };

    struct DropDownMenuItem
    {
        std::string text;
        Rgba textColor = Rgba(255, 255, 255, 255);
        Rgba bgColorHovered;
        Rgba textColorHovered;

        std::function<void()> OnClick;
        std::function<void()> OnHover = {};
    };

    struct DropDownMenu
    {
        Border border = { Rgba(255, 255, 255, 255), 0.0f };
        Vector2 size;
        Vector2 outerPadding = Vector2(0.0f, 0.0f);
        Vector2 originOffset = Vector2(0.0f, 0.0f);
        Rgba bgColor = Rgba(0, 0, 0, 255);
        float cornerRounding = 0.0f;
        float lineHeight = 0.0f;
        std::vector<std::shared_ptr<DropDownMenuItem>> items;
    };

    struct Text
    {
        std::string value;
        Rgba color = Rgba(255, 255, 255, 255);
        float height = NULL;
    };

    struct Image
    {
        unsigned int textureId;
        Vector2 size;
        Rgba tintColor = Rgba(255, 255, 255, 255);
        float cornerRounding = 0.0f;
    };

    struct ImageButton
    {
        std::string id;
        Image image;
        Rgba tintColorHovered = {};

        std::function<void()> OnClick;
    };

    struct Modal
    {
        std::string id;
        Container headerContainer;
        Container bodyContainer;
        Vector2 size;
        Vector2 padding = Vector2(0.0f, 0.0f);
        Rgba bgColor = Rgba(0, 0, 0, 255);
        float cornerRounding = 0.0f;
        bool canSaveSettings = false;
        bool isTitlebarVisible = false;
        bool isScrollbarVisible = false;
        bool isResizable = false;
        bool isCollapsible = false;
        bool isMovable = false;
    };

    struct Node
    {
        std::string name;

        std::function<void()> DrawContent;
    };

    struct Placeholder
    {
        Rgba color = Rgba(255, 255, 255, 255);
        std::string text;
    };

    struct TextInput
    {
        std::string id;
        Border border = { Rgba(255, 255, 255, 255), 0.0f };
        Placeholder placeholder;
        Vector2 padding = Vector2(0.0f, 0.0f);
        Rgba bgColor = Rgba(0, 0, 0, 255);
        Rgba textColor = Rgba(255, 255, 255, 255);
        float cornerRounding = 0.0f;

        std::function<void()> OnClick = {};
    };

    struct TextInputMultiline
    {
        TextInput textInput;
        Vector2 size;
    };

    struct TextInputSingleline
    {
        TextInput textInput;
        float width;
    };

    template<typename T>
    struct SearchInput
    {
        TextInputSingleline textInputSingleline;

        std::function<std::string(std::shared_ptr<T>)> ExtractLabel;
        std::function<std::vector<std::shared_ptr<T>()>> OnSearch;
        std::function<void(std::shared_ptr<T>)> OnSelect;
    };

    struct TreeNode
    {
        std::string name;
        std::vector<TreeNode> children;
    };

    struct Window
    {
        std::string name;
        Vector2 size;
        Vector2 position = Vector2(0.0f, 0.0f);
        Vector2 padding = Vector2(0.0f, 0.0f);
        Rgba bgColor = Rgba(0, 0, 0, 255);
        bool canSaveSettings = false;
        bool isTitlebarVisible = false;
        bool isScrollbarVisible = false;
        bool isResizable = false;
        bool isCollapsible = false;
        bool isMovable = false;

        std::function<void()> DrawContent;
    };

    /**
     * NOTES:
     *  - Raw elements
     *  - Those elements are drawn on the foreground without being clipped to any parent elements
     *  - They will always be drawn on top of any other elements
     */
    struct RawText
    {
        std::string value;
        Vector2 position;
        Rgba color = Rgba(255, 255, 255, 255);
    };

    struct RawWindow
    {
        Vector2 minCornerPosition;
        Vector2 maxCornerPosition;
        Rgba bgColor = Rgba(0, 0, 0, 255);
        float cornerRounding = 0.0f;

        std::function<void()> DrawContent;
    };

    Gui() = default;

    void Init(GLFWwindow* glfwWindow) const;
    void Destroy() const;
    void Render() const;
    void Clear() const;

    // Elements
    void DrawButton(Button& button) const;
    void DrawContainer(Container& container) const;
    void DrawDivider(const Divider& divider) const;
    void DrawDropDownMenu(const DropDownMenu& dropDownMenu) const;
    void DrawImage(const Image& image) const;
    void DrawImageButton(ImageButton& imageButton) const;
    void DrawNode(const Node& node) const;
    void DrawModal(Modal& modal) const;
    void DrawText(Text& text) const;
    void DrawTextWrapped(Text& Text) const;
    void DrawTextInputMultiline(std::string& value, TextInputMultiline& textInputMultiline) const;
    void DrawTextInputSingleline(std::string& value, TextInputSingleline& textInputSingleline) const;
    void DrawTreeNode(const TreeNode& rootTreeNode) const;
    void DrawWindow(Window& window) const;

    template<typename T>
    void DrawSearchInput(std::string& value, SearchInput<T>& searchInput) const
    {
        DrawTextInputSingleline(value, searchInput.textInputSingleline);
        if (value.empty()) return;

        std::vector<std::shared_ptr<T>> suggestions = searchInput.OnSearch();
        if (suggestions.empty()) return;

        std::vector<std::shared_ptr<DropDownMenuItem>> dropDownMenuItems = {};
        for(std::shared_ptr<T> suggestion: suggestions)
        {
            const std::string& label = searchInput.ExtractLabel(suggestion);

            DropDownMenuItem item = {};
            item.text = label;
            item.textColor = searchInput.textInputSingleline.textInput.textColor;
            item.bgColorHovered = Rgba(50, 56, 102, 255);
            item.OnClick = [&searchInput, &suggestion]() {
                searchInput.OnSelect(suggestion);
            };

            dropDownMenuItems.push_back(std::make_shared<DropDownMenuItem>(item));
        }

        // Calculates drop down menu size
        Vector2 dropDownMenuSize = Vector2(searchInput.textInputSingleline.width, 0.0f);
        for (std::shared_ptr<DropDownMenuItem> dropDownMenuItem : dropDownMenuItems)
        {
            ImVec2 textSize = ImGui::CalcTextSize(dropDownMenuItem->text.c_str());
            dropDownMenuSize.y += textSize.y;
        }

        Border settingsDropDownMenuBorder = {};
        settingsDropDownMenuBorder.color = searchInput.textInputSingleline.textInput.border.color;
        settingsDropDownMenuBorder.height = 2.0f;

        DropDownMenu settingsDropDownMenu = {};
        settingsDropDownMenu.border = settingsDropDownMenuBorder;
        settingsDropDownMenu.size = dropDownMenuSize;
        settingsDropDownMenu.outerPadding = Vector2(15.0f, 15.0f);
        settingsDropDownMenu.bgColor = searchInput.textInputSingleline.textInput.bgColor;
        settingsDropDownMenu.lineHeight = 10.0f;
        settingsDropDownMenu.items = dropDownMenuItems;

        DrawDropDownMenu(settingsDropDownMenu);
    }

    /**
     * NOTES:
     *  - Raw elements
     *  - Those elements are drawn on the foreground without being clipped to any parent elements
     *  - They will always be drawn on top of any other elements
     */
    void DrawRawText(const RawText& rawText) const;
    void DrawRawWindow(const RawWindow& rawWindow) const;

    // Modals helpers
    bool AreAnyModalsOpen() const;
    void OpenModal(const std::string& id);
    void CloseModal(const std::string& id);

    // position helpers
    void AlignCenter(Vector2 elementSize) const;
    void AlignCenterX(float elementWidth) const;
    void AlignCenterY(float elementHeigth) const;
    void DisplayInline() const;
    void ScrollToY(float x) const;
    void ScrollToX(float y) const;
    float GetScrollPositionY() const;
    float GetScrollPositionX() const;
    float GetMaxScrollPositionY() const;
    float GetMaxScrollPositionX() const;
    const Vector2 GetPosition() const;
    float GetPositionX() const;
    float GetPositionY() const;
    void SetPosition(Vector2 position) const;
    void SetPositionX(float x) const;
    void SetPositionY(float y) const;

    // Dimension helpers
    const Vector2 GetAvailableSpace() const;
    const Vector2 GetParentContainerPaddingSize() const;
    float GetTextInputSinglelineHeight() const;
    const Vector2 GetTextSize(const std::string& text) const;
    const Vector2 GetTextSize(const Text& text) const;
    const Vector2 GetViewportSize() const;

    // Config helpers
    float GetDeltaTime() const;

private:
    // NOTE: Keeps track of all open modals
    std::map<std::string, bool> m_openModalIds = {};

    struct ImagePositioned
    {
        Image image;
        Vector2 position;
    };

    void DrawImagePositioned(const ImagePositioned& imagePositioned) const;
    void DrawPlaceholder(const Placeholder& placeholder) const;
    const Vector2 ToVector2(const ImVec2& vector2) const;
    const Vector4 ToVector4(const ImVec4& vector4) const;
    const ImVec2 ToImVec2(const Vector2& vector2) const;
    const ImVec4 ToImVec4(const Vector4& vector4) const;
};
