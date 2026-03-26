#include "gui/loginmenu.hpp"

LoginMenu::LoginMenu(QWidget *parent) : QWidget(parent)
{

    this->resize(400, 300);

    // Hauptlayout (Vertikal)
    mLayout = new QVBoxLayout(this);
    mLayout->addStretch();

    // LineFields for login data
    mLoginField = new QLineEdit();
    mLoginField->setPlaceholderText("Login...");

    mPasswordField = new QLineEdit();
    mPasswordField->setPlaceholderText("Password...");
    mPasswordField->setEchoMode(QLineEdit::Password);

    // Initialize the horizontal Layout for the Buttons
    mButtonLayout = new QHBoxLayout();

    mLoginButton = new QPushButton("Log in");
    
    mRegisterUserButton = new QPushButton("Register");
    
    // Adding Buttons for lower Layout
    mButtonLayout->addWidget(mRegisterUserButton);
    mButtonLayout->addWidget(mLoginButton);
    
    // Error-Label
    mErrorLabel = new QLabel("Hallo");
    mErrorLabel->setStyleSheet("color: red; font-weight: bold;");
    mErrorLabel->setAlignment(Qt::AlignCenter); // Zentriert den Fehlertext
    mErrorLabel->hide();
    
    mLayout->addWidget(mLoginField);
    mLayout->addWidget(mPasswordField);
    // Add buttonLayout as part of main layout
    mLayout->addLayout(mButtonLayout);
    mLayout->addWidget(mErrorLabel);
    
    mLayout->addStretch();
    mLayout->setSpacing(5);
    mLayout->setContentsMargins(10, 10, 10, 10);

    //Connect Buttons with Actions
    connect(mLoginButton, &QPushButton::clicked, this, &LoginMenu::OnLoginClicked);
    connect(mRegisterUserButton, &QPushButton::clicked, this, &LoginMenu::OnRegisterClicked);
}


bool LoginMenu::AllTextFieldsValid()
{
    QString login = mLoginField->text();
    QString password = mPasswordField->text();
    if (login.isEmpty() || password.isEmpty())
    {
        mErrorLabel->setText("Please enter your login and password!");
        mErrorLabel->show();
        return false;
    }
    else
    {
        return true;
    }
}

void LoginMenu::OnLoginClicked()
{
    mErrorLabel->hide();
    QString login = mLoginField->text();
    QString password = mPasswordField->text();
    if (AllTextFieldsValid())
    {
        emit(LoginRequested(login, password));
    }
}

void LoginMenu::OnRegisterClicked()
{
    mErrorLabel->hide();
    QString login = mLoginField->text();
    QString password = mPasswordField->text();
    if(AllTextFieldsValid())
    {
        emit(RegisterRequested(login, password));
    }
}

void LoginMenu::SetErrorMessage(std::string msg)
{
    mErrorLabel->setText(msg.c_str());
    mErrorLabel->show();
}