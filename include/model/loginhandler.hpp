#ifndef LOGINHANDLER_HPP
#define LOGINHANDLER_HPP

#include <string>
#include <cryptopp/osrng.h>
#include <cryptopp/secblock.h>
#include <sqlite3.h>
#include <memory>
#include "model/dbhandler.hpp"
#include "model/user.hpp"
#include "model/passwordentry.hpp"

#define IV_LENGTH 16
#define HASH_SIZE 32
#define SALT_SIZE 32
#define MASTER_KEY_ITERATIONS 200000
#define AES_ITERATIONS 200000
#define AES_KEYLENGTH 32
#define USERLOGIN_PATH "data/UserLogin.db"
/**
 * The Loginhandler class contains the logic for generating hashes and authenticating users
 */
class Loginhandler
{
public:
    /**
     * Initializes the Loginhandler
     * @param dbhandler is a pointer to the DBHandler class which controls the Database
     */
    Loginhandler();
    bool CheckLoginCredentials(std::shared_ptr<DBHandler> dbHandler, std::string login, std::string password);
    /**
     * This method generates a new User with given inputs and generates a random salt to create a Hash
     */
    User CreateNewUser(const std::string &login, const std::string &password);

    PasswordEntry GenerateLoginEntry(std::string &userLogin, std::string &userPassword, std::string &userSalt, std::string &sourceLogin, std::string &sourcePassword, std::string &source);

    void DecryptAllPasswords(std::vector<PasswordEntry> &passwordEntries, std::string &masterLogin, std::string &masterPassword, std::string &salt_base64);

private:
    static CryptoPP::SecByteBlock GenerateSalt(size_t size);
};
#endif