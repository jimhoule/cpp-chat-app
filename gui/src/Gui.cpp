#include "Gui.h"

#include <imgui/imgui_internal.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// **********
// * PUBLIC *
// **********
void Gui::Init(GLFWwindow* glfwWindow) const
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
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 150");
}

void Gui::Destroy() const
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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

// Elements
void Gui::DrawButton(Button& button) const
{
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, button.border.height);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ToImVec2(button.padding));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, button.cornerRounding);
    ImGui::PushStyleColor(ImGuiCol_Button, ToImVec4(button.bgColor.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_Text, ToImVec4(button.textColor.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_Border, ToImVec4(button.border.color.ToVector4()));
    if (!button.bgColorActive.IsEmpty()) ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec4(button.bgColorActive.ToVector4()));
    if (!button.bgColorHovered.IsEmpty()) ImGui::PushStyleColor(ImGuiCol_ButtonActive, ToImVec4(button.bgColorHovered.ToVector4()));

    if (button.isDisabled)  ImGui::BeginDisabled();

    ImGui::Button(button.label.c_str(), ImVec2(button.size.x, button.size.y));
    if (ImGui::IsItemClicked()) button.OnClick();
    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        if (!!button.OnHover) button.OnHover();
    }

    if (button.isDisabled)  ImGui::EndDisabled();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);
    if (!button.bgColorActive.IsEmpty()) ImGui::PopStyleColor(1);
    if (!button.bgColorHovered.IsEmpty()) ImGui::PopStyleColor(1);
}

void Gui::DrawContainer(Container& container) const
{
    ImGuiChildFlags flags = ImGuiChildFlags_AlwaysUseWindowPadding;
    if (container.isAutoResizableY) flags |= ImGuiChildFlags_AutoResizeY;
    if (container.isAutoResizableX) flags |= ImGuiChildFlags_AutoResizeX;

    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, container.border.height);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ToImVec2(container.padding));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, container.cornerRounding);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ToImVec4(container.bgColor.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_Border, ToImVec4(container.border.color.ToVector4()));

    if (ImGui::BeginChild(container.id.c_str(), ToImVec2(container.size), flags))
    {
        const bool isHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
        // Draws rectangle on top of background container when is hovered
        if (isHovered && !container.bgColorHovered.IsEmpty())
        {
            ImDrawList* windowDrawList = ImGui::GetWindowDrawList();

            ImVec2 bgPositionMin = ImGui::GetCursorScreenPos();
            ImVec2 bgPositionMax = ImVec2(bgPositionMin.x + container.size.x, bgPositionMin.y + container.size.y);
            ImU32 bgColorHovered = IM_COL32(
                container.bgColorHovered.r,
                container.bgColorHovered.g,
                container.bgColorHovered.b,
                container.bgColorHovered.a
            );

            windowDrawList->AddRectFilled(
                bgPositionMin,
                bgPositionMax,
                bgColorHovered,
                container.cornerRounding
            );
        }

        if (isHovered && !!container.OnHover) container.OnHover();

        ContainerState state = {};
        state.isHovered = isHovered;
        container.DrawContent(state);
    }
    ImGui::EndChild();

    // NOTE: IsItemClicked() refers to the item that was most recently defined. For a child window, the "item" representing the whole child rectangle is defined by the BeginChild() and EndChild() calls
    const bool isClicked = ImGui::IsItemClicked();
    if (isClicked && !!container.OnClick) container.OnClick();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

void Gui::DrawDivider(const Divider& divider) const
{
    ImDrawList* windowDrawList = ImGui::GetWindowDrawList();
    ImVec2 lineStartPosition = ImGui::GetCursorScreenPos();
    ImVec2 lineEndPosition = ImVec2(lineStartPosition.x + ImGui::GetWindowWidth(), lineStartPosition.y);
    ImU32 color = IM_COL32(
        divider.color.r,
        divider.color.g,
        divider.color.b,
        divider.color.a
    );

    windowDrawList->AddLine(lineStartPosition, lineEndPosition, color, divider.height);
}

