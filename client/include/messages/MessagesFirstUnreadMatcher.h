#pragma once

#include "views/MessageView.h"
#include "views/MessagesReadStateView.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

/**
 * NOTES:
 *  - Finds the first unread message of the selected conversation (ChatLayer draws the unread messages divider above it)
 *  - Uses the read position captured when the conversation was selected, not the current one (MarkRead() moves the current
 *    one right away and the divider must stay put while the conversation is selected)
 *  - Has no GUI or network dependencies
 */
class MessagesFirstUnreadMatcher
{
public:
    /**
     * NOTES:
     *  - Captures lastReadMessagePosition once per selection (ignored if already captured)
     *  - Ignored if nullptr (no read state yet and will be captured and it arrives)
     *  - The caller only passes the selected conversation's read state
     */
    void Capture(const MessagesReadStateView* messagesReadState);
    // NOTE: Called once after the draw loop so the next frame looks for the first unread message again
    void EndFrame();
    /**
     * NOTES:
     *  - True only for the first message after the captured read position not sent by the current user
     *  - False for every message once it matched the frame or while nothing is captured
     */
    bool Match(const MessageView& message, const std::string& currentUserId);
    // NOTE: Called on selection (before Capture()), close and logout
    void Reset();

private:
    // NOTE: True once the first uread message was matched for this frame
    bool m_isMatched = false;
    // NOTE: nullpot until captured (0 is a real position meaning nothing read yet)
    std::optional<std::uint64_t> m_lastReadMessagePosition = std::nullopt;
};
