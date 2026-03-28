#include "gui/usermenu.hpp"
#include <iostream>
#include "gui/insertdialog.hpp"
#include "gui/confirmdialog.hpp"


UserMenu::UserMenu(QWidget *parent) : QWidget(parent)
{
    // Initialize main layout 
    pMainLayout = new QVBoxLayout();
    this->setLayout(pMainLayout);
    
    //Split the screen in the middle
    pSplitter = new QSplitter();
    pMainLayout->addWidget(pSplitter);
    
    //Listwidget to store every button
    pListView = new QListView();
    
    pSplitter->addWidget(pListView);
    
    // Placeholder for detailview
    pDetailLabel = new QLabel("Select an entry");
    pSplitter->addWidget(pDetailLabel);
    
    QHBoxLayout *bottomBar = new QHBoxLayout();
    pAddBtn = new QPushButton("Add Entry");
    pEditBtn = new QPushButton("Edit Entry");
    pDeleteBtn = new QPushButton("Delete Entry");
    pDeleteBtn->setEnabled(false);
    
    bottomBar->addWidget(pAddBtn);
    bottomBar->addWidget(pEditBtn);
    bottomBar->addWidget(pDeleteBtn);
    
    pMainLayout->addLayout(bottomBar);
    
    pEntryModel = new QStandardItemModel(this);
    pListView->setModel(pEntryModel);
    pListView->setEditTriggers(QListView::NoEditTriggers);

    // Handle mouseclick on ListView
    connect(pListView, &QListView::clicked, this, &UserMenu::HandleListClick);
    //Connect addButton action
    connect(pAddBtn, &QPushButton::pressed, this, &UserMenu::OnAddEntryButtonPressed);
    //Connect deleteButton action
    connect(pDeleteBtn, &QPushButton::pressed, this, &UserMenu::OnDeleteButtonPressed);
    //Connect editButton action
    connect(pEditBtn, &QPushButton::pressed, this, &UserMenu::OnEditButtonPressed);
}

// Show a AddEntryDialog on Buttonpress
void UserMenu::OnAddEntryButtonPressed()
{
    InsertDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        std::string sourceLogin = dialog.GetUsername().toStdString();
        std::string sourcePassword = dialog.GetPassword().toStdString();
        std::string source = dialog.GetSource().toStdString();
        emit AddEntryButtonRequest(sourceLogin, sourcePassword, source);
    };
}

void UserMenu::OnDeleteButtonPressed()
{
    ConfirmDialog confirmDialog(this);
    if (confirmDialog.exec() == QDialog::Accepted)
    {
        int entryID = mPasswordEntries.at(mSelectedIndex).mID;
        emit(DeleteEntryRequest(entryID));
    }
    //Turn off delete button after pressing once -> user needs to explicitly select another entry
    pDeleteBtn->setEnabled(false);
}

UserMenu::~UserMenu()
{

}

void UserMenu::RefreshEntries(std::vector<PasswordEntry> &entries)
{
    mPasswordEntries = entries;
    pEntryModel->clear();
    for (size_t i = 0; i < entries.size(); i++)
    {
        PasswordEntry &entry = entries.at(i);
        QStandardItem* item = new QStandardItem(entry.mSource.c_str());
        item->setData(static_cast<int>(i), Qt::UserRole);
        pEntryModel->appendRow(item);
    }

    if(entries.size() == 0)
    {
        pDetailLabel->setText("Select an entry");
        pDeleteBtn->setEnabled(false);
    }
    pDetailLabel->setText("Select an entry");
}

void UserMenu::HandleListClick(const QModelIndex &index)
{
    pDeleteBtn->setEnabled(true);
    mSelectedIndex = index.data(Qt::UserRole).toInt();
    const PasswordEntry &entry = mPasswordEntries.at(mSelectedIndex);
    pDetailLabel->setText(QString::fromStdString(std::format("Login: {}\nPassword: {}", entry.mLogin, entry.mPassword)));
}

void UserMenu::OnEditButtonPressed()
{
    InsertDialog dialog(this);
    PasswordEntry currentEntry = mPasswordEntries.at(mSelectedIndex);
    dialog.SetUsernameText(currentEntry.mLogin);
    dialog.SetPasswordText(currentEntry.mPassword);
    dialog.SetSourceText(currentEntry.mSource);
    if (dialog.exec() == QDialog::Accepted)
    {
        int entryID = currentEntry.mID;
        std::string newUsername = dialog.GetUsername().toStdString();
        std::string newPassword = dialog.GetPassword().toStdString();
        std::string newSource = dialog.GetSource().toStdString();
        emit EditEntryRequest(entryID, newUsername, newPassword, newSource);
    };
}