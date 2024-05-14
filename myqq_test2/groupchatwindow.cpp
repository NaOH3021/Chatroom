#include "groupchatwindow.h"
#include "ui_groupchatwindow.h"

#include <QMessageBox>
#include <QKeyEvent>
#include <QFileDialog>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

groupchatwindow::groupchatwindow(const QString& groupName, const QString& myName, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::groupchatwindow),
    groupName(groupName),
    myName(myName),  // 设置 myName
    tcpSocket(new QTcpSocket(this)),
    onlineStatus(new QLabel(this))
{
    ui->setupUi(this);
    ui->labelGroupName->setText(groupName);

    // 打开文件）
    QFile file("group_member_list_"+groupName+".txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 处理文件打开错误
        return;
    }

    // 读取文件内容并添加
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
                QStringList parts = line.split(","); // 用户名和密码之间用,分隔
                QString username = parts.at(0);
                ui->listWidget->addItem(username);
    }

    // 添加背景图片
        QPixmap bkgnd(":/groupchatwindow_pic.jpg");
        bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);

        QPalette palette;
        palette.setBrush(QPalette::Background, bkgnd);
        this->setPalette(palette);

        QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
        db.setHostName("127.0.0.1");
        db.setDatabaseName("chatrecords_2");
        db.setUserName("root");
        db.setPassword("hjnxxmhxs050804!");
        db.setPort(3306);

        if (!db.open()) {
            qDebug() << "Failed to open database connection: " << db.lastError().text();
            return;
        }

        // 群组消息
            QString queryString = "SELECT sender, message, timestamp FROM chatrecords_2 WHERE receiver=:groupName ORDER BY timestamp";

            QSqlQuery query;
            query.prepare(queryString);
            query.bindValue(":groupName", groupName);

            if (query.exec()) {
                while (query.next()) {
                    // 查询结果
                    QString sender = query.value(0).toString();
                    QString message = query.value(1).toString();
                    QDateTime timestamp = query.value(2).toDateTime();

                    // 进行显示
                    // 结果添加到聊天记录中
                    ui->textBrowser->append(sender + "\t" + timestamp.toString("yyyy-MM-dd hh:mm:ss") + "\n" + message);
                }
            } else {
                qDebug() << "Failed to fetch group messages from database: " << query.lastError().text();
            }

            ui->textBrowser->append("—————以上为历史消息—————");
    // 处理接收消息
    connect(tcpSocket, &QTcpSocket::readyRead, this, &groupchatwindow::onReadyRead);

    connect(ui->backgroundSetButton, &QPushButton::clicked, this, &groupchatwindow::on_backgroundSetButton_clicked);
    // 连接服务器
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

    tcpSocket->connectToHost(QHostAddress(ipAddress), 1234);

    qDebug() << "group "<< groupName <<" connect to IP " << ipAddress;


}

