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
    QPushButton *addBtn = new QPushButton("Add Entry");
    QPushButton *editBtn = new QPushButton("Edit Entry");
    QPushButton *deleteBtn = new QPushButton("Delete Entry");
    
    bottomBar->addWidget(addBtn);
    bottomBar->addWidget(editBtn);
    bottomBar->addWidget(deleteBtn);
    
    pMainLayout->addLayout(bottomBar);
    
    pEntryModel = new QStandardItemModel(this);
    pListView->setModel(pEntryModel);
    pListView->setEditTriggers(QListView::NoEditTriggers);

    // Handle mouseclick on ListView
    connect(pListView, &QListView::clicked, this, &UserMenu::HandleListClick);
    //Connect addButton action
    connect(addBtn, &QPushButton::pressed, this, &UserMenu::OnAddEntryButtonPressed);
    //Connect deleteButton action
    connect(deleteBtn, &QPushButton::pressed, this, &UserMenu::OnDeleteButtonPressed);
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
    }
}

void UserMenu::HandleListClick(const QModelIndex &index)
{
    mSelectedIndex = index.data(Qt::UserRole).toInt();
    const PasswordEntry &entry = mPasswordEntries.at(mSelectedIndex);
    pDetailLabel->setText(QString::fromStdString(std::format("Login: {}\nPassword: {}", entry.mLogin, entry.mPassword)));
}