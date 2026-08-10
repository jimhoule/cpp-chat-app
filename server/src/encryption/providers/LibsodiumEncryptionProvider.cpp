#include "encryption/providers/LibsodiumEncryptionProvider.h"

#include "log/Logger.h"

#include <array>
#include <sodium.h>
#include <stdexcept>

// **********
// * PUBLIC *
// **********
/**
 * NOTES:
 *  - sodium_init must run before any other libsodium call, it is done here because the module builds this provider once at startup while the server is still
 *    single threaded, the function itself is not safe to race
 *  - It returns 0 on the first call and 1 when already initialized, only -1 is a real failure
 */
LibsodiumEncryptionProvider::LibsodiumEncryptionProvider(Logger& logger)
    : m_logger(logger)
{
    int sodiumResult = sodium_init();
    if (sodiumResult < 0)
    {
        m_logger.Error("Libsodium initialization failed");

        throw std::runtime_error("Libsodium initialization failed");
    }
}

/**
 * NOTES:
 *  - crypto_pwhash_str is Argon2id, it draws its own random salt and embeds both the salt and the parameters it used inside the returned string, so Verify
 *    needs nothing besides the hash itself and there is no salt column to store
 *  - The same input therefore never produces the same output, comparing two hashes with == is always wrong, that is what Verify is for
 *  - INTERACTIVE is 2 passes over 64 MiB, tuned to stay under a login request budget, MODERATE and SENSITIVE exist if we ever decide to spend more per attempt
 *  - The only failure is libsodium not being able to allocate its 64 MiB, that is a server fault the caller cannot correct, so it is not an ExpectedException
 */
std::string LibsodiumEncryptionProvider::Encrypt(const std::string& string) const
{
    std::array<char, crypto_pwhash_STRBYTES> hash = {};

    int hashResult = crypto_pwhash_str(hash.data(), string.c_str(), string.size(), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE);
    if (hashResult != 0)
    {
        m_logger.Error("Hashing failed, libsodium could not allocate enough memory");

        throw std::runtime_error("Hashing failed");
    }

    // NOTE: The buffer holds a NUL terminated ascii string, so data() stops at the terminator instead of dragging the unused tail of the array into the string
    const std::string hashedString = std::string(hash.data());

    return hashedString;
}

/**
 * NOTES:
 *  - Reads the parameters back out of the stored hash, so rows written under older limits keep verifying after we raise them
 *  - Comparison is constant time internally, a wrong password costs the same as a nearly correct one
 *  - A malformed or truncated hash also returns -1, reported as a failed verification because the caller has no way to act on the difference
 */
bool LibsodiumEncryptionProvider::Verify(const std::string& string, const std::string& hashedString) const
{
    int verificationResult = crypto_pwhash_str_verify(hashedString.c_str(), string.c_str(), string.size());
    bool isVerified = verificationResult == 0;

    return isVerified;
}