void Gui::DrawDropDownMenu(const DropDownMenu& dropDownMenu) const
{
    ImVec2 triggerElementBoundingBoxTopLeftCornerPosition = ImGui::GetItemRectMin();
    ImVec2 triggerElementBoundingBoxBottomRightCornerPosition = ImGui::GetItemRectMax();

    // Gets drop down menu inner rect (content rect) corners
    ImVec2 minCornerPosition = ImVec2(
        triggerElementBoundingBoxTopLeftCornerPosition.x + dropDownMenu.outerPadding.x + dropDownMenu.originOffset.x,
        triggerElementBoundingBoxBottomRightCornerPosition.y + dropDownMenu.outerPadding.y + dropDownMenu.originOffset.y
    );
    ImVec2 maxCornerPosition = ImVec2(
        minCornerPosition.x + dropDownMenu.size.x,
        minCornerPosition.y + dropDownMenu.size.y+ (dropDownMenu.lineHeight * dropDownMenu.items.size())
    );

    /**
     * NOTES:
     *  - Gets drop down menu outer rect (padding rect) corners
     *  - Used to apply padding outside of content rect
     */
    ImVec2 outerPaddedMinCornerPosition = ImVec2(
        minCornerPosition.x - dropDownMenu.outerPadding.x,
        minCornerPosition.y - dropDownMenu.outerPadding.y
    );
    ImVec2 outerPaddedMaxCornerPosition = ImVec2(
        maxCornerPosition.x + dropDownMenu.outerPadding.x,
        maxCornerPosition.y + dropDownMenu.outerPadding.y
    );

    ImDrawList* foregroundDrawList = ImGui::GetForegroundDrawList();

    ImU32 bgColor = IM_COL32(
        dropDownMenu.bgColor.r,
        dropDownMenu.bgColor.g,
        dropDownMenu.bgColor.b,
        dropDownMenu.bgColor.a
    );

    // Draws drop down menu outer rect (outer padding rect)
    foregroundDrawList->AddRectFilled(
        outerPaddedMinCornerPosition,
        outerPaddedMaxCornerPosition,
        bgColor,
        dropDownMenu.cornerRounding,
        ImDrawFlags_RoundCornersAll
    );

    // Draws drop down menu inner rect (content rect)
    foregroundDrawList->AddRectFilled(
        minCornerPosition,
        maxCornerPosition,
        bgColor,
        dropDownMenu.cornerRounding,
        ImDrawFlags_RoundCornersAll
    );

    // Draws drop down menu border
    ImU32 borderColor = IM_COL32(
        dropDownMenu.border.color.r,
        dropDownMenu.border.color.g,
        dropDownMenu.border.color.b,
        dropDownMenu.border.color.a
    );
    foregroundDrawList->AddRect(
        outerPaddedMinCornerPosition,
        outerPaddedMaxCornerPosition,
        borderColor,
        dropDownMenu.cornerRounding,
        ImDrawFlags_RoundCornersAll,
        dropDownMenu.border.height
    );

    // Draws drop down menu items
    // NOTE: Bounding box is not applied on the border
    ImVec2 textClickableBoundingBoxMinCornerPosition = ImVec2(
        outerPaddedMinCornerPosition.x + dropDownMenu.border.height,
        minCornerPosition.y
    );
    for (std::shared_ptr<DropDownMenuItem> dropDownMenuItem : dropDownMenu.items)
    {
        // Draws centered text
        float textPositionOffsetY = dropDownMenu.lineHeight * 0.5f;
        ImVec2 centeredTextPosition = ImVec2(minCornerPosition.x, textClickableBoundingBoxMinCornerPosition.y + textPositionOffsetY);
        ImU32 textColor = IM_COL32(
            dropDownMenuItem->textColor.r,
            dropDownMenuItem->textColor.g,
            dropDownMenuItem->textColor.b,
            dropDownMenuItem->textColor.a
        );
        foregroundDrawList->AddText(
            centeredTextPosition,
            textColor,
            dropDownMenuItem->text.c_str()
        );

        // Draws rect bounding box for click and hover events
        ImVec2 textSize = ImGui::CalcTextSize(dropDownMenuItem->text.c_str());
        float textClickableBoundingBoxHeight = textSize.y + dropDownMenu.lineHeight;
        // NOTE: Bounding box is not applied on the border
        ImVec2 textClickableBoundingBoxMaxCornerPosition = ImVec2(
            outerPaddedMaxCornerPosition.x - dropDownMenu.border.height,
            textClickableBoundingBoxMinCornerPosition.y + textClickableBoundingBoxHeight
        );
        ImRect textClickableBoundingBox(textClickableBoundingBoxMinCornerPosition, textClickableBoundingBoxMaxCornerPosition);

        const bool isHovered = textClickableBoundingBox.Contains(ImGui::GetIO().MousePos);
        // NOTE: 0 for left mouse button
        const bool isClicked = isHovered && ImGui::IsMouseClicked(0);

        if (isClicked && !!dropDownMenuItem->OnClick) dropDownMenuItem->OnClick();
        if (isHovered)
        {
            if (!dropDownMenuItem->bgColorHovered.IsEmpty())
            {
                ImU32 bgColorHovered = IM_COL32(
                    dropDownMenuItem->bgColorHovered.r,
                    dropDownMenuItem->bgColorHovered.g,
                    dropDownMenuItem->bgColorHovered.b,
                    dropDownMenuItem->bgColorHovered.a
                );
                foregroundDrawList->AddRectFilled(
                    textClickableBoundingBoxMinCornerPosition,
                    textClickableBoundingBoxMaxCornerPosition,
                    bgColorHovered,
                    0.0f
                );

                foregroundDrawList->AddText(
                    centeredTextPosition,
                    textColor,
                    dropDownMenuItem->text.c_str()
                );
            }

            if (!dropDownMenuItem->textColorHovered.IsEmpty())
            {
                ImU32 textColorHovered = IM_COL32(
                    dropDownMenuItem->textColorHovered.r,
                    dropDownMenuItem->textColorHovered.g,
                    dropDownMenuItem->textColorHovered.b,
                    dropDownMenuItem->textColorHovered.a
                );
                foregroundDrawList->AddText(
                    centeredTextPosition,
                    textColorHovered,
                    dropDownMenuItem->text.c_str()
                );
            }

            if (!!dropDownMenuItem->OnHover) dropDownMenuItem->OnHover();
        }

        // Sets next drop down menu item y coordinate
        textClickableBoundingBoxMinCornerPosition.y += textClickableBoundingBoxHeight;
    }
}

