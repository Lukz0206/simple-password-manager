#include "controller/appmanager.hpp"
#include <QMessageBox>

AppManager::AppManager(Appmodel &model) : mAppmodel(model)
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
    connect(pLoginMenu, &LoginMenu::ResetRequested, this, &AppManager::DeleteDB);
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
        ShowMessageBox("Error while accessing database! Please restart app");
        return;
    }

    switch (status)
    {
    case NOLOGINFOUND:
        pLoginMenu->SetErrorMessage("Login not found");
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
            ShowMessageBox("Error while creating new user! Please try again!");
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
    FetchUserEntriesAndRefresh();
    // Connect button actions
    connect(pUserMenu, &UserMenu::AddEntryButtonRequest, this, &AppManager::InsertNewEntry);
    connect(pUserMenu, &UserMenu::DeleteEntryRequest, this, &AppManager::DeleteSelectedEntry);
    connect(pUserMenu, &UserMenu::EditEntryRequest, this, &AppManager::EditSelectedEntry);
    connect(pUserMenu, &UserMenu::BackToMenuRequest, this, &AppManager::LogoutUser);
    pMenuStack->addWidget(pUserMenu);
}

void AppManager::InsertNewEntry(std::string &login, std::string &password, std::string &source)
{
    try
    {
        mAppmodel.InsertNewEntry(login, password, source);
        FetchUserEntriesAndRefresh();
    }
    catch (const DBException &e)
    {
        ShowMessageBox("Couldn't add entry to database!");
    }
}

void AppManager::ShowMessageBox(std::string message)
{
    QMessageBox messageBox;
    messageBox.setText(message.c_str());
    messageBox.exec();
    return;
}

void AppManager::DeleteSelectedEntry(int entryID)
{
    try
    {
        mAppmodel.DeleteEntry(entryID);
        FetchUserEntriesAndRefresh();
    }
    catch (std::exception &e)
    {
        ShowMessageBox("Couldn't delete entry");
    }
}

void AppManager::EditSelectedEntry(int id, std::string &newLogin, std::string &newPassword, std::string &newSource)
{
    try
    {
        mAppmodel.EditEntry(id, newLogin, newPassword, newSource);
        FetchUserEntriesAndRefresh();
    }
    catch (const std::exception &e)
    {
        ShowMessageBox("Error while editing entry");
    }
}

void AppManager::FetchUserEntriesAndRefresh()
{
    try
    {
        std::string currentUserLogin = mAppmodel.GetCurrentUser().mLogin;
        std::vector<PasswordEntry> entries = mAppmodel.ReadAllPasswordEntriesFromUser(currentUserLogin);
        pUserMenu->RefreshEntries(entries);
    }
    catch (const std::exception &e)
    {
        ShowMessageBox("Error while fetching and decrypting data");
    }
}

void AppManager::LogoutUser()
{
    mAppmodel.Reset();
    pLoginMenu->Reset();
    pMenuStack->setCurrentIndex(LOGINMENU_INDEX);
    // Makes the old usermenu unusable
    pUserMenu->deleteLater();
}

void AppManager::DeleteDB()
{
    try
    {
        mAppmodel.DeleteAllData();
        ShowMessageBox("Data successfully deleted!");
    }
    catch(const std::exception& e)
    {
        ShowMessageBox("An error occured. Could not delete data");
    }
}