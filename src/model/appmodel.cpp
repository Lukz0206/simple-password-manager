#include "model/appmodel.hpp"

Appmodel::Appmodel(const std::shared_ptr<DBHandler> dbhandler)
{
    pDBHandler = dbhandler;
}

Appmodel::~Appmodel(){}

void Appmodel::SetManager(AppManager *manager)
{
    pManager = manager;
}

int Appmodel::AuthenticateUser(std::string &login, std::string &password)
{
    try
    {
        if (!pDBHandler->IsMasterLoginInDB(login))
        {
            return NOLOGINFOUND;
        }
        std::cout << "Login found" << std::endl;
        Loginhandler loginhandler;
        if (!loginhandler.CheckLoginCredentials(pDBHandler, login, password))
        {
            return INCORRECTLOGIN;
        }
        std::cout << "Login correct. Show next Page" << std::endl;
        // Get Data from User and change password to normal representation
        User currentUser = pDBHandler->GetMasterLoginData(login);
        currentUser.mPassword = password;
        // Store the logindata
        mCurrentUser.mLogin = currentUser.mLogin;
        mCurrentUser.mPassword = password;
        mCurrentUser.mSalt = currentUser.mSalt;
        return APP_OK;
    }
    catch (DBException &e)
    {
        throw e;
    }
}

// Reads all Passwordenntries for a user and encrypts them. Can throw an error
std::vector<PasswordEntry> Appmodel::ReadAllPasswordEntriesFromUser(std::string &login)
{
    std::vector<PasswordEntry> entries = pDBHandler->ReadAllPasswordEntriesFromUser(login);
    Loginhandler loginhandler;
    try
    {
        loginhandler.DecryptAllPasswords(
            entries, mCurrentUser.mLogin, mCurrentUser.mPassword, mCurrentUser.mSalt);
    }
    catch (const std::exception &e)
    {
        throw e;
    }

    return entries;
}


void Appmodel::InsertNewEntry(std::string &login, std::string &password, std::string &source)
{
    Loginhandler loginhandler;
    PasswordEntry entryToAdd = loginhandler.GenerateLoginEntry(
        mCurrentUser.mLogin, mCurrentUser.mPassword, mCurrentUser.mSalt, login, password, source);
    try
    {
        pDBHandler->InsertNewEntry(entryToAdd.mUser, entryToAdd.mLogin, 
            entryToAdd.mPassword, entryToAdd.mSource, entryToAdd.mIV);
    }
    catch(const DBException& e)
    {
        throw e;
    }
    
}

bool Appmodel::IsMasterLoginInDB(std::string login)
{
    try
    {
        return pDBHandler->IsMasterLoginInDB(login);
    }
    catch(const DBException& e)
    {
        throw e;
    }
    
}

void Appmodel::AddNewMasterLogin(User &user)
{
    try
    {
        pDBHandler->AddNewMasterLogin(user);
    }
    catch(const DBException& e)
    {
        throw e;
    }
}

User Appmodel::GetCurrentUser(){
    return mCurrentUser;
}