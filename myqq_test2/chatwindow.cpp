// chatwindow.cpp
#include "chatwindow.h"
#include "ui_chatwindow.h"
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QKeyEvent>
#include <QMenu>



chatwindow::chatwindow(const QString &username, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chatwindow),
    username(username),
    tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);

    // 用户名标签
    ui->usernameLabel->setText("Username: " + username);

    // 连接发送点击信号到槽函数
    connect(ui->sendButton, &QPushButton::clicked, this, &chatwindow::sendMessage);

    // 连接添加好友按钮点击信号到槽函数
    connect(ui->ButtonAddFriend, &QPushButton::clicked, this, &chatwindow::addFriend);


    connect(ui->ButtonAddGroup, &QPushButton::clicked, this, &chatwindow::addGroup);

    connect(ui->listWidgetGroup, &QListWidget::customContextMenuRequested, this, &chatwindow::onListWidgetGroupContextMenu);



    // 添加背景图片
        QPixmap bkgnd(":/chatwindow_pic.jpg");
        bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);

        QPalette palette;
        palette.setBrush(QPalette::Background, bkgnd);
        this->setPalette(palette);

    // 初始化好友列表
    setupFriendList();
    setupGroupList();
    sndMsg(UsrEnter,getIP(),username);
    this->m_status=Online;
    connect(tcpSocket, &QTcpSocket::readyRead, this, &chatwindow::onReadyRead);
    //this->m_status=Online;
    QString ipAddress;
    ipAddress=getIP();

    ui->listWidgetFriend->installEventFilter(this);

    ui->listWidgetGroup->installEventFilter(this);


    tcpSocket->connectToHost(QHostAddress(ipAddress), 1234);
}

QString chatwindow::getIP(){
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

    // 获取ip地址
    for (const QHostAddress &address : qAsConst(ipAddressesList)) {
        if (address != QHostAddress::LocalHost && address.toIPv4Address()) {
            if (address.toString().startsWith("192.168.")) {
                ipAddress = address.toString();
                break;
            }
        }
    }

    if (ipAddress.isEmpty()) {
        qDebug() << "Unable to determine LAN IP address. Using localhost.";
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    }
    return ipAddress;
}

//处理右击事件
bool chatwindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::ContextMenu) {
        QContextMenuEvent *contextEvent = static_cast<QContextMenuEvent *>(event);

        if (obj == ui->listWidgetFriend) {
            currentItem = ui->listWidgetFriend->itemAt(contextEvent->pos());
            // 获取被右击的项
            QListWidgetItem *clickedItem = ui->listWidgetFriend->itemAt(contextEvent->pos());

            if (clickedItem) {
                // 创建上下文菜单
                QMenu *menu = new QMenu(ui->listWidgetFriend);
                QAction *deleteAction = menu->addAction("删除");

                // 菜单项的信号槽
                connect(deleteAction, &QAction::triggered, this, &chatwindow::deleteFriend);

                // 上下文菜单
                menu->exec(contextEvent->globalPos());
                delete menu;
            }

            return true;
        }

        if (obj == ui->listWidgetGroup) {
            currentItem = ui->listWidgetGroup->itemAt(contextEvent->pos());
            //右击的项
            QListWidgetItem *clickedItem = ui->listWidgetGroup->itemAt(contextEvent->pos());

            if (clickedItem) {
                // 上下文菜单
                QMenu *menu = new QMenu(ui->listWidgetGroup);
                QAction *deleteAction = menu->addAction("删除");

                // 菜单项的信号槽
                connect(deleteAction, &QAction::triggered, this, &chatwindow::deleteGroup);

                // 上下文菜单
                menu->exec(contextEvent->globalPos());
                delete menu;
            }

            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}

//删除好友
void chatwindow::deleteFriend() {
    if (!currentItem) {
        return;
    }

    QString friendName = currentItem->text();

    // 确认删除的对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "删除好友", "是否确认删除好友 " + friendName + "？",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    // delete friend

    // 删除好友列表中的某项
    ui->listWidgetFriend->removeItemWidget(currentItem);
    delete currentItem;
    currentItem = nullptr;

    // 删除好友列表文件中的好友名
    QString friendListFileName = "friend_list_" + username + ".txt";
    QFile friendListFile(friendListFileName);
    if (friendListFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream stream(&friendListFile);
        QStringList friendList;

        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line != friendName) {
                friendList.append(line);
            }
        }

        friendListFile.resize(0);

        for (const QString &friendUsername : friendList) {
            stream << friendUsername << "\n";
        }

        friendListFile.close();
        // 清空好友列表
        ui->listWidgetFriend->clear();

        // 打开好友列表文件
        QFile friendListFile("friend_list_" + username + ".txt");
        if (friendListFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&friendListFile);

            // 读取好友列表并添加
            QString line;
            while (!(line = stream.readLine()).isNull()) {
                ui->listWidgetFriend->addItem(line);
            }

            friendListFile.close();
        } else {
            QMessageBox::warning(this, tr("Setup Friend List"), tr("Failed to open friend list file."));
        }
    } else {
        qDebug() << "Failed to open friend list file for deletion.";
    }
}

