#ifndef INSERTDIALOG_HPP
#define INSERTDIALOG_HPP

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>

class InsertDialog : public QDialog {
    Q_OBJECT

public:
    InsertDialog(QWidget *parent = nullptr);

    QString GetUsername() const;
    QString GetPassword() const;
    QString GetSource() const;
    void CheckInput();
    void SetUsernameText(std::string);
    void SetPasswordText(std::string);
    void SetSourceText(std::string);

private:
    QLineEdit *pUsernameEdit;
    QLineEdit *pPasswordEdit;
    QLineEdit *pSourceEdit;
    QPushButton *pOKButton;
    QPushButton *pCancelButton;
};

#endif