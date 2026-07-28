#include "exceptions/ExpectedException.h"

// **********
// * PUBLIC *
// **********
ExpectedException::ExpectedException(SocketErrorCode socketErrorCode, const std::string& message)
    : m_socketErrorCode(socketErrorCode)
    , std::runtime_error(message)
{}

SocketErrorCode ExpectedException::GetSocketErrorCode() const
{
    return m_socketErrorCode;
}
