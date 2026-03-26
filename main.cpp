#include <iostream>
#include <QPushButton>
#include <QApplication>
#include <memory>
#include <sqlite3.h>
#include "model/loginhandler.hpp"
#include "model/dbhandler.hpp"
#include "model/user.hpp"
#include "controller/appmanager.hpp"
#include "model/appmodel.hpp"

int main(int argc, char *argv[])
{
    std::shared_ptr<DBHandler> dbHandler = std::make_shared<DBHandler>(DBHandler("data/UserLogin.db"));
    QApplication application(argc, argv);
    Appmodel model(dbHandler);
    AppManager manager(model);
    model.SetManager(&manager);

    manager.ShowWindow();
    return application.exec();
}