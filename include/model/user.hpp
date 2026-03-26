#ifndef USER_HPP
#define USER_HPP
#include <string>

class User
{
public:
    User(std::string login = "", std::string password = "", std::string salt = "");
    std::string mLogin;
    std::string mPassword;
    std::string mSalt;
};

#endif