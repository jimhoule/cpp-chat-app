#include "users/UsersService.h"

#include "encryption/EncryptionService.h"
#include "uuid/UuidService.h"

// **********
// * PUBLIC *
// **********
UsersService::UsersService(std::unique_ptr<IUsersRepository> usersRepository, EncryptionService& encryptionService, UuidService& uuidService, Logger& logger)
    : m_usersRepository(std::move(usersRepository))
    , m_encryptionService(encryptionService)
    , m_uuidService(uuidService)
    , m_logger(logger)
{}

UsersService::UserResult UsersService::Create(const CreateUserDto& createUserDto)
{
    UserResult userResult = {};

    // Checks if a user with this email already exists
    std::optional<User> existingUser = m_usersRepository->FindByEmail(createUserDto.email);
    if (existingUser.has_value())
    {
        userResult.code = UsersResultCode::EMAIL_ALREADY_USED;
        return userResult;
    }

    EncryptionService::EncryptStringDto encryptStringDto = {};
    encryptStringDto.string = createUserDto.password;

    User user = {};
    user.id = m_uuidService.Generate();
    user.email = createUserDto.email;
    user.firstName = createUserDto.firstName;
    user.lastName = createUserDto.lastName;
    user.password = m_encryptionService.Encrypt(encryptStringDto);

    userResult.data = m_usersRepository->Create(user);

    return userResult;
}

UsersService::UserResult UsersService::FindByEmail(const FindUserByEmailDto& findUserByEmailDto)
{
    UserResult userResult = {};
    userResult.data = m_usersRepository->FindByEmail(findUserByEmailDto.email);

    return userResult;
}

UsersService::UserResult UsersService::FindById(const FindUserByIdDto& findUserByIdDto)
{
    UserResult userResult = {};
    userResult.data = m_usersRepository->FindById(findUserByIdDto.id);

    return userResult;
}

UsersService::UserResult UsersService::VerifyPassword(const VerifyUserPasswordDto& verifyUserPasswordDto)
{
    UserResult userResult = {};

    EncryptionService::VerifyStringDto verifyStringDto = {};
    verifyStringDto.string = verifyUserPasswordDto.password;
    verifyStringDto.hashedString = verifyUserPasswordDto.hashedPassword;
    bool isVerified = m_encryptionService.Verify(verifyStringDto);
    if (!isVerified)
    {
        userResult.code = UsersResultCode::INVALID_PASSWORD;
    }

    return userResult;
}

std::string UsersService::ConvertUsersResultCodeToString(UsersResultCode usersResultCode)
{
    switch (usersResultCode)
    {
        case UsersResultCode::OK:
            return "OK";

        case UsersResultCode::EMAIL_ALREADY_USED:
            return "EMAIL ALREADY USED";

        case UsersResultCode::INVALID_PASSWORD:
            return "INVALID PASSWORD";

        default:
            return "Unknown users result code";
    }
}