QString groupchatwindow::getIP(){
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

QString groupchatwindow::getUsr(){
    return myName;
}


groupchatwindow::~groupchatwindow()
{
    delete ui;
    delete tcpSocket;
}

void groupchatwindow::sndMsg(MsgType type,QString srvaddr)
{
    QString message = ui->textEdit->toPlainText();
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString address=getIP();

    out<<type;
    switch (type) {
    case Group:

        out << myName << groupName << message << address;
        //tcpSocket->write(data);
        qDebug() <<"output:"<<myName<<groupName<<message<<"!"<<address;//检验

        // 清空输入框
        ui->textEdit->clear();
        ui->textBrowser->append(myName +"\t"+ QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\n"+ message);


        break;

    }
    tcpSocket->write(data);

}

void groupchatwindow::keyPressEvent(QKeyEvent *event)
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

void groupchatwindow::onEnterPressed(){
    MsgType type=Group;
    QString message = ui->textEdit->toPlainText();
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString address=getIP();

    out<<type;
    switch (type) {
    case Group:

        out << myName << groupName << message << address;
        //tcpSocket->write(data);
        qDebug() <<"output:"<<myName<<groupName<<message<<"!"<<address;//检验

        // 清空输入框
        ui->textEdit->clear();
         ui->textBrowser->append(myName +"\t"+ QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\n"+ message);


        break;

    }
    tcpSocket->write(data);

}
void groupchatwindow::on_statusNewer_clicked()
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    MsgType type=Request;
    out<<type;
    //tcpSocket->write(data);


    tcpSocket->write(data);

    qDebug()<<"send request to server";



}


void groupchatwindow::on_sndButton_clicked()
{
    sndMsg(Group);
}

bool groupchatwindow::findMember(QString memberName){
    QString groupMemberListFileName = "group_member_list_" + groupName + ".txt";
        QFile groupMemberListFile(groupMemberListFileName);

        if (groupMemberListFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&groupMemberListFile);

            while (!in.atEnd()) {
                QString line = in.readLine();
                if (line.trimmed() == memberName) {
                    groupMemberListFile.close();
                    return true; // 成员名找到
                }
            }

            groupMemberListFile.close();
        }

        return false; // 成员名未找到
}

/*bool groupchatwindow::findGroup(QString groupName){

}*/

bool groupchatwindow::userExists(const QString &username) {
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

void groupchatwindow::membernewer(){

    // 打开文件
    QFile file("group_member_list_"+groupName+".txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 文件打开错误
        return;
    }

    // 读取文件内容并添加到 QListWidget
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
                QStringList parts = line.split(","); // 用户名和密码之间用,分隔
                QString username = parts.at(0);
                ui->listWidget->addItem(username);
    }

}


void groupchatwindow::onReadyRead(){
    // 处理接收到的消息






    QByteArray data = tcpSocket->readAll();
    QDataStream stream(&data, QIODevice::ReadOnly);
    MsgType type;
    QString sender,groupName,message,ipAddress;
    stream >> type;
    switch (type) {
    case Reply:{
        do {
            QString username;
            Status status;
            //ui->listWidget->clear();
            stream >> username >> status;
            if(username=="") continue;
            if(status==Offline&&findMember(username)) ui->listWidget->addItem(username + " Offline");

            if(status==Online&&findMember(username)) ui->listWidget->addItem(username + " Online");
            stream >> type ;

        }
        while (type==Reply);
        break;
    }
    case Group:{
        stream >> sender >> groupName >> message >> ipAddress;
        if(findMember(sender)&&groupName==this->groupName){
            ui->textBrowser->append(sender + "\t" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\n" + message);
            break;

        }
    }

    }
}

void groupchatwindow::on_pushButton_clicked()
{
    //  addMember 窗口对象
        addMember *addMemberWindow = new addMember(groupName);

        // 连接 addMember 窗口的 memberAdded 信号到槽函数
        connect(addMemberWindow, &addMember::memberAdded, this, &groupchatwindow::onMemberAdded);

        //  addMember 窗口
        addMemberWindow->show();

}
void groupchatwindow::onMemberAdded(const QString &memberName)
{
    //<-->
    ui->listWidget->clear();
    // 打开文件
    QFile file("group_member_list_"+groupName+".txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 处理文件打开错误
        return;
    }

    // 读取文件内容并添加
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
                QStringList parts = line.split(","); // 用户名和密码之间用,分隔
                QString username = parts.at(0);
                ui->listWidget->addItem(username);
    }
}

void groupchatwindow::on_pushButton_2_clicked()
{
    // 当前登录用户的用户名和好友的用户名
        QString currentUser = myName;
        QString groupname = groupName;

        // 导出文件名
        QString exportFileName = currentUser + "_chatingroup_" + groupname + ".txt";

        // 写入内容
        QFile exportFile(exportFileName);
        if (exportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&exportFile);

            // 写入聊天记录
            stream << ui->textBrowser->toPlainText();

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

void groupchatwindow::on_backgroundSetButton_clicked()
{
    // 图片文件路径
    QString filePath = QFileDialog::getOpenFileName(this, "Select Background Image", "", "Images (*.png *.jpg *.jpeg *.bmp *.gif)");

    if (!filePath.isEmpty()) {
        // 设置背景图片
        QPixmap background(filePath);
        background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
        QPalette palette;
        palette.setBrush(QPalette::Background, background);
        this->setPalette(palette);
    }
}
