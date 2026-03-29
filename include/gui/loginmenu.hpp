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
    void Reset();

signals:
    void LoginRequested(QString login, QString password);
    void RegisterRequested(QString login, QString password);
    void ResetRequested();

private:
    QLineEdit    *pLoginField;
    QLineEdit    *pPasswordField;
    QPushButton  *pRegisterUserButton;
    QPushButton  *pLoginButton;
    QPushButton  *pResetButton;
    QHBoxLayout  *pButtonLayout;
    QVBoxLayout  *pLayout;
    QLabel       *pErrorLabel;

    bool AllTextFieldsValid();
    void OnResetClicked();
    void OnLoginClicked();
    void OnRegisterClicked();
};

#endif