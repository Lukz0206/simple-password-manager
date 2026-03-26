#include <iostream>
#include <format>
#include "model/dbhandler.hpp"
#include "errors/dbexception.hpp"
#include "model/loginhandler.hpp"

DBHandler::DBHandler(std::string dbPath)
{
    this->mUserDBPath = dbPath;
}

DBHandler::~DBHandler()
{
}

void DBHandler::AddNewMasterLogin(User &user)
{
    std::string login = user.mLogin;
    std::string passwordhash = user.mPassword;
    std::string salt = user.mSalt;
    try
    {
        OpenDatabase();
        std::string sqlstatement = std::format("Insert Into Logindata values('{}','{}','{}');", login, passwordhash, salt);
        std::cout << sqlstatement << std::endl;
        char *errormsg = nullptr;
        std::string error;
        int rc = sqlite3_exec(pLoginDB, sqlstatement.c_str(), NULL, NULL, &errormsg);
        if (rc != SQLITE_OK)
        {
            error = errormsg;
            // std::cout << errormsg << std::endl;
            sqlite3_free(errormsg);
            sqlite3_close(pLoginDB);
            throw DBException(error.c_str());
        }
        sqlite3_close(pLoginDB);
    }
    catch (DBException &e)
    {
        throw e;
    }
}

void DBHandler::OpenDatabase()
{
    sqlite3 *loginDB;
    int rc = sqlite3_open(mUserDBPath.c_str(), &loginDB);

    if (rc)
    {
        std::string error("Can't open database: %s\n", sqlite3_errmsg(loginDB));
        throw DBException(error.c_str());
    }
    pLoginDB = loginDB;
}

// Callback function for SQL Query
int DBHandler::ReadCountFunc(void *data, int numCols, char **colVals, char **colNames)
{
    int *countPtr = (int *)data;
    if (numCols > 0)
    {
        *countPtr = atoi(colVals[0]);
    }
    return 0;
}

bool DBHandler::IsMasterLoginInDB(std::string login)
{
    try
    {
        OpenDatabase();
    }
    catch (const DBException &e)
    {
        throw e;
    }

    std::string sqlstatement = std::format("Select Count(Login) from Logindata where Login='{}';", login);
    std::cout << sqlstatement << std::endl;
    sqlite3_stmt *statement;
    // Create SQL bytecode statement
    char *errmsg = nullptr;
    int totalUsers = 0;
    if (sqlite3_exec(pLoginDB, sqlstatement.c_str(), ReadCountFunc, &totalUsers, &errmsg) != SQLITE_OK)
    {
        std::string error = errmsg;
        sqlite3_free(errmsg);
        sqlite3_close(pLoginDB);
        throw DBException(error.c_str());
    }
    std::cout << "TotalUsers : " << totalUsers << std::endl;
    bool isInDB;
    if (totalUsers == 0)
    {
        isInDB = false;
    }
    else
    {
        isInDB = true;
    }
    sqlite3_close(pLoginDB);
    return isInDB;
}

// This Function should never have more than one row
int DBHandler::ReadUserLoginEntry(void *data, int numCols, char **colVals, char **colNames)
{
    User *user = (User *)data;
    std::cout << "NumCols: " << numCols << std::endl;

    user->mLogin = colVals[0];
    user->mPassword = colVals[1];
    user->mSalt = colVals[2];
    return 0;
}

User DBHandler::GetMasterLoginData(std::string login)
{
    try
    {
        OpenDatabase();
    }
    catch (const DBException &e)
    {
        throw e;
    }
    std::string sql = std::format("Select Login, Hash, Salt from Logindata where Login='{}';", login);
    User userLogin;
    char *errmsg;
    if (sqlite3_exec(pLoginDB, sql.c_str(), &ReadUserLoginEntry, &userLogin, &errmsg) != SQLITE_OK)
    {
        std::string error = errmsg;
        sqlite3_free(errmsg);
        sqlite3_close(pLoginDB);
        throw DBException(error.c_str());
    }
    sqlite3_close(pLoginDB);
    return userLogin;
}

// Insert the given strings exactly into the Table USERPASSWORD as it is
void DBHandler::InsertNewEntry(const std::string &user, const std::string &login, const std::string &password, const std::string &source,  const std::string &iv_base64)
{   std::cout << "Passwort to add: " << password << std::endl;
    std::string sql = std::format("INSERT INTO USERPASSWORD VALUES(NULL, '{}', '{}', '{}', '{}', '{}');", user, login, password, source, iv_base64);
    std::cout << sql << std::endl;
    try
    {
        OpenDatabase();
    }
    catch (const DBException &e)
    {
        throw e;
    }
    char *errormsg;
    if (sqlite3_exec(pLoginDB, sql.c_str(), NULL, NULL, &errormsg) != SQLITE_OK)
    {
        std::string error = errormsg;
        sqlite3_free(errormsg);
        throw DBException(error.c_str());
    }
    std::cout << "Data inserted successfully \n";
    sqlite3_close(pLoginDB);
}

int ReadUserPasswordEntries(void *data, int numCols, char **colVals, char **colNames)
{
    PasswordEntry entry;
    entry.mID = atoi(colVals[0]);
    entry.mUser = colVals[1];
    entry.mLogin = colVals[2];
    entry.mPassword = colVals[3];
    entry.mSource = colVals[4];
    entry.mIV = colVals[5];
    
    std::vector<PasswordEntry>* entryList = static_cast<std::vector<PasswordEntry>*>(data);
    entryList->push_back(entry);
    return 0;
}
 
/**
* This method fetches every password-entry for a specific user and creates a vector containing the unencrypted data 
*/
std::vector<PasswordEntry> DBHandler::ReadAllPasswordEntriesFromUser(std::string &user)
{
    std::string sql = std::format("SELECT PASSID, USER, LOGINNAME, PASSWORD, SOURCE, IV from Userpassword where USER = '{}';", user);
    std::vector<PasswordEntry> userEntries;
    try
    {
        OpenDatabase();
        char *errmsg;
        if (sqlite3_exec(pLoginDB, sql.c_str(), &ReadUserPasswordEntries, &userEntries, &errmsg) != SQLITE_OK)
        {
            std::string err = errmsg;
            sqlite3_free(errmsg);
            sqlite3_close(pLoginDB);
            throw DBException(err.c_str());
        }
        sqlite3_close(pLoginDB);
        return userEntries;
    }
    catch(const DBException& e)
    {
        throw e;
    }
   
}

void DBHandler::DeletePasswordEntry(int entryID)
{
    try
    {
        OpenDatabase();
        std::string sql = std::format("Delete from Userpassword where PASSID={}", entryID);
        char* errmsg;
        if(sqlite3_exec(pLoginDB, sql.c_str(), NULL, NULL, &errmsg) != SQLITE_OK)
        {
            std::string error = errmsg;
            sqlite3_free(errmsg);
            sqlite3_close(pLoginDB);
        }
        sqlite3_close(pLoginDB);
    }
    catch(const DBException& e)
    {
        throw e;
    }
}