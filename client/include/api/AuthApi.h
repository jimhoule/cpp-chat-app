#pragma once

#include "serializer/LoginSocketEventSerializer.h"
#include "serializer/RegisterSocketEventSerializer.h"
#include "socket/SocketClient.h"

#include <string>

struct LoginParams
{
    std::string Email;
    std::string Password;

};

struct RegisterParams
{
    std::string FirstName;
    std::string LastName;
    std::string Email;
    std::string Password;

};

class AuthApi
{
public:
    AuthApi(const SocketClient& SocketClient);

    void Login(const LoginParams& LoginParams);
    void Register(const RegisterParams& RegisterParams);

private:
    LoginSocketEventSerializer m_LoginSocketEventSerializer = {};
    RegisterSocketEventSerializer m_RegisterSocketEventSerializer = {};
    SocketClient m_SocketClient = {};
};
