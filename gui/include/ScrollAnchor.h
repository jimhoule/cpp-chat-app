#pragma once

#include "Gui.h"

#include <string>

/**
 * NOTES:
 *  - Keeps a scrolling list on the same item when items are prepended above it (loading older items on scroll up)
 *  - Owned by the layer because Gui elements are rebuilt every frame and hold no state
 *  - Measures on the second frame after the prepend because ImGui gives a child window no height on the frame it first appears
 *  - Settled means the content height did not change since the previous frame (until then the scroll position is stale)
 */
class ScrollAnchor
{
public:
    // NOTE: Correction measured on the previous frame and handed over once (Gui::UNSET when none)
    float ConsumeCorrectedScrollY();
    // NOTE: Called at the end of list's DrawContent with the cursor Y after the last item (moves to the next state and updates settled)
    void EndFrame(float contentHeight);
    // NOTE: True while an anchor waits to be measured (auto-scroll must not move the view)
    bool HasAnchor() const;
    // NOTE: True when the layout did not change since the previous frame (the load trigger must wait otherwise)
    bool IsSettled() const;
    // NOTE: Called before drawing each item (itemOffsetY is the item Y minus the first item Y)
    void MeasureItem(const std::string& itemId, float itemOffsetY, float scrollY);
    void Reset();
    // NOTE: Called right before items are prepended (anchorItemId is the current first item)
    void Start(const std::string& anchorItemId);

private:
    /**
     * NOTES:
     *  - IDLE: No anchor
     *  - WAITING_FOR_ITEM_HEIGHTS: Frame of the prepend (new item containers have no height yet)
     *  - MEASURING: Next frame (real heights and the anchor is measured)
     */
    enum class State
    {
        IDLE,
        WAITING_FOR_ITEM_HEIGHTS,
        MEASURING
    };

    std::string m_anchorItemId = "";
    float m_correctedScrollY = Gui::UNSET;
    float m_previousContentHeight = Gui::UNSET;
    bool m_isSettled = false;
    State m_state = State::IDLE;
};
