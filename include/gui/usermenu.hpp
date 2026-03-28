#ifndef USERMENU_HPP
#define USERMENU_HPP

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QSplitter>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <vector>
#include <QStandardItemModel>
#include <model/passwordentry.hpp>

class UserMenu : public QWidget
{
    Q_OBJECT
public:
    UserMenu(QWidget *parent = nullptr);
    void RefreshEntries(std::vector<PasswordEntry> &entries);
    ~UserMenu();

signals:
    void AddEntryButtonRequest(std::string &login, std::string &password, std::string &source);
    void DeleteEntryRequest(int entryID);
    void EditEntryRequest(int, std::string&, std::string&, std::string&);
private:
    QLabel *pDetailLabel;
    QStandardItemModel *pEntryModel;
    std::vector<PasswordEntry> mPasswordEntries;
    QVBoxLayout *pMainLayout;  
    QSplitter *pSplitter;
    QListView *pListView;
    QPushButton *pAddBtn;
    QPushButton *pEditBtn;
    QPushButton *pDeleteBtn;
    void OnAddEntryButtonPressed();
    void HandleListClick(const QModelIndex &index);
    void OnDeleteButtonPressed();
    void OnEditButtonPressed();
    int mSelectedIndex;
};

#endif