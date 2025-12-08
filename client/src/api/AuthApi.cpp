#include "api/AuthApi.h"

#include <iostream>

// **********
// * PUBLIC *
// **********
void AuthApi::Login(const LoginParams& LoginParams) const
{
    std::cout << "Logging in with email: " << LoginParams.Email << " and password " << LoginParams.Password << std::endl;
}

void AuthApi::Register(const RegisterParams& RegisterParams) const
{
    std::cout << "Registering with firstname: " << RegisterParams.FirstName << " lastname: " << RegisterParams.LastName << " email: " << RegisterParams.Email << " and password " << RegisterParams.Password << std::endl;
}
