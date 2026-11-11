#include "ScrollAnchor.h"

#include <cmath>
#include <utility>

// NOTE: Content height is computed with float scroll values so tiny differences do not count as changes
constexpr float CONTENT_HEIGHT_TOLERANCE = 1.0f;

// **********
// * PUBLIC *
// **********
float ScrollAnchor::ConsumeCorrectedScrollY()
{
    // NOTE: exchange hands the correction over and resets it so it is applied once
    return std::exchange(m_correctedScrollY, Gui::UNSET);
}

void ScrollAnchor::EndFrame(float contentHeight)
{
    m_isSettled = std::abs(contentHeight - m_previousContentHeight) < CONTENT_HEIGHT_TOLERANCE;
    m_previousContentHeight = contentHeight;

    if (m_state == State::WAITING_FOR_ITEM_HEIGHTS)
    {
        m_state = State::MEASURING;
        return;
    }

    if (m_state == State::MEASURING)
    {
        // NOTE: Anchor is measured by now
        m_state = State::IDLE;
        m_anchorItemId.clear();
    }
}

bool ScrollAnchor::HasAnchor() const
{
    return !m_anchorItemId.empty();
}

bool ScrollAnchor::IsSettled() const
{
    return m_isSettled;
}

void ScrollAnchor::MeasureItem(const std::string& itemId, float itemOffsetY, float scrollY)
{
    // NOTE: Scroll is still the pre prepend one where the anchor was the first item (offset 0)
    if (m_state != State::MEASURING || itemId != m_anchorItemId)
    {
        return;
    }

    m_correctedScrollY = scrollY + itemOffsetY;
}

void ScrollAnchor::Reset()
{
    m_anchorItemId.clear();
    m_previousContentHeight = Gui::UNSET;
    m_correctedScrollY = Gui::UNSET;
    m_isSettled = false;
    m_state = State::IDLE;
}

void ScrollAnchor::Start(const std::string& anchorItemId)
{
    m_anchorItemId = anchorItemId;
    m_state = State::WAITING_FOR_ITEM_HEIGHTS;
}