// 删除群聊
void chatwindow::deleteGroup() {
    if (!currentItem) {
        return;
    }

    QString groupName = currentItem->text();

    //确认删除的对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "删除群聊", "是否确认删除群聊 " + groupName + "？",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    // delete group

    // 删除群聊列表中的项
    ui->listWidgetGroup->removeItemWidget(currentItem);
    delete currentItem;
    currentItem = nullptr;

    // 删除群聊列表文件中的群聊名
    QString groupListFileName = "group_list_" + username + ".txt";
    QFile groupListFile(groupListFileName);
    if (groupListFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream stream(&groupListFile);
        QStringList groupList;

        while (!stream.atEnd()) {
            QString line = stream.readLine();
            if (line != groupName) {
                groupList.append(line);
            }
        }

        groupListFile.resize(0);

        for (const QString &group : groupList) {
            stream << group << "\n";
        }

        groupListFile.close();
    } else {
        qDebug() << "Failed to open group list file for deletion.";
    }

    // 删除群聊成员列表文件中的用户名
    QString groupMemberListFileName = "group_member_list_" + groupName + ".txt";
    QFile groupMemberListFile(groupMemberListFileName);
    if (groupMemberListFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream memberStream(&groupMemberListFile);
        QStringList memberList;

        while (!memberStream.atEnd()) {
            QString memberName = memberStream.readLine();
            if (memberName != username) {
                memberList.append(memberName);
            }
        }

        groupMemberListFile.resize(0); // 清空文件内容

        for (const QString &memberName : memberList) {
            memberStream << memberName << "\n";
        }

        groupMemberListFile.close();
        // 清空群聊列表
        ui->listWidgetGroup->clear();

        // 打开群聊列表文件
        QFile groupListFile("group_list_" + username + ".txt");
        if (groupListFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&groupListFile);

            // 读取好友列表并添加
            QString line;
            while (!(line = stream.readLine()).isNull()) {
                ui->listWidgetGroup->addItem(line);
            }

            groupListFile.close();
        } else {
            QMessageBox::warning(this, tr("Setup Group List"), tr("Failed to open group list file."));
        }
    } else {
        qDebug() << "Failed to open group member list file for deletion.";
    }
}


void chatwindow::onListWidgetGroupContextMenu(const QPoint &pos)
{
    // 获取被右击
    QListWidgetItem *item = ui->listWidgetGroup->itemAt(pos);

    if (item) {
        // 获取被右击的内容
        QString selectedItemText = item->text();

        // 输出内容
        qDebug() << "Right-clicked on item: " << selectedItemText;
    }
}

