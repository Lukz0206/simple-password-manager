#ifndef PASSWORDENTRY_HPP
#define PASSWORDENTRY_HPP
#include <string>

/**
 * A PasswordEntry object represents the data of a database that is used by the GUI
 * It contains a Loginname, Password and source of the login.
 * The GUI uses the given infos to generate the labels accordingly.
 */
class PasswordEntry
{
public:
    PasswordEntry();
    ~PasswordEntry();
    size_t mID;
    std::string mUser;
    std::string mLogin;
    std::string mPassword;
    std::string mSource;
    std::string mIV;
};

#endif