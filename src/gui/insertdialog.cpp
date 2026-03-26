#include "gui/insertdialog.hpp"

InsertDialog::InsertDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Login");

    pUsernameEdit = new QLineEdit(this);

    pPasswordEdit = new QLineEdit(this);
    pPasswordEdit->setEchoMode(QLineEdit::Password);

    pSourceEdit = new QLineEdit(this);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Username:", pUsernameEdit);
    formLayout->addRow("Password:", pPasswordEdit);
    formLayout->addRow("Source:", pSourceEdit);

    pOKButton = new QPushButton("OK");
    pCancelButton = new QPushButton("Cancel");

    //Deactivate Button on show
    pOKButton->setEnabled(false);


    connect(pOKButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(pCancelButton, &QPushButton::clicked, this, &QDialog::reject);

    connect(pUsernameEdit, &QLineEdit::textChanged, this, &InsertDialog::CheckInput);
    connect(pPasswordEdit, &QLineEdit::textChanged, this, &InsertDialog::CheckInput);
    connect(pSourceEdit, &QLineEdit::textChanged, this, &InsertDialog::CheckInput);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(pOKButton);
    buttonLayout->addWidget(pCancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
}

QString InsertDialog::GetUsername() const
{
    return pUsernameEdit->text();
}

QString InsertDialog::GetPassword() const
{
    return pPasswordEdit->text();
}
QString InsertDialog::GetSource() const
{
    return pSourceEdit->text();
}

void InsertDialog::CheckInput()
{
    if (!pUsernameEdit->text().isEmpty() &&
        !pPasswordEdit->text().isEmpty() &&
        !pSourceEdit->text().isEmpty())
    {
        pOKButton->setEnabled(true);
    }
}