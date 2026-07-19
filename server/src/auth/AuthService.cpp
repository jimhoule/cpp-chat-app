#include "auth/AuthService.h"

// **********
// * PUBLIC *
// **********
AuthService::AuthService(UsersService& usersService) : m_usersService(usersService)
{}

AuthServiceResult AuthService::Login(const LoginDto& loginDto)
{
    AuthServiceResult authServiceResult;

    // Checks if user with this email exists
    FindUserByEmailDto findUserByEmailDto = {};
    findUserByEmailDto.email = loginDto.email;
    std::optional<User> user = m_usersService.FindByEmail(findUserByEmailDto);
    if (!user.has_value()) return authServiceResult;

    // Validates password
    // TODO: Create Encryption service for hashing logic
    const bool isPasswordValid = user.value().password == "hashed." + loginDto.password;
    if (!isPasswordValid) return authServiceResult;

    // Creates session
    // TODO: Create Sessions service
    std::string sessionId = "sessionId." + user.value().id;

    authServiceResult.sessionId =  sessionId;
    authServiceResult.user = user;

    return authServiceResult;
}

AuthServiceResult AuthService::Register(const RegisterDto& registerDto)
{
    AuthServiceResult authServiceResult;

    // Checks if user with this email exists
    FindUserByEmailDto findUserByEmailDto = {};
    findUserByEmailDto.email = registerDto.email;
    std::optional<User> existingUser = m_usersService.FindByEmail(findUserByEmailDto);
    if (existingUser.has_value()) return authServiceResult;

    // Creates user
    CreateUserDto createUserDto = {};
    createUserDto.email = registerDto.email;
    createUserDto.firstName = registerDto.firstName;
    createUserDto.lastName = registerDto.lastName;
    createUserDto.password = registerDto.password;
    User user = m_usersService.Create(createUserDto);
    
    // Creates session
    // TODO: Create Sessions service
    std::string sessionId = "sessionId." + user.id;

    authServiceResult.sessionId =  sessionId;
    authServiceResult.user = user;

    return authServiceResult;
}