void chatwindow::addFriend()
{
    // 用户输入的好友账号名
    QString friendUsername = QInputDialog::getText(this, "Add Friend", "Enter friend's username:");

    // 当前用户账号名
    QString currentUser = username;

    // 当前用户的好友列表文件名
    QString friendListFileName = "friend_list_" + currentUser + ".txt";

    // 打开好友列表文件，读取好友列表
    QFile friendListFile(friendListFileName);
    if (friendListFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&friendListFile);

        // 是否已经是好友
        bool isAlreadyFriend = false;
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line == friendUsername) {
                isAlreadyFriend = true;
                break;
            }
        }

        // 关闭文件
        friendListFile.close();

        if (isAlreadyFriend) {
            // 显示警告信息
            QMessageBox::warning(this, tr("Add Friend"), tr("You are already friends with this user."));
        } else {
            // 继续添加好友的逻辑
            if (friendListFile.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream out(&friendListFile);

                // 打开好友的好友列表文件
                QFile friendFriendListFile("friend_list_" + friendUsername + ".txt");
                if (friendFriendListFile.exists()) {
                    if (friendFriendListFile.open(QIODevice::Append | QIODevice::Text)) {
                        QTextStream friendStream(&friendFriendListFile);
                        friendStream << currentUser << "\n";
                        friendFriendListFile.close();
                    }
                } else {
                    // 弹出警告
                    QMessageBox::warning(this, tr("Add Friend"), tr("Friend's account not found. Unable to add."));
                    return;
                }

                // 将好友的用户名写入当前用户的好友列表文件
                out << friendUsername << "\n";
                //有逻辑问题？？？？？？？？？？？？

                // 关闭当前用户的好友列表文件
                friendListFile.close();

                // 更新好友列表
                setupFriendList();

                // 添加成功
                QMessageBox::information(this, tr("Add Friend"), tr("Friend added successfully."));
            } else {
                QMessageBox::warning(this, tr("Add Friend"), tr("Failed to open friend list file."));
            }
        }
    } else {
        QMessageBox::warning(this, tr("Add Friend"), tr("Failed to open friend list file."));
    }
}