void Gui::DrawImage(const Image& image) const
{
    ImagePositioned imagePositioned = {};
    imagePositioned.image = image;
    imagePositioned.position = ToVector2(ImGui::GetCursorScreenPos());
    DrawImagePositioned(imagePositioned);
}

void Gui::DrawImageButton(ImageButton& imageButton) const
{
    Image imageButtonImage = imageButton.image;

    ImGui::InvisibleButton(imageButton.id.c_str(), ToImVec2(imageButton.image.size));
    if (ImGui::IsItemClicked()) imageButton.OnClick();
    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        if (!imageButton.tintColorHovered.IsEmpty()) imageButtonImage.tintColor = imageButton.tintColorHovered;
    }

    ImagePositioned imagePositioned = {};
    imagePositioned.image = imageButtonImage;
    // NOTE: position image to bounding box min position
    imagePositioned.position = ToVector2(ImGui::GetItemRectMin());
    DrawImagePositioned(imagePositioned);
}

void Gui::DrawModal(Modal& modal) const
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_None;
    if (!modal.canSaveSettings) flags |= ImGuiWindowFlags_NoSavedSettings;
    if (!modal.isTitlebarVisible) flags |= ImGuiWindowFlags_NoTitleBar;
    if (!modal.isScrollbarVisible) flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    if (!modal.isResizable) flags |= ImGuiWindowFlags_NoResize;
    if (!modal.isCollapsible) flags |= ImGuiWindowFlags_NoCollapse;
    if (!modal.isMovable) flags |= ImGuiWindowFlags_NoMove;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ToImVec2(modal.padding));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, modal.cornerRounding);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ToImVec4(modal.bgColor.ToVector4()));

    ImGui::SetNextWindowSize(ToImVec2(modal.size));
    if (ImGui::BeginPopupModal(modal.id.c_str(), nullptr, flags))
    {
        DrawContainer(modal.headerContainer);
        DrawContainer(modal.bodyContainer);

        ImGui::EndPopup();
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(1);
}

