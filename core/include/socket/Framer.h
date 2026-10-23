#pragma once

#include <cstddef>
#include <string>
#include <vector>

/**
 * NOTES:
 *  - Length prefix framing: [uint32 payload length prefix, network byte order][payload bytes]
 *  - One Framer per socket, each socket is its own byte stream
 *  - std::string as a byte container, can hold binary payloads
 */
class Framer
{
public:
    /**
     * NOTES:
     *  - Builds a frame: [4 bytes payload length prefix][payload]
     *  - TCP is a byte stream so the reader cannot see where a message ends
     *  - The length in front tells it how many bytes to read for this message
     *    and the bytes after that belong to the next frame
     *  - Same job as a delimiter but it does not depend on the payload content (works for binary too)
     *  - The prefix is always LENGTH_PREFIX_SIZE (4) bytes and the length written in it is
     *    the payload size so the frame is 4 + payload size
     */
    static std::string Encode(const std::string& payload);

    void Append(const char* data, std::size_t size);
    /**
     * NOTES:
     *  - Decodes frames built by Encode(): [4 bytes payload length prefix][payload]
     *  - m_buffer holds whatever read() returned so far which can end in the middle
     *    of the frame
     *  - Each loop reads one frame's prefix, checks its whole payload has arrived and
     *    copies it out and moves to the next frame
     *  - Stops at first incomplete frame and its bytes stay in m_buffer until the next
     *    Append() brings the rest
     *  - Throws above MAX_MESSAGE_SIZE which means the stream is out of sync and caller
     *    closes socket
     */
    std::vector<std::string> ExtractFrames();

private:
    std::string m_buffer = "";
};
