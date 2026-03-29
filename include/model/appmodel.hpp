#ifndef APPMODEL_HPP
#define APPMODEL_HPP

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "controller/appmanager.hpp"
#include "model/dbhandler.hpp"
#include "model/loginhandler.hpp"
#include "model/passwordentry.hpp"
#include "model/user.hpp"

#define NOLOGINFOUND 1
#define INCORRECTLOGIN 2
#define APP_OK 0

class AppManager;

class Appmodel
{
public:
    Appmodel(const std::shared_ptr<DBHandler> dbhandler);
    ~Appmodel();
    void SetManager(AppManager *manager);
    int AuthenticateUser(std::string &login, std::string &password);
    std::vector<PasswordEntry> ReadAllPasswordEntriesFromUser(std::string &login);
    void InsertNewEntry(std::string &login, std::string &password, std::string &source);
    bool IsMasterLoginInDB(std::string login);
    void AddNewMasterLogin(User &user);
    void DeleteEntry(int entryID);
    void EditEntry(int id, std::string& login, std::string& password, std::string& source);
    User GetCurrentUser();
    /**
     * Resets internal variables of the class
     */
    void Reset();

    void DeleteAllData();

private:
    AppManager                  *pManager;
    User                        mCurrentUser;
    std::shared_ptr<DBHandler>  pDBHandler;
};

#endif