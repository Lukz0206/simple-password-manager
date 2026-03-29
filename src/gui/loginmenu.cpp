#include "gui/loginmenu.hpp"

#include <gui/confirmdialog.hpp>

LoginMenu::LoginMenu(QWidget *parent) : QWidget(parent)
{

    this->resize(400, 300);

    // Hauptlayout (Vertikal)
    pLayout = new QVBoxLayout(this);
    pLayout->addStretch();

    // LineFields for login data
    pLoginField = new QLineEdit();
    pLoginField->setPlaceholderText("Login...");

    pPasswordField = new QLineEdit();
    pPasswordField->setPlaceholderText("Password...");
    pPasswordField->setEchoMode(QLineEdit::Password);

    // Initialize the horizontal Layout for the Buttons
    pButtonLayout = new QHBoxLayout();
    pResetButton = new QPushButton("Reset DB");
    pResetButton->setFixedSize(80, 25);

    pLoginButton = new QPushButton("Log in");
    pRegisterUserButton = new QPushButton("Register");

    
    // Adding Buttons for lower Layout
    pButtonLayout->addWidget(pResetButton);
    pButtonLayout->addWidget(pRegisterUserButton);
    pButtonLayout->addWidget(pLoginButton);
    
    // Error-Label
    pErrorLabel = new QLabel("Hallo");
    pErrorLabel->setStyleSheet("color: red; font-weight: bold;");
    pErrorLabel->setAlignment(Qt::AlignCenter); // Zentriert den Fehlertext
    pErrorLabel->hide();
    
    pLayout->addWidget(pLoginField);
    pLayout->addWidget(pPasswordField);
    // Add buttonLayout as part of main layout
    pLayout->addLayout(pButtonLayout);
    pLayout->addWidget(pErrorLabel);
    
    pLayout->addStretch();
    pLayout->setSpacing(5);
    pLayout->setContentsMargins(10, 10, 10, 10);

    //Connect Buttons with Actions
    connect(pResetButton, &QPushButton::clicked, this, &LoginMenu::OnResetClicked);
    connect(pLoginButton, &QPushButton::clicked, this, &LoginMenu::OnLoginClicked);
    connect(pRegisterUserButton, &QPushButton::clicked, this, &LoginMenu::OnRegisterClicked);
}


bool LoginMenu::AllTextFieldsValid()
{
    QString login = pLoginField->text();
    QString password = pPasswordField->text();
    if (login.isEmpty() || password.isEmpty())
    {
        pErrorLabel->setText("Please enter your login and password!");
        pErrorLabel->show();
        return false;
    }
    else
    {
        return true;
    }
}

void LoginMenu::OnLoginClicked()
{
    pErrorLabel->hide();
    QString login = pLoginField->text();
    QString password = pPasswordField->text();
    if (AllTextFieldsValid())
    {
        emit(LoginRequested(login, password));
    }
}

void LoginMenu::OnRegisterClicked()
{
    pErrorLabel->hide();
    QString login = pLoginField->text();
    QString password = pPasswordField->text();
    if(AllTextFieldsValid())
    {
        emit(RegisterRequested(login, password));
    }
}

void LoginMenu::SetErrorMessage(std::string msg)
{
    pErrorLabel->setText(msg.c_str());
    pErrorLabel->show();
}

void LoginMenu::Reset()
{
    pLoginField->clear();
    pPasswordField->clear();
    pErrorLabel->clear();
}

void LoginMenu::OnResetClicked()
{
    ConfirmDialog dialog;
    if(dialog.exec() == QDialog::Accepted)
    {
        emit ResetRequested();
    }  
}