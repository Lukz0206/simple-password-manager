#include "controller/appmanager.hpp"
#include <QMessageBox>


AppManager::AppManager( Appmodel &model) : mAppmodel(model)
{
    mAppmodel = model;
    pMainWindow = std::make_unique<QMainWindow>();
    pMainWindow->resize(400, 600);
    pLoginMenu = new LoginMenu();
    pMenuStack = new QStackedWidget();
    pMenuStack->addWidget(pLoginMenu);
    pMenuStack->setCurrentIndex(LOGINMENU_INDEX);
    pMainWindow->setCentralWidget(pMenuStack);

    // Connect slot with action
    // LoginButton in main menu with Loginhandler
    connect(pLoginMenu, &LoginMenu::LoginRequested, this, &AppManager::AuthenticateUser);
    connect(pLoginMenu, &LoginMenu::RegisterRequested, this, &AppManager::RegisterUser);
}
void AppManager::ShowWindow()
{
    pMainWindow->show();
}

void AppManager::AuthenticateUser(QString login, QString password)
{
    std::string loginStr = login.toStdString();
    std::string passwdStr = password.toStdString();
    int status;
    try
    {
        status = mAppmodel.AuthenticateUser(loginStr, passwdStr);
    }
    catch (const DBException &e)
    {
        ShowErrorbox("Error while accessing database! Please restart app");
        return;
    }

    switch (status)
    {
    case NOLOGINFOUND:
        pLoginMenu->SetErrorMessage("Login or password wrong");
        break;
    case INCORRECTLOGIN:
        pLoginMenu->SetErrorMessage("Incorrect login or password");
        break;
    case APP_OK:
        InitializeUserMenu();
        ShowUserMenu();
        break;
    default:
        std::cerr << "Unhandled case in auth" << std::endl;
        break;
    }
}

void AppManager::RegisterUser(QString login, QString password)
{
    std::cout << "Registering User" << std::endl;
    Loginhandler loginhandler;

    std::string loginStr = login.toStdString();
    std::string passwordStr = password.toStdString();

    if (mAppmodel.IsMasterLoginInDB(loginStr) == false)
    {
        User user = loginhandler.CreateNewUser(loginStr, passwordStr);
        try
        {
            mAppmodel.AddNewMasterLogin(user);
        }
        catch (const DBException &e)
        {
            ShowErrorbox("Error while creating new user! Please try again!");
        }
    }
    else
    {
        pLoginMenu->SetErrorMessage("Login not available");
    }
}

void AppManager::ShowUserMenu()
{
    pMenuStack->setCurrentIndex(USERMENU_INDEX);
}

void AppManager::InitializeUserMenu()
{
    pUserMenu = new UserMenu();
    User currentUser = mAppmodel.GetCurrentUser();
    std::vector<PasswordEntry> userEntries;

    try
    {
        userEntries = mAppmodel.ReadAllPasswordEntriesFromUser(currentUser.mLogin);
    }
    catch (const std::exception &e)
    {
        ShowErrorbox("Couldn't decrypt your passwords!");
        return;
    }

    // Connect button actions
    connect(pUserMenu, &UserMenu::AddEntryButtonRequest, this, &AppManager::InsertNewEntry);
    connect(pUserMenu, &UserMenu::DeleteEntryRequest, this, &AppManager::DeleteSelectedEntry);
    pMenuStack->addWidget(pUserMenu);
    pUserMenu->RefreshEntries(userEntries);
}

void AppManager::InsertNewEntry(std::string &login, std::string &password, std::string &source)
{
    std::cout << std::format("{},{},{}", login, password, source) << std::endl;
    try
    {
        mAppmodel.InsertNewEntry(login, password, source);        
        User currentUser = mAppmodel.GetCurrentUser();
        std::vector<PasswordEntry> entries = mAppmodel.ReadAllPasswordEntriesFromUser(currentUser.mLogin);
        pUserMenu->RefreshEntries(entries);
    }
    catch (const DBException &e)
    {
        ShowErrorbox("Couldn't add entry to database!");
    }
}

void AppManager::ShowErrorbox(std::string message)
{
    QMessageBox errorBox;
    errorBox.setText(message.c_str());
    errorBox.exec();
    return;
}

void AppManager::DeleteSelectedEntry(int entryID)
{
    std::cout << "Delete requested: " << entryID << std::endl; 
}