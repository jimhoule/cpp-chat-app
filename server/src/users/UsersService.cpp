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

    UserView userView = ConvertUserToUserView(m_usersRepository->Create(user));
    userResult.data = userView;

    return userResult;
}

UsersService::UserResult UsersService::FindById(const FindUserByIdDto& findUserByIdDto)
{
    UserResult userResult = {};

    std::optional<User> user = m_usersRepository->FindById(findUserByIdDto.id);
    if (user.has_value())
    {
        UserView userView = ConvertUserToUserView(user.value());
        userResult.data = userView;
    }

    return userResult;
}

UsersService::UsersResult UsersService::FindByIds(const FindUsersByIdsDto& findUsersByIdsDto)
{
    const std::vector<User> users = m_usersRepository->FindByIds(findUsersByIdsDto.ids);

    std::vector<UserView> userViews = {};
    for (const User& user : users)
    {
        UserView userView = ConvertUserToUserView(user);
        userViews.push_back(userView);
    }

    UsersResult usersResult = {};
    usersResult.data = userViews;

    return usersResult;
}

UsersService::UsersResult UsersService::Search(const SearchUsersDto& searchUsersDto)
{
    UsersResult usersResult = {};

    // Returns empty result if empty search string was sent
    if (searchUsersDto.search.empty())
    {
        return usersResult;
    }

    // Searches users
    const std::vector<User> users = m_usersRepository->Search(searchUsersDto.search);

    // Converts users to user views
    std::vector<UserView> userViews = {};
    // NOTE: The final size is known, so the vector allocates once instead of reallocating as it grows
    userViews.reserve(users.size());
    for (const User& user : users)
    {
        const UserView userView = ConvertUserToUserView(user);
        userViews.push_back(userView);
    }

    usersResult.data = userViews;

    return usersResult;
}

UsersService::UserResult UsersService::VerifyCredentials(const VerifyUserCredentialsDto& verifyUserCredentialsDto)
{
    UserResult userResult = {};

    // Checks if a user with this email exists
    std::optional<User> user = m_usersRepository->FindByEmail(verifyUserCredentialsDto.email);
    if (!user.has_value())
    {
        userResult.code = UsersResultCode::UNKNOWN_USER;
        return userResult;
    }

    EncryptionService::VerifyStringDto verifyStringDto = {};
    verifyStringDto.string = verifyUserCredentialsDto.password;
    verifyStringDto.hashedString = user.value().password;
    bool isVerified = m_encryptionService.Verify(verifyStringDto);
    if (!isVerified)
    {
        userResult.code = UsersResultCode::INVALID_PASSWORD;
        return userResult;
    }

    const UserView userView = ConvertUserToUserView(user.value());
    userResult.data = userView;

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

        case UsersResultCode::UNKNOWN_USER:
            return "UNKNOWN USER";

        default:
            return "Unknown users result code";
    }
}

// ***********
// * PRIVATE *
// ***********
UserView UsersService::ConvertUserToUserView(const User& user) const
{
    UserView userView = {};
    userView.id = user.id;
    userView.email = user.email;
    userView.firstName = user.firstName;
    userView.lastName = user.lastName;

    return userView;
}