void Gui::DrawNode(const Node& node) const
{
    if (ImGui::TreeNode(node.name.c_str()))
    {
        node.DrawContent();
        ImGui::TreePop();
    }
}

void Gui::DrawText(Text& text) const
{
    ImGui::PushStyleColor(ImGuiCol_Text, ToImVec4(text.color.ToVector4()));
    if (!!text.height)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImFont* font = io.Fonts->AddFontFromFileTTF("../../assets/Audiowide-Regular.ttf", text.height);
        ImGui::PushFont(font);
    }

    ImGui::TextUnformatted(text.value.c_str());

    ImGui::PopStyleColor(1);
    if (!!text.height) ImGui::PopFont();
}

void Gui::DrawTextWrapped(Text& text) const
{
    ImGui::PushStyleColor(ImGuiCol_Text, ToImVec4(text.color.ToVector4()));
    if (!!text.height)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImFont* font = io.Fonts->AddFontFromFileTTF("../../assets/Audiowide-Regular.ttf", text.height);
        ImGui::PushFont(font);
    }

    ImGui::TextWrapped("%s", text.value.c_str());

    ImGui::PopStyleColor(1);
    if (!!text.height) ImGui::PopFont();
}

void Gui::DrawTextInputMultiline(std::string& value, TextInputMultiline& textInputMultiline) const
{
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, textInputMultiline.textInput.border.height);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ToImVec2(textInputMultiline.textInput.padding));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, textInputMultiline.textInput.cornerRounding);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ToImVec4(textInputMultiline.textInput.bgColor.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_Text, ToImVec4(textInputMultiline.textInput.textColor.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_Border, ToImVec4(textInputMultiline.textInput.border.color.ToVector4()));

    // NOTE: ## prefix tells ImGui to use the string for internal id generation but not to display it as a visible label
    const std::string& id = "##" + textInputMultiline.textInput.id;
    ImGui::InputTextMultiline(id.c_str(), &value, ToImVec2(textInputMultiline.size));
    if (ImGui::IsItemClicked() && !!textInputMultiline.textInput.OnClick) textInputMultiline.textInput.OnClick();

    // Draws placeholder on top of Text input multiline
    if (!textInputMultiline.textInput.placeholder.text.empty() && value.size() == 0) DrawPlaceholder(textInputMultiline.textInput.placeholder);

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);
}

void Gui::DrawTextInputSingleline(std::string& value, TextInputSingleline& textInputSingleline) const
{
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, textInputSingleline.textInput.border.height);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ToImVec2(textInputSingleline.textInput.padding));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, textInputSingleline.textInput.cornerRounding);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ToImVec4(textInputSingleline.textInput.bgColor.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_Text, ToImVec4(textInputSingleline.textInput.textColor.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_Border, ToImVec4(textInputSingleline.textInput.border.color.ToVector4()));
    ImGui::PushItemWidth(textInputSingleline.width);

    // NOTE: ## prefix tells ImGui to use the string for internal id generation but not to display it as a visible label
    const std::string& id = "##" + textInputSingleline.textInput.id;
    ImGui::InputText(id.c_str(), &value);
    if (ImGui::IsItemClicked() && !!textInputSingleline.textInput.OnClick) textInputSingleline.textInput.OnClick();

    // Draws placeholder on top of Text input singlelie
    if (!textInputSingleline.textInput.placeholder.text.empty() && value.size() == 0) DrawPlaceholder(textInputSingleline.textInput.placeholder);

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);
    ImGui::PopItemWidth();
}

