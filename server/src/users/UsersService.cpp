#include "users/UsersService.h"

#include "uuid/UuidService.h"

std::string ConvertUsersResultCodeToString(UsersResultCode usersResultCode)
{
    switch (usersResultCode)
    {
        case UsersResultCode::OK:
            return "OK";

        case UsersResultCode::EMAIL_ALREADY_USED:
            return "EMAIL ALREADY USED";

        default:
            return "Unknown users result code";
    }
}

// **********
// * PUBLIC *
// **********
UsersService::UsersService(std::unique_ptr<IUsersRepository> usersRepository, UuidService& uuidService, Logger& logger)
    : m_usersRepository(std::move(usersRepository))
    , m_uuidService(uuidService)
    , m_logger(logger)
{}

UserResult UsersService::Create(const CreateUserDto& createUserDto)
{
    UserResult userResult = {};

    // Checks if a user with this email already exists
    std::optional<User> existingUser = m_usersRepository->FindByEmail(createUserDto.email);
    if (existingUser.has_value())
    {
        userResult.code = UsersResultCode::EMAIL_ALREADY_USED;
        return userResult;
    }

    User user = {};
    user.id = m_uuidService.Generate();
    user.email = createUserDto.email;
    user.firstName = createUserDto.firstName;
    user.lastName = createUserDto.lastName;
    user.password = "hashed." + createUserDto.password;

    userResult.data = m_usersRepository->Create(user);

    return userResult;
}

UserResult UsersService::FindByEmail(const FindUserByEmailDto& findUserByEmailDto)
{
    UserResult userResult = {};
    userResult.data = m_usersRepository->FindByEmail(findUserByEmailDto.email);

    return userResult;
}