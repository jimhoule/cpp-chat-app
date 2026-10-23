#include "socket/Framer.h"

#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <stdexcept>

constexpr std::size_t LENGTH_PREFIX_SIZE = sizeof(std::uint32_t);
constexpr std::uint32_t MAX_MESSAGE_SIZE = 10 * 1024 * 1024;

// **********
// * PUBLIC *
// **********
std::string Framer::Encode(const std::string& payload)
{
    if (payload.size() > MAX_MESSAGE_SIZE)
    {
        throw std::length_error("Payload exceeds max message size");
    }

    std::uint32_t networkLength = htonl(static_cast<std::uint32_t>(payload.size()));

    // Creates the 4 bytes prefix (empty)
    std::string frame(LENGTH_PREFIX_SIZE, '\0');
    /**
     * NOTES:
     *  - Writes payload length into those 4 bytes and the size stays 4 bytes
     *  - memcpy because += cannot append uint32 raw bytes
     */
    std::memcpy(frame.data(), &networkLength, LENGTH_PREFIX_SIZE);
    // Appends the payload after the 4 bytes prefix
    frame += payload;

    return frame;
}

void Framer::Append(const char* data, std::size_t size)
{
    // NOTE: Explicit size because data is not null terminated and can contain \0
    m_buffer.append(data, size);
}

std::vector<std::string> Framer::ExtractFrames()
{
    std::vector<std::string> payloads = {};
    // NOTE: Position in m_buffer where the next unextracted frame starts (how far it has been read)
    std::size_t offset = 0;

    // Loops while there are at least 4 unextracted bytes so the prefix can be read
    while (m_buffer.size() - offset >= LENGTH_PREFIX_SIZE)
    {
        // Length in network byte order
        std::uint32_t networkLength = 0;
        // NOTE: memcpy instead of casting because the string data is not aligned for uint32
        std::memcpy(&networkLength, m_buffer.data() + offset, LENGTH_PREFIX_SIZE);
        // Length in machine byte order
        const std::uint32_t payloadLength = ntohl(networkLength);

        // NOTE: A length this big means the stream is out of sync and nothing can recover it
        if (payloadLength > MAX_MESSAGE_SIZE)
        {
            throw std::length_error("Frame length exceeds max message size");
        }

        /**
         * NOTES:
         *  - Payload not fully arrived yet
         *  - Checks how many bytes there currently are compared to how many are needed
         *  - m_buffer.size() - offset is the number of bytes not extracted yet counted 
         *    from the start of the current frame
         *  - Subtracting LENGTH_PREFIX_SIZE removes the prefix which leaves the payload
         *    bytes that have arrived so far
         *  - payloadLength is how many bytes the prefix says this frame has
         */
        if (m_buffer.size() - offset - LENGTH_PREFIX_SIZE < payloadLength)
        {
            break;
        }

        // Copies the current frame's payload out of the buffer without its prefix
        const std::string payload = m_buffer.substr(offset + LENGTH_PREFIX_SIZE, payloadLength);
        payloads.push_back(payload);
        // Moves past this frame
        offset += LENGTH_PREFIX_SIZE + payloadLength;
    }

    // NOTE: One erase for all extracted frames, not one per frame
    m_buffer.erase(0, offset);

    return payloads;
}