void Gui::DrawTreeNode(const TreeNode& rootTreeNode) const
{
    if (ImGui::TreeNode(rootTreeNode.name.c_str()))
    {
        for (const TreeNode& child : rootTreeNode.children)
        {
            DrawTreeNode(child);
        }
        ImGui::TreePop();
    }
}

void Gui::DrawWindow(Window& window) const
{
    bool isOpen = true;

    ImGuiWindowFlags flags = ImGuiWindowFlags_None;
    if (!window.canSaveSettings) flags |= ImGuiWindowFlags_NoSavedSettings;
    if (!window.isTitlebarVisible) flags |= ImGuiWindowFlags_NoTitleBar;
    if (!window.isScrollbarVisible) flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    if (!window.isResizable) flags |= ImGuiWindowFlags_NoResize;
    if (!window.isCollapsible) flags |= ImGuiWindowFlags_NoCollapse;
    if (!window.isMovable) flags |= ImGuiWindowFlags_NoMove;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ToImVec2(window.padding));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ToImVec4(window.bgColor.ToVector4()));

    ImGui::SetNextWindowPos(ToImVec2(window.position));
    ImGui::SetNextWindowSize(ToImVec2(window.size));
    if (ImGui::Begin(window.name.c_str(), &isOpen, flags)) window.DrawContent();
    ImGui::End();

    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(1);
}

/**
 * NOTES:
 *  - Raw elements
 *  - Those elements are drawn on the foreground without being clipped to any parent elements
 *  - They will always be drawn on top of any other elements
 */
void Gui::DrawRawText(const RawText& rawText) const
{
    ImDrawList* foregroundDrawList = ImGui::GetForegroundDrawList();

    ImU32 color = IM_COL32(
        rawText.color.r,
        rawText.color.g,
        rawText.color.b,
        rawText.color.a
    );

    foregroundDrawList->AddText(
        ToImVec2(rawText.position),
        color,
        rawText.value.c_str()
    );
}

void Gui::DrawRawWindow(const RawWindow& rawWindow) const
{
    ImDrawList* foregroundDrawList = ImGui::GetForegroundDrawList();

    ImU32 bgColor = IM_COL32(
        rawWindow.bgColor.r,
        rawWindow.bgColor.g,
        rawWindow.bgColor.b,
        rawWindow.bgColor.a
    );

    foregroundDrawList->AddRectFilled(
        ToImVec2(rawWindow.minCornerPosition),
        ToImVec2(rawWindow.maxCornerPosition),
        bgColor,
        rawWindow.cornerRounding,
        ImDrawFlags_RoundCornersAll
    );

    rawWindow.DrawContent();
}

// Modal helpers
bool Gui::AreAnyModalsOpen() const
{
    return !!m_openModalIds.size();
}

void Gui::OpenModal(const std::string& id)
{
    ImGui::OpenPopup(id.c_str());
    m_openModalIds.insert(std::pair<std::string, bool>(id, true));
}

void Gui::CloseModal(const std::string& id)
{
    ImGui::CloseCurrentPopup();
    m_openModalIds.erase(id);
}

// position helpers
void Gui::AlignCenter(Vector2 elementSize) const
{
    const Vector2 availableSpace = GetAvailableSpace();
    const Vector2 offset = (availableSpace - elementSize) * 0.5f;
    SetPosition(GetPosition() + offset);
}

void Gui::AlignCenterX(float elementWidth) const
{
    const Vector2 availableSpace = GetAvailableSpace();
    const float offsetX = (availableSpace.x - elementWidth) * 0.5f;
    SetPositionX(GetPositionX() + offsetX);
}

void Gui::AlignCenterY(float elementHeigth) const
{
    const Vector2 availableSpace = GetAvailableSpace();
    const float offsetY = (availableSpace.y - elementHeigth) * 0.5f;
    SetPositionY(GetPositionY() + offsetY);
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

// Dimension helpers
const Vector2 Gui::GetAvailableSpace() const
{
    return ToVector2(ImGui::GetContentRegionAvail());
}

const Vector2 Gui::GetParentContainerPaddingSize() const
{
    return ToVector2(ImGui::GetStyle().WindowPadding);
}

float Gui::GetTextInputSinglelineHeight() const
{
    return ImGui::GetFontSize() + ImGui::GetFrameHeight();
}

const Vector2 Gui::GetTextSize(const std::string& text) const
{
    return ToVector2(ImGui::CalcTextSize(text.c_str()));
}

const Vector2 Gui::GetTextSize(const Text& text) const
{
    if (!!text.height)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImFont* font = io.Fonts->AddFontFromFileTTF("../../assets/Audiowide-Regular.ttf", text.height);
        ImGui::PushFont(font);
    }

    ImVec2 textSize = ImGui::CalcTextSize(text.value.c_str());

    if (!!text.height) ImGui::PopFont();

    return ToVector2(textSize);
}

const Vector2 Gui::GetViewportSize() const
{
    return ToVector2(ImGui::GetIO().DisplaySize);
}

// Config helpers
float Gui::GetDeltaTime() const
{
    return ImGui::GetIO().DeltaTime;
}

// ***********
// * PRIVATE *
// ***********
void Gui::DrawImagePositioned(const ImagePositioned& imagePositioned) const
{
    // NOTE: Uses window drawlist instead of forground drawlist to avoid image being visible when scrolling out of sight
    ImDrawList* windowDrawList = ImGui::GetWindowDrawList();

    ImVec2 size = ImVec2(imagePositioned.position.x + imagePositioned.image.size.x, imagePositioned.position.y + imagePositioned.image.size.y);
    ImVec2 uvPositionStart = ImVec2(0.0f, 0.0f);
    ImVec2 uvPositionEnd = ImVec2(1.0f, 1.0f);
    ImU32 tintColor = IM_COL32(
        imagePositioned.image.tintColor.r,
        imagePositioned.image.tintColor.g,
        imagePositioned.image.tintColor.b,
        imagePositioned.image.tintColor.a
    );

    windowDrawList->AddImageRounded(
        imagePositioned.image.textureId,
        ToImVec2(imagePositioned.position),
        size,
        uvPositionStart,
        uvPositionEnd,
        tintColor,
        imagePositioned.image.cornerRounding,
        ImDrawFlags_RoundCornersAll
    );
}

void Gui::DrawPlaceholder(const Placeholder& placeholder) const
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec2 framePadding = style.FramePadding;

    ImDrawList* foregroundDrawList = ImGui::GetForegroundDrawList();
    ImVec2 boundingBoxMinPosition = ImGui::GetItemRectMin();

    ImVec2 placeholderPosition(
        boundingBoxMinPosition.x + framePadding.x,
        boundingBoxMinPosition.y + framePadding.y
    );
    ImU32 placeholderColor = IM_COL32(
        placeholder.color.r,
        placeholder.color.g,
        placeholder.color.b,
        placeholder.color.a
    );

    foregroundDrawList->AddText(placeholderPosition, placeholderColor, placeholder.text.c_str());
}

const Vector2 Gui::ToVector2(const ImVec2& imguiVec2) const
{
    return Vector2(imguiVec2.x, imguiVec2.y);
}

const Vector4 Gui::ToVector4(const ImVec4& imguiVec4) const
{
    return Vector4(imguiVec4.x, imguiVec4.y, imguiVec4.z, imguiVec4.w);
}

const ImVec2 Gui::ToImVec2(const Vector2& vector2) const
{
    return ImVec2(vector2.x, vector2.y);
}

const ImVec4 Gui::ToImVec4(const Vector4& vector4) const
{
    return ImVec4(vector4.x, vector4.y, vector4.z, vector4.w);
}
