#ifndef DBHANDLER_HPP
#define DBHANDLER_HPP

#include <sqlite3.h>
#include <string>
#include "errors/dbexception.hpp"
#include "model/user.hpp"
#include <vector>
#include "model/passwordentry.hpp"
// This object handles the sqlite database
class DBHandler
{
public:
    DBHandler(std::string dbPath);
    ~DBHandler();
    /**
     * Adds a new user to the UserLogin.db file. If something goes wrong an exception will be thrown.
     */
    void AddNewMasterLogin(User &user);
    /**
     * Checks if the given parameter is already in the Database
     */
    bool IsMasterLoginInDB(std::string);
    User GetMasterLoginData(std::string login);
    
    void InsertNewEntry(const std::string &user, const std::string &login, const std::string &password, const std::string &source, const std::string &iv_base64);
    std::vector<PasswordEntry> ReadAllPasswordEntriesFromUser(std::string &user);
    void DeletePasswordEntry(int entryID);
    void EditEntry(PasswordEntry &entry);


private:
    std::string mUserDBPath;
    sqlite3 *pLoginDB;

    // Creates a connection for the database which has to be closed manually, can throw an Exception
    void OpenDatabase();
    /**
     * Is used as a callback to read the count of rows from a SQL query
     */
    static int ReadCountFunc(void *data, int numCols, char **colVals, char **colNames);
    static int ReadUserLoginEntry(void *data, int numCols, char **colVals, char **colNames);
    const char* CloseAndFinalizeDB(sqlite3 *db, sqlite3_stmt *stmt);

};

#endif