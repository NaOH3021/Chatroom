// addmember.cpp

#include "addmember.h"
#include "ui_addmember.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>

addMember::addMember(const QString &groupName, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addMember),
    groupName(groupName)
{
    ui->setupUi(this);

    connect(ui->addMemberButton, &QPushButton::clicked, this, &addMember::onAddMemberClicked);
}

addMember::~addMember()
{
    delete ui;
}

void membernewer(){

}

bool addMember::userExists(const QString &username)
{
    QFile file("user_data.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening file for reading";
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList data = line.split(",");
        if (data.size() >= 1 && data[0] == username) {
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}

void addMember::onAddMemberClicked()
{
    QString memberName = ui->lineEdit->text().trimmed();

    if (memberName.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a valid member name.");
        return;
    }

    // 判断用户是否成功注册
    if (!userExists(memberName)) {
        QMessageBox::warning(this, "Error", "User with this name is not registered.");
        return;
    }

    // 判断用户是否已经是群聊的成员
    QString groupMemberListFileName = "group_member_list_" + groupName + ".txt";
    QFile groupMemberListFile(groupMemberListFileName);

    if (groupMemberListFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&groupMemberListFile);

        bool isAlreadyMember = false;
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line == memberName) {
                isAlreadyMember = true;
                break;
            }
        }

        groupMemberListFile.close();

        if (isAlreadyMember) {
            QMessageBox::warning(this, "Error", "User is already a member of this group.");
            return;
        }
    }

    // 向group_list_username文件中写入当前群聊名称
    QFile userGroupListFile("group_list_" + memberName + ".txt");
    if (userGroupListFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&userGroupListFile);
        out << groupName << "\n";
        userGroupListFile.close();
    } else {
        QMessageBox::warning(this, "Error", "Failed to open user group list file.");
        return;
    }

    // 向group_member_list_groupname文件中写入该成员名称
    if (groupMemberListFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&groupMemberListFile);
        out << memberName << "\n";
        groupMemberListFile.close();
    } else {
        QMessageBox::warning(this, "Error", "Failed to open group member list file.");
        return;
    }

    QMessageBox::information(this, "Success", "Member added successfully.");
    emit memberAdded(memberName);
}
