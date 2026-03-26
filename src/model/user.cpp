#include <string>
#include "model/user.hpp"

User::User(std::string login, std::string password, std::string salt)
{
    this->mLogin = login;
    this->mPassword = password;
    this->mSalt = salt;
}