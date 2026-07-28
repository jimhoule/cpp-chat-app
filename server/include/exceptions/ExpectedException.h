#pragma once

#include "socket/SocketErrorCode.h"

#include <stdexcept>
#include <string>

class ExpectedException : public std::runtime_error
{
public:
    ExpectedException(SocketErrorCode socketErrorCode, const std::string& message);

    SocketErrorCode GetSocketErrorCode() const;

private:
    SocketErrorCode m_socketErrorCode;
};
