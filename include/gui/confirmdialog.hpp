#ifndef CONFIRMDIALOG_HPP
#define CONFIRMDIALOG_HPP

#include <QDialog>
class ConfirmDialog : public QDialog
{
    Q_OBJECT
public:
    ConfirmDialog(QWidget *parent = nullptr);
    ~ConfirmDialog();
private:
};

#endif