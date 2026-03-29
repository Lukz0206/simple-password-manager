#ifndef APPMANAGER_HPP
#define APPMANAGER_HPP

#include <iostream>
#include <QWindow>
#include <QPushButton>
#include <memory>
#include <QObject>
#include <QStackedWidget>
#include <QMainWindow>
#include <QString>
#include "model/dbhandler.hpp"
#include "model/user.hpp"
#include "model/loginhandler.hpp"
#include "gui/loginmenu.hpp"
#include "gui/usermenu.hpp"
#include "gui/insertdialog.hpp"
#include "model/appmodel.hpp"

#define LOGINMENU_INDEX 0
#define USERMENU_INDEX 1

class Appmodel;

class AppManager : public QObject
{
    Q_OBJECT

public:
    AppManager(Appmodel &model);
    void ShowWindow();
    void ShowUserMenu();
    void ShowMessageBox(std::string message);
    
public slots:
    void AuthenticateUser(QString login, QString password);
    void RegisterUser(QString login, QString password);
    // Creates a Menu with the given data for the user
    void InitializeUserMenu();
    void InsertNewEntry(std::string &login, std::string &password, std::string &source);
    void DeleteSelectedEntry(int entryID);
    void EditSelectedEntry(int, std::string&, std::string&, std::string&);
    void FetchUserEntriesAndRefresh();
    void LogoutUser();
    void DeleteDB();

    private:
    Appmodel&                       mAppmodel;
    std::unique_ptr<QMainWindow>    pMainWindow;
    LoginMenu                       *pLoginMenu;
    UserMenu                        *pUserMenu;
    QStackedWidget                  *pMenuStack;
};

#endif