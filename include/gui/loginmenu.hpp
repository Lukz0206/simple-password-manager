#ifndef LOGINMENU_HPP
#define LOGINMENU_HPP

#include <QWidget>
#include <QObject>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QString>


class LoginMenu : public QWidget
{
    Q_OBJECT

public:
    explicit LoginMenu(QWidget *parent = nullptr);
    void SetErrorMessage(std::string msg);

signals:
    void LoginRequested(QString login, QString password);
    void RegisterRequested(QString login, QString password);

private:
    QLineEdit    *mLoginField;
    QLineEdit    *mPasswordField;
    QPushButton  *mRegisterUserButton;
    QPushButton  *mLoginButton;
    QHBoxLayout  *mButtonLayout;
    QVBoxLayout  *mLayout;
    QLabel       *mErrorLabel;

    bool AllTextFieldsValid();
    void OnLoginClicked();
    void OnRegisterClicked();
};

#endif