#include "Gui.h"

#include <imgui/imgui_internal.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// **********
// * PUBLIC *
// **********
void Gui::Init(GLFWwindow* GlfwWindow) const
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& Io = ImGui::GetIO();
    (void)Io;
    Io.Fonts->AddFontFromFileTTF("../../assets/Audiowide-Regular.ttf");

     // Setup Dear ImGui style
    ImGuiStyle& Style = ImGui::GetStyle();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(GlfwWindow, true);
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

void Gui::DrawButton(Button& Button) const
{
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, Button.Border.Height);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ToImVec2(Button.Padding));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, Button.CornerRounding);
    ImGui::PushStyleColor(ImGuiCol_Button, ToImVec4(Button.BgColor.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_Text, ToImVec4(Button.TextColor.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_Border, ToImVec4(Button.Border.Color.ToVector4()));
    if (!Button.BgColorActive.IsEmpty()) ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec4(Button.BgColorActive.ToVector4()));
    if (!Button.BgColorHovered.IsEmpty()) ImGui::PushStyleColor(ImGuiCol_ButtonActive, ToImVec4(Button.BgColorHovered.ToVector4()));

    if (Button.IsDisabled)  ImGui::BeginDisabled();

    ImGui::Button(Button.Label.c_str(), ImVec2(Button.Size.X, Button.Size.Y));
    if (ImGui::IsItemClicked()) Button.OnClick();
    if (ImGui::IsItemHovered() && !!Button.OnHover) Button.OnHover();

    if (Button.IsDisabled)  ImGui::EndDisabled();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);
    if (!Button.BgColorActive.IsEmpty()) ImGui::PopStyleColor(1);
    if (!Button.BgColorHovered.IsEmpty()) ImGui::PopStyleColor(1);
}

void Gui::DrawContainer(Container& Container) const
{
    ImGuiChildFlags Flags = ImGuiChildFlags_AlwaysUseWindowPadding;
    if (Container.IsAutoResizableY) Flags |= ImGuiChildFlags_AutoResizeY;
    if (Container.IsAutoResizableX) Flags |= ImGuiChildFlags_AutoResizeX;

    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, Container.Border.Height);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ToImVec2(Container.Padding));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, Container.CornerRounding);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ToImVec4(Container.BgColor.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_Border, ToImVec4(Container.Border.Color.ToVector4()));

    if (ImGui::BeginChild(Container.ID.c_str(), ToImVec2(Container.Size), Flags))
    {
        const bool IS_HOVERED = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
        // Draws rectangle on top of background container when is hovered
        if (IS_HOVERED && !Container.BgColorHovered.IsEmpty())
        {
            ImDrawList* WindowDrawList = ImGui::GetWindowDrawList();

            ImVec2 BgPositionMin = ImGui::GetCursorScreenPos();
            ImVec2 BgPositionMax = ImVec2(BgPositionMin.x + Container.Size.X, BgPositionMin.y + Container.Size.Y);
            ImU32 BgColorHovered = IM_COL32(
                Container.BgColorHovered.R,
                Container.BgColorHovered.G,
                Container.BgColorHovered.B,
                Container.BgColorHovered.A
            );

            WindowDrawList->AddRectFilled(
                BgPositionMin,
                BgPositionMax,
                BgColorHovered,
                Container.CornerRounding
            );
        }

        if (IS_HOVERED && !!Container.OnHover) Container.OnHover();

        ContainerState State = {};
        State.IsHovered = IS_HOVERED;
        Container.DrawContent(State);
    }
    ImGui::EndChild();

    // NOTE: IsItemClicked() refers to the item that was most recently defined. For a child window, the "item" representing the whole child rectangle is defined by the BeginChild() and EndChild() calls
    const bool IS_CLICKED = ImGui::IsItemClicked();
    if (IS_CLICKED && !!Container.OnClick) Container.OnClick();

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

void Gui::DrawDivider(const Divider& Divider) const
{
    ImDrawList* WindowDrawList = ImGui::GetWindowDrawList();
    ImVec2 LineStartPosition = ImGui::GetCursorScreenPos();
    ImVec2 LineEndPosition = ImVec2(LineStartPosition.x + ImGui::GetWindowWidth(), LineStartPosition.y);
    ImU32 Color = IM_COL32(
        Divider.Color.R,
        Divider.Color.G,
        Divider.Color.B,
        Divider.Color.A
    );

    WindowDrawList->AddLine(LineStartPosition, LineEndPosition, Color, Divider.Height);
}

// NOTE: Draws corners from right to left (top right corner to bottom left corner)
void Gui::DrawDropDownMenu(const DropDownMenu& DropDownMenu) const
{
    ImVec2 TriggerElementBoundingBoxBottomRightCornerPosition = ImGui::GetItemRectMax();

    // Gets drop down menu inner rect (content rect) corners (top right corner and bottom left corner)
    ImVec2 MinCornerPosition = ImVec2(
        TriggerElementBoundingBoxBottomRightCornerPosition.x - DropDownMenu.OuterPadding.X,
        TriggerElementBoundingBoxBottomRightCornerPosition.y + DropDownMenu.OuterPadding.Y
    );
    ImVec2 MaxCornerPosition = ImVec2(
        TriggerElementBoundingBoxBottomRightCornerPosition.x - DropDownMenu.Size.X - DropDownMenu.OuterPadding.X,
        TriggerElementBoundingBoxBottomRightCornerPosition.y + DropDownMenu.Size.Y + DropDownMenu.OuterPadding.Y + (DropDownMenu.LineHeight * DropDownMenu.Items.size())
    );

    // Gets drop down menu outer rect (padding rect) corners (top right corner and bottom left corner)
    ImVec2 PaddedMinCornerPosition = ImVec2(
        MinCornerPosition.x + DropDownMenu.OuterPadding.X,
        MinCornerPosition.y - DropDownMenu.OuterPadding.Y
    );
    ImVec2 PaddedMaxCornerPosition = ImVec2(
        MaxCornerPosition.x - DropDownMenu.OuterPadding.X,
        MaxCornerPosition.y + DropDownMenu.OuterPadding.Y
    );

    ImDrawList* ForegroundDrawList = ImGui::GetForegroundDrawList();

    /**
     * NOTES:
     *  - Corner rounding is always 0 as it is deactivated for this component
     *  - Because we draw from top right corner to bottom left corner we would need negative
     *    values for corner rounding
     *  - Corner rounding values in ImGui cannot be negative number
     */
    float CornerRounding = 0.0f;

    ImU32 BgColor = IM_COL32(
        DropDownMenu.BgColor.R,
        DropDownMenu.BgColor.G,
        DropDownMenu.BgColor.B,
        DropDownMenu.BgColor.A
    );

    // Draws drop down menu outer rect (padding rect)
    ForegroundDrawList->AddRectFilled(
        PaddedMinCornerPosition,
        PaddedMaxCornerPosition,
        BgColor,
        CornerRounding,
        ImDrawFlags_RoundCornersAll
    );

    // Draws drop down menu inner rect (content rect)
    ForegroundDrawList->AddRectFilled(
        MinCornerPosition,
        MaxCornerPosition,
        BgColor,
        CornerRounding,
        ImDrawFlags_RoundCornersAll
    );

    // Draws drop down menu border
    ImU32 BorderColor = IM_COL32(
        DropDownMenu.Border.Color.R,
        DropDownMenu.Border.Color.G,
        DropDownMenu.Border.Color.B,
        DropDownMenu.Border.Color.A
    );
    ForegroundDrawList->AddRect(
        PaddedMinCornerPosition,
        PaddedMaxCornerPosition,
        BorderColor,
        CornerRounding,
        ImDrawFlags_RoundCornersAll,
        DropDownMenu.Border.Height
    );

    // Draws drop down menu items
    ImVec2 TextClickableBoundingBoxMinCornerPosition = ImVec2(PaddedMaxCornerPosition.x, MinCornerPosition.y);
    for (std::shared_ptr<DropDownMenuItem> DropDownMenuItem : DropDownMenu.Items)
    {
        // Draws centered text
        float TextPositionOffsetY = DropDownMenu.LineHeight * 0.5f;
        ImVec2 CenteredTextPosition = ImVec2(MaxCornerPosition.x, TextClickableBoundingBoxMinCornerPosition.y + TextPositionOffsetY);
        ImU32 TextColor = IM_COL32(
            DropDownMenuItem->TextColor.R,
            DropDownMenuItem->TextColor.G,
            DropDownMenuItem->TextColor.B,
            DropDownMenuItem->TextColor.A
        );
        ForegroundDrawList->AddText(
            CenteredTextPosition,
            TextColor,
            DropDownMenuItem->Text.c_str()
        );

        // Draws rect bounding box for click and hover events
        ImVec2 TextSize = ImGui::CalcTextSize(DropDownMenuItem->Text.c_str());
        float TextClickableBoundingBoxHeight = TextSize.y + DropDownMenu.LineHeight;
        ImVec2 TextClickableBoundingBoxMaxCornerPosition = ImVec2(
            PaddedMinCornerPosition.x,
            TextClickableBoundingBoxMinCornerPosition.y + TextClickableBoundingBoxHeight
        );
        ImRect TextClickableBoundingBox(TextClickableBoundingBoxMinCornerPosition, TextClickableBoundingBoxMaxCornerPosition);

        const bool IS_HOVERED = TextClickableBoundingBox.Contains(ImGui::GetIO().MousePos);
        // NOTE: 0 for left mouse button
        const bool IS_CLICKED = IS_HOVERED && ImGui::IsMouseClicked(0);

        if (IS_CLICKED && !!DropDownMenuItem->OnClick) DropDownMenuItem->OnClick();
        if (IS_HOVERED)
        {
            if (!DropDownMenuItem->BgColorHovered.IsEmpty())
            {
                ImU32 BgColorHovered = IM_COL32(
                    DropDownMenuItem->BgColorHovered.R,
                    DropDownMenuItem->BgColorHovered.G,
                    DropDownMenuItem->BgColorHovered.B,
                    DropDownMenuItem->BgColorHovered.A
                );
                ForegroundDrawList->AddRectFilled(
                    TextClickableBoundingBoxMinCornerPosition,
                    TextClickableBoundingBoxMaxCornerPosition,
                    BgColorHovered,
                    0.0f
                );

                ForegroundDrawList->AddText(
                    CenteredTextPosition,
                    TextColor,
                    DropDownMenuItem->Text.c_str()
                );
            }

            if (!DropDownMenuItem->TextColorHovered.IsEmpty())
            {
                ImU32 TextColorHovered = IM_COL32(
                    DropDownMenuItem->TextColorHovered.R,
                    DropDownMenuItem->TextColorHovered.G,
                    DropDownMenuItem->TextColorHovered.B,
                    DropDownMenuItem->TextColorHovered.A
                );
                ForegroundDrawList->AddText(
                    CenteredTextPosition,
                    TextColorHovered,
                    DropDownMenuItem->Text.c_str()
                );
            }

            if (!!DropDownMenuItem->OnHover) DropDownMenuItem->OnHover();
        }

        // Sets next drop down menu item Y coordinate
        TextClickableBoundingBoxMinCornerPosition.y += TextClickableBoundingBoxHeight;
    }
}

void Gui::DrawImage(const Image& Image) const
{
    ImagePositioned ImagePositioned = {};
    ImagePositioned.Image = Image;
    ImagePositioned.Position = ToVector2(ImGui::GetCursorScreenPos());
    DrawImagePositioned(ImagePositioned);
}

void Gui::DrawImageButton(ImageButton& ImageButton) const
{
    Image ImageButtonImage = ImageButton.Image;

    ImGui::InvisibleButton(ImageButton.ID.c_str(), ToImVec2(ImageButton.Image.Size));
    if (ImGui::IsItemClicked()) ImageButton.OnClick();
    if (ImGui::IsItemHovered() && !ImageButton.TintColorHovered.IsEmpty()) ImageButtonImage.TintColor = ImageButton.TintColorHovered;

    ImagePositioned ImagePositioned = {};
    ImagePositioned.Image = ImageButtonImage;
    // NOTE: Position image to bounding box min position
    ImagePositioned.Position = ToVector2(ImGui::GetItemRectMin());
    DrawImagePositioned(ImagePositioned);
}

void Gui::DrawModal(Modal& Modal) const
{
    ImGuiWindowFlags Flags = ImGuiWindowFlags_None;
    if (!Modal.CanSaveSettigs) Flags |= ImGuiWindowFlags_NoSavedSettings;
    if (!Modal.IsTitlebarVisible) Flags |= ImGuiWindowFlags_NoTitleBar;
    if (!Modal.IsScrollbarVisible) Flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    if (!Modal.IsResizable) Flags |= ImGuiWindowFlags_NoResize;
    if (!Modal.IsCollapsible) Flags |= ImGuiWindowFlags_NoCollapse;
    if (!Modal.IsMovable) Flags |= ImGuiWindowFlags_NoMove;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ToImVec2(Modal.Padding));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, Modal.CornerRounding);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ToImVec4(Modal.BgColor.ToVector4()));

    ImGui::SetNextWindowSize(ToImVec2(Modal.Size));
    if (ImGui::BeginPopupModal(Modal.ID.c_str(), nullptr, Flags))
    {
        DrawContainer(Modal.HeaderContainer);
        DrawContainer(Modal.BodyContainer);

        ImGui::EndPopup();
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(1);
}

void Gui::DrawNode(const Node& Node) const
{
    if (ImGui::TreeNode(Node.Name.c_str()))
    {
        Node.DrawContent();
        ImGui::TreePop();
    }
}

void Gui::DrawText(const Text& Text) const
{
    ImGui::TextUnformatted(Text.Value.c_str());
}

void Gui::DrawTextWrapped(const Text& Text) const
{
    ImGui::TextWrapped("%s", Text.Value.c_str());
}

void Gui::DrawTextInputMultiline(std::string& Value, TextInputMultiline& TextInputMultiline) const
{
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, TextInputMultiline.TextInput.Border.Height);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ToImVec2(TextInputMultiline.TextInput.Padding));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, TextInputMultiline.TextInput.CornerRounding);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ToImVec4(TextInputMultiline.TextInput.BgColor.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_Text, ToImVec4(TextInputMultiline.TextInput.TextColor.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_Border, ToImVec4(TextInputMultiline.TextInput.Border.Color.ToVector4()));

    // NOTE: ## prefix tells ImGui to use the string for internal ID generation but not to display it as a visible label
    const std::string& ID = "##" + TextInputMultiline.TextInput.ID;
    ImGui::InputTextMultiline(ID.c_str(), &Value, ToImVec2(TextInputMultiline.Size));
    if (ImGui::IsItemClicked() && !!TextInputMultiline.TextInput.OnClick) TextInputMultiline.TextInput.OnClick();

    // Draws placeholder on top of Text input multiline
    if (!TextInputMultiline.TextInput.Placeholder.Text.empty() && Value.size() == 0) DrawPlaceholder(TextInputMultiline.TextInput.Placeholder);

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);
}

void Gui::DrawTextInputSingleline(std::string& Value, TextInputSingleline& TextInputSingleline) const
{
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, TextInputSingleline.TextInput.Border.Height);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ToImVec2(TextInputSingleline.TextInput.Padding));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, TextInputSingleline.TextInput.CornerRounding);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ToImVec4(TextInputSingleline.TextInput.BgColor.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_Text, ToImVec4(TextInputSingleline.TextInput.TextColor.ToVector4()));
    ImGui::PushStyleColor(ImGuiCol_Border, ToImVec4(TextInputSingleline.TextInput.Border.Color.ToVector4()));
    ImGui::PushItemWidth(TextInputSingleline.Width);

    // NOTE: ## prefix tells ImGui to use the string for internal ID generation but not to display it as a visible label
    const std::string& ID = "##" + TextInputSingleline.TextInput.ID;
    ImGui::InputText(ID.c_str(), &Value);
    if (ImGui::IsItemClicked() && !!TextInputSingleline.TextInput.OnClick) TextInputSingleline.TextInput.OnClick();

    // Draws placeholder on top of Text input singlelie
    if (!TextInputSingleline.TextInput.Placeholder.Text.empty() && Value.size() == 0) DrawPlaceholder(TextInputSingleline.TextInput.Placeholder);

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);
    ImGui::PopItemWidth();
}

void Gui::DrawTreeNode(const TreeNode& RootTreeNode) const
{
    if (ImGui::TreeNode(RootTreeNode.Name.c_str()))
    {
        for (const TreeNode& Child : RootTreeNode.Children)
        {
            DrawTreeNode(Child);
        }
        ImGui::TreePop();
    }
}

void Gui::DrawWindow(Window& Window) const
{
    bool IsOpen = true;

    ImGuiWindowFlags Flags = ImGuiWindowFlags_None;
    if (!Window.CanSaveSettigs) Flags |= ImGuiWindowFlags_NoSavedSettings;
    if (!Window.IsTitlebarVisible) Flags |= ImGuiWindowFlags_NoTitleBar;
    if (!Window.IsScrollbarVisible) Flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    if (!Window.IsResizable) Flags |= ImGuiWindowFlags_NoResize;
    if (!Window.IsCollapsible) Flags |= ImGuiWindowFlags_NoCollapse;
    if (!Window.IsMovable) Flags |= ImGuiWindowFlags_NoMove;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ToImVec2(Window.Padding));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ToImVec4(Window.BgColor.ToVector4()));

    ImGui::SetNextWindowPos(ToImVec2(Window.Position));
    ImGui::SetNextWindowSize(ToImVec2(Window.Size));
    if (ImGui::Begin(Window.Name.c_str(), &IsOpen, Flags)) Window.DrawContent();
    ImGui::End();

    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(1);
}

bool Gui::AreAnyModalsOpen() const
{
    return !!OpenModalIds.size();
}

void Gui::OpenModal(const std::string& ID)
{
    ImGui::OpenPopup(ID.c_str());
    OpenModalIds.insert(std::pair<std::string, bool>(ID, true));
}

void Gui::CloseModal(const std::string& ID)
{
    ImGui::CloseCurrentPopup();
    OpenModalIds.erase(ID);
}

void Gui::AlignCenter(Vector2 ElementSize) const
{
    const Vector2 AVAILABLE_SPACE = GetAvailableSpace();
    const Vector2 OFFSET = (AVAILABLE_SPACE - ElementSize) * 0.5f;
    SetPosition(GetPosition() + OFFSET);
}

void Gui::AlignCenterX(float ElementWidth) const
{
    const Vector2 AVAILABLE_SPACE = GetAvailableSpace();
    const float OFFSET_X = (AVAILABLE_SPACE.X - ElementWidth) * 0.5f;
    SetPositionX(GetPositionX() + OFFSET_X);
}

void Gui::AlignCenterY(float ElementHeigth) const
{
    const Vector2 AVAILABLE_SPACE = GetAvailableSpace();
    const float OFFSET_Y = (AVAILABLE_SPACE.Y - ElementHeigth) * 0.5f;
    SetPositionY(GetPositionY() + OFFSET_Y);
}

void Gui::DisplayInline() const
{
    ImGui::SameLine();
}

const Vector2 Gui::GetTextSize(const std::string& Text) const
{
    return ToVector2(ImGui::CalcTextSize(Text.c_str()));
}

float Gui::GetTextInputSinglelineHeight() const
{
    return ImGui::GetFontSize() + ImGui::GetFrameHeight();
}

void Gui::ScrollToY(float Y) const
{
    ImGui::SetScrollHereY(Y);
}

void Gui::ScrollToX(float X) const
{
    ImGui::SetScrollHereX(X);
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

const Vector2 Gui::GetViewportSize() const
{
    return ToVector2(ImGui::GetIO().DisplaySize);
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

void Gui::SetPosition(Vector2 Position) const
{
    ImGui::SetCursorPos(ToImVec2(Position));
}

void Gui::SetPositionX(float X) const
{
    ImGui::SetCursorPosX(X);
}

void Gui::SetPositionY(float Y) const
{
    ImGui::SetCursorPosY(Y);
}

// ***********
// * PRIVATE *
// ***********
void Gui::DrawImagePositioned(const ImagePositioned& ImagePositioned) const
{
    // NOTE: Uses window drawlist instead of forground drawlist to avoid image being visible when scrolling out of sight
    ImDrawList* WindowDrawList = ImGui::GetWindowDrawList();

    ImVec2 Size = ImVec2(ImagePositioned.Position.X + ImagePositioned.Image.Size.X, ImagePositioned.Position.Y + ImagePositioned.Image.Size.Y);
    ImVec2 UvPositionStart = ImVec2(0.0f, 0.0f);
    ImVec2 UvPositionEnd = ImVec2(1.0f, 1.0f);
    ImU32 TintColor = IM_COL32(
        ImagePositioned.Image.TintColor.R,
        ImagePositioned.Image.TintColor.G,
        ImagePositioned.Image.TintColor.B,
        ImagePositioned.Image.TintColor.A
    );

    WindowDrawList->AddImageRounded(
        ImagePositioned.Image.TextureID,
        ToImVec2(ImagePositioned.Position),
        Size,
        UvPositionStart,
        UvPositionEnd,
        TintColor,
        ImagePositioned.Image.CornerRounding,
        ImDrawFlags_RoundCornersAll
    );
}

void Gui::DrawPlaceholder(const Placeholder& Placeholder) const
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec2 FramePadding = style.FramePadding;

    ImDrawList* ForegroundDrawList = ImGui::GetForegroundDrawList();
    ImVec2 BoundingBoxMinPosition = ImGui::GetItemRectMin();

    ImVec2 PlaceholderPosition(
        BoundingBoxMinPosition.x + FramePadding.x,
        BoundingBoxMinPosition.y + FramePadding.y
    );
    ImU32 PlaceholderColor = IM_COL32(
        Placeholder.Color.R,
        Placeholder.Color.G,
        Placeholder.Color.B,
        Placeholder.Color.A
    );

    ForegroundDrawList->AddText(PlaceholderPosition, PlaceholderColor, Placeholder.Text.c_str());
}

const Vector2 Gui::ToVector2(const ImVec2& ImguiVec2) const
{
    return Vector2(ImguiVec2.x, ImguiVec2.y);
}

const Vector4 Gui::ToVector4(const ImVec4& ImguiVec4) const
{
    return Vector4(ImguiVec4.x, ImguiVec4.y, ImguiVec4.z, ImguiVec4.w);
}

const ImVec2 Gui::ToImVec2(const Vector2& Vector2) const
{
    return ImVec2(Vector2.X, Vector2.Y);
}

const ImVec4 Gui::ToImVec4(const Vector4& Vector4) const
{
    return ImVec4(Vector4.X, Vector4.Y, Vector4.Z, Vector4.W);
}
