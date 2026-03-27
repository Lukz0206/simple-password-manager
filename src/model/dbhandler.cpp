#include <iostream>
#include <format>
#include "model/dbhandler.hpp"
#include "errors/dbexception.hpp"
#include "model/loginhandler.hpp"

const char* CloseAndFinalizeDB(sqlite3 *db, sqlite3_stmt *stmt)
{
    std::string error = sqlite3_errmsg(db);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return error.c_str();
}

DBHandler::DBHandler(std::string dbPath)
{
    this->mUserDBPath = dbPath;
}

DBHandler::~DBHandler()
{
}

void DBHandler::AddNewMasterLogin(User &user)
{
    std::vector<std::string> userData = {user.mLogin, user.mPassword, user.mSalt};
    try
    {
        OpenDatabase();
    }
    catch (DBException &e)
    {
        throw e;
    }
    std::string sql = "Insert Into Logindata values(?1, ?2, ?3);";
    
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(pLoginDB, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK)
    {
        std::string error = CloseAndFinalizeDB(pLoginDB, stmt);
        throw DBException(error);
    }

    for (int i = 0; i < userData.size(); i++)
    {
        sqlite3_bind_text(stmt, i + 1, userData.at(i).c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_bind_text(stmt, i + 1, userData.at(i).c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
        {
            std::string error = CloseAndFinalizeDB(pLoginDB, stmt);
            throw DBException(error);
        }
    }

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        throw DBException(CloseAndFinalizeDB(pLoginDB, stmt));
    }

    CloseAndFinalizeDB(pLoginDB, stmt);
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

bool DBHandler::IsMasterLoginInDB(std::string login)
{
    try
    {
        OpenDatabase();
    }
    catch (const DBException &e)
    {
        throw;
    }

    std::string sql = std::format("Select Count(Login) from Logindata where Login = ?1;");

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(pLoginDB, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK)
    {
        std::string error = CloseAndFinalizeDB(pLoginDB, stmt);
        throw DBException(error);
    }

    if (sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
    {
        std::string error = CloseAndFinalizeDB(pLoginDB, stmt);
        throw DBException(error);
    }

    int totalUsers = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        totalUsers = sqlite3_column_int(stmt,0);
    }
    bool isInDB;
    if (totalUsers == 0)
    {
        isInDB = false;
    }
    else
    {
        isInDB = true;
    }
    CloseAndFinalizeDB(pLoginDB, stmt);
    return isInDB;
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
    std::string sql = "Select Login, Hash, Salt from Logindata where Login= ?1;";
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(pLoginDB, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK)
    {
        std::string error = CloseAndFinalizeDB(pLoginDB, stmt);
        throw DBException(error);
    }

    if (sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
    {
        std::string error = CloseAndFinalizeDB(pLoginDB, stmt);
        throw DBException(error);
    }
    User userLogin;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        userLogin.mLogin = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        userLogin.mPassword = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        userLogin.mSalt = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    }
    CloseAndFinalizeDB(pLoginDB, stmt);
    return userLogin;
}

// Insert the given strings exactly into the Table USERPASSWORD as it is
void DBHandler::InsertNewEntry(const std::string &user, const std::string &login, const std::string &password, const std::string &source, const std::string &iv_base64)
{
    std::vector<std::string> entryData = {user, login, password, source, iv_base64};
    std::string sql = std::format("INSERT INTO USERPASSWORD VALUES(NULL, ?1, ?2, ?3, ?4, ?5);");
    std::cout << sql << std::endl;
    try
    {
        OpenDatabase();
        sqlite3_stmt *stmt = nullptr;
        if (sqlite3_prepare_v2(pLoginDB, sql.c_str(), -1, &stmt, NULL) != SQLITE_OK)
        {
            std::string err = CloseAndFinalizeDB(pLoginDB, stmt);
            throw DBException(err);
        }
        // Iterate through entries and add data in correct order
        int bindstatus;
        for (int i = 0; i < entryData.size(); i++)
        {
            bindstatus = sqlite3_bind_text(stmt, i + 1, entryData.at(i).c_str(), -1, SQLITE_TRANSIENT);
            if (bindstatus != SQLITE_OK)
            {
                std::string err = CloseAndFinalizeDB(pLoginDB, stmt);
                throw DBException(err);
            }
        }

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            std::string err = CloseAndFinalizeDB(pLoginDB, stmt);
            throw DBException(err);
        }
        std::cout << "Data inserted successfully \n";
        CloseAndFinalizeDB(pLoginDB, stmt);
    }
    catch (const DBException &e)
    {
        throw;
    }
}

/**
 * This method fetches every password-entry for a specific user and creates a vector containing the unencrypted data
 */
std::vector<PasswordEntry> DBHandler::ReadAllPasswordEntriesFromUser(std::string &user)
{
    const std::string sql = "SELECT PASSID, USER, LOGINNAME, PASSWORD, SOURCE, IV from Userpassword where USER = ?1;";
    std::vector<PasswordEntry> userEntries;

    OpenDatabase();
    sqlite3_stmt *stmt = nullptr;

    if (sqlite3_prepare_v2(pLoginDB, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::string err = CloseAndFinalizeDB(pLoginDB, stmt);
        throw DBException(err);
    }

    if (sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
    {
        std::string err = CloseAndFinalizeDB(pLoginDB, stmt);
        throw DBException(err);
    }

    int status;
    while ((status = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        PasswordEntry entry;
        entry.mID = sqlite3_column_int(stmt, 0);
        entry.mUser = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        entry.mLogin = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        entry.mPassword = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        entry.mSource = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
        entry.mIV = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
        userEntries.push_back(entry);
    }

    if (status != SQLITE_DONE)
    {
        std::string err = CloseAndFinalizeDB(pLoginDB, stmt);
        throw DBException(err);
    }

    CloseAndFinalizeDB(pLoginDB,stmt);
    return userEntries;
}

void DBHandler::DeletePasswordEntry(int entryID)
{
    try
    {
        OpenDatabase();
        std::string sql = std::format("Delete from Userpassword where PASSID={}", entryID);
        char *errmsg;
        // Doesn't require injection safety because the user can't directly change entryID to cause undefined behaviour
        if (sqlite3_exec(pLoginDB, sql.c_str(), NULL, NULL, &errmsg) != SQLITE_OK)
        {
            std::string error = errmsg;
            sqlite3_free(errmsg);
            sqlite3_close(pLoginDB);
        }
        sqlite3_close(pLoginDB);
    }
    catch (const DBException &e)
    {
        throw e;
    }
}