void chatwindow::addGroup()
{
    // 用户输入的群聊名称
    QString groupName = QInputDialog::getText(this, "Create Group", "Enter group name:");

    // 当前用户账号名
    QString currentUser = username;

    if(groupName==""){
        QMessageBox::warning(this, tr("Create Group"), tr("Group name is empty."));
        return;
    }

    // 当前用户的群列表文件名
    QString groupListFileName = "group_list_" + currentUser + ".txt";

    // 打开群列表文件，读取群列表
    QFile groupListFile(groupListFileName);
    if (groupListFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&groupListFile);

        // 检查是否已经在该群聊中
        bool isInGroup = false;
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line == groupName) {
                isInGroup = true;
                break;
            }
        }

        // 关闭文件
        groupListFile.close();

        if (isInGroup) {
            // 显示警告
            QMessageBox::warning(this, tr("Create Group"), tr("You are already in this group."));
        } else {
            QTextStream out(&groupListFile);

            // 用户名写入成员列表文件
            QString groupMemberListFileName = "group_member_list_" + groupName + ".txt";
            QFile groupMemberListFile(groupMemberListFileName);

            // 创建群聊
            if (groupListFile.open(QIODevice::Append | QIODevice::Text)) {
                if (groupMemberListFile.exists()) {
                    // 直接打开文件
                    if (groupMemberListFile.open(QIODevice::Append | QIODevice::Text)) {
                        QTextStream groupMemberStream(&groupMemberListFile);

                        // 用户个数
                        int memberCount = QInputDialog::getInt(this, "Create Group", "Enter number of members:");

                        // 每个成员的用户名
                        for (int i = 0; i < memberCount; ++i) {
                            QString memberName = QInputDialog::getText(this, "Create Group", "Enter member name:");
                            groupMemberStream << memberName << "\n";
                        }

                        groupMemberListFile.close();
                    }
                } else {
                    // 创建文件并写入
                    if (groupMemberListFile.open(QIODevice::Append | QIODevice::Text)) {
                        QTextStream groupMemberStream(&groupMemberListFile);

                        // 用户个数
                        int memberCount = QInputDialog::getInt(this, "Create Group", "Enter number of members:");

                        // 并写入每个成员的用户名
                        for (int i = 0; i < memberCount; ++i) {
                                QString memberName = QInputDialog::getText(this, "Create Group", "Enter member name:");
                                groupMemberStream << memberName << "\n";

                                // 打开成员的群列表文件
                                QFile memberGroupListFile("group_list_" + memberName + ".txt");
                                if (memberGroupListFile.open(QIODevice::Append | QIODevice::Text)) {
                                    QTextStream memberGroupListStream(&memberGroupListFile);
                                    memberGroupListStream << groupName << "\n";
                                    memberGroupListFile.close();
                                } else {
                                    // 弹出警告
                                    QMessageBox::warning(this, tr("Create Group"), tr("Unable to open member's group list file."));
                                }
                            }

                        groupMemberListFile.close();
                    } else {
                        // 弹出警告
                        QMessageBox::warning(this, tr("Create Group"), tr("Unable to create group member list file."));
                        groupListFile.close();
                        return;
                    }
                }

                // 将群聊的名称写入当前用户的群列表文件
                out << groupName << "\n";

                // 关闭当前用户的群列表文件
                groupListFile.close();

                // 更新群列表
                setupGroupList();

                // 创建成功
                QMessageBox::information(this, tr("Create Group"), tr("Group created successfully."));
            } else {
                QMessageBox::warning(this, tr("Create Group"), tr("Failed to open group list file."));
            }
        }

    } else {
        QMessageBox::warning(this, tr("Create Group"), tr("Failed to open group list file."));
    }
}


void FriendChatWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        onEnterPressed();
    }
    else
    {
        // 处理其他键盘事件
        QWidget::keyPressEvent(event);
    }
}

void chatwindow::onEnterPressed(){
    // 清空好友列表
    ui->listWidgetFriend->clear();

    // 打开好友列表文件
    QFile friendListFile("friend_list_" + username + ".txt");
    if (friendListFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&friendListFile);

        // 读取好友列表并添加
        QString line;
        while (!(line = stream.readLine()).isNull()) {
            ui->listWidgetFriend->addItem(line);
        }

        friendListFile.close();
    } else {
        QMessageBox::warning(this, tr("Setup Friend List"), tr("Failed to open friend list file."));
    }


}
void chatwindow::on_listWidgetFriend_itemDoubleClicked(QListWidgetItem *item)
{
    QString friendUsername = item->text();
    FriendChatWindow *friendChat = new FriendChatWindow(friendUsername,username, this);
    friendChat->show();
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    MsgType type=Request;
    out<<type;
    //tcpSocket->write(data);


        tcpSocket->write(data);

        qDebug()<<"send request to server";



}

void chatwindow::on_listWidgetGroup_itemDoubleClicked(QListWidgetItem *item)
{

    QString groupname = item->text();
    //
    //
    groupchatwindow *groupChat=new groupchatwindow(groupname,username);
    groupChat->show();
}



void chatwindow::setupFriendList()
{
    // 清空好友列表
    ui->listWidgetFriend->clear();

    // 打开好友列表文件
    QFile friendListFile("friend_list_" + username + ".txt");
    if (friendListFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&friendListFile);

        // 读取好友列表并添加
        QString line;
        while (!(line = stream.readLine()).isNull()) {
            ui->listWidgetFriend->addItem(line);
        }

        friendListFile.close();
    } else {
        QMessageBox::warning(this, tr("Setup Friend List"), tr("Failed to open friend list file."));
    }
}

void chatwindow::setupGroupList(){
    // 清空群聊列表
    ui->listWidgetGroup->clear();

    // 打开群聊列表文件
    QFile groupListFile("group_list_" + username + ".txt");
    if (groupListFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&groupListFile);

        // 读取好友列表并添加
        QString line;
        while (!(line = stream.readLine()).isNull()) {
            ui->listWidgetGroup->addItem(line);
        }

        groupListFile.close();
    } else {
        QMessageBox::warning(this, tr("Setup Group List"), tr("Failed to open group list file."));
    }
}

void chatwindow::sendMessage()
{

}

bool chatwindow::isMyfriend(QString friendName){
    QString friendListFileName = "friend_list_" + username + ".txt";
        QFile friendListFile(friendListFileName);

        if (friendListFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&friendListFile);

            while (!in.atEnd()) {
                QString line = in.readLine();
                if (line.trimmed() == friendName) {
                    friendListFile.close();
                    return true; // 成员名找到
                }
            }

            friendListFile.close();
        }

        return false; // 成员名未找到
}
chatwindow::~chatwindow()
{
    qDebug()<<username<<"is offline";
    delete ui;
    delete tcpSocket;
}

void chatwindow::sndMsg(MsgType type, QString srvaddr,QString username){

    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    switch (type) {
    case UsrLeft:{
        QString address=getIP();
        out<<type<<address<<username;

        tcpSocket->write(data);
        break;
    }
    case Reply:{//bug!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        out<<type<<username<<m_status;
        tcpSocket->write(data);
        break;
    }
    case UsrEnter:{
        out<<type<<getIP()<<username;
        tcpSocket->write(data);
        break;
    }

}
}

void chatwindow::closeEvent(QCloseEvent* event) {
    this->m_status=Offline;
    sndMsg(UsrLeft,getIP(),username);
    event->accept();

}


void chatwindow::onReadyRead(){
    QByteArray data = tcpSocket->readAll();
    QDataStream stream(&data, QIODevice::ReadOnly);
    MsgType type;

    stream >> type ;
    switch (type) {
    case UsrLeft:{
        QString sender , ipAddress;
        stream >> ipAddress >> sender;
        qDebug() <<"receive:"<<sender<<"is offline from server now";//检验
        break;
    }
    case Request:{
        sndMsg(Reply,getIP(),username);
        qDebug() <<"receive request successfully";//检验
        break;
    }
    }

}

bool chatwindow::userExists(const QString &username) {
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

bool chatwindow::findGroup(QString groupName){
    QString groupListFileName = "group_list_" + username + ".txt";
        QFile groupListFile(groupListFileName);

        if (groupListFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&groupListFile);

            while (!in.atEnd()) {
                QString line = in.readLine();
                if (line.trimmed() == groupName) {
                    groupListFile.close();
                    return true; // 成员名找到
                }
            }

            groupListFile.close();
        }

        return false; // 成员名未找到
}


void chatwindow::on_sendButton_clicked()
{
ChatRecords *chatrecords= new ChatRecords(username);
chatrecords->show();
}

void chatwindow::on_pushButton_clicked()
{
    // 当前登录用户的用户名和好友的用户名
        QString currentUser = username;
        //QString friendUser = friendName;

        // 文件名
        QString exportFileName = currentUser + "_chatwith_"  + ".txt";

        // 写入内容
        QFile exportFile(exportFileName);
        if (exportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&exportFile);

            // 写入聊天记录
            stream << ui->chatDisplay->toPlainText();

            exportFile.close();
            qDebug() << "Chat records exported to file: " << exportFileName;

            // 导出成功
            QMessageBox::information(this, "Success", "Chat records exported successfully.");
        } else {
            qDebug() << "Failed to open file for writing: " << exportFileName;

            // 导出失败
            QMessageBox::critical(this, "Error", "Failed to export chat records.");
        }
}

