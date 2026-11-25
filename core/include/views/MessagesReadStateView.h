#pragma once

#include <cstdint>
#include <string>

struct MessagesReadStateView
{
    std::string conversationId;
    /**
     * NOTES:
     *  - Newest message position of the conversation on the server from any sender (0 if no messages)
     *  - Not the message the client marked (that is the newest on the client's screen and 
     *    newer messages may have reached the server without being displayed yet)
     *  - Used by the client's double-count guard (a live message adds 1 only if its position is higher)
     */
    std::uint64_t latestMessagePosition;
    /**
     * NOTES:
     *  - Position of newest message read
     *  - Last read position as stored after the call so not necessarily the message the
     *    client marked (higher if another device already marked further)
     *  - Used by the client for the new messages divider
     */
    std::uint64_t lastReadMessagePosition;
    std::uint64_t unreadCount;
};
