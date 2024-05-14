// friendchatwindow.cpp
#include "friendchatwindow.h"
#include "ui_friendchatwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>


FriendChatWindow::FriendChatWindow(const QString& friendName, const QString& myName, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::FriendChatWindow),
      friendName(friendName),
      myName(myName),  // 设置 myName
      tcpSocket(new QTcpSocket(this)),
      onlineStatus(new QLabel(this))
{
    ui->setupUi(this);
    ui->labelFriendName->setText(friendName);

    //添加背景图片
        QPixmap bkgnd(":/friendchatwindow_pic.jpg");
        bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);

        QPalette palette;
        palette.setBrush(QPalette::Background, bkgnd);
        this->setPalette(palette);



    onlineStatus->setPixmap(QPixmap(":/offlineIcon.png").scaled(80, 20, Qt::KeepAspectRatio));  // 缩放图标
    onlineStatus->setGeometry(360, 0, 80, 20);  // 调整位置
    onlineStatus->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    onlineStatus->show();  // 显示标签

    // 发送按钮点击信号到槽函数
    //connect(ui->textEdit, &QTextEdit::, this, &FriendChatWindow::onEnterPressed);

    // 连接槽函数
    connect(ui->backgroundSetButton, &QPushButton::clicked, this, &FriendChatWindow::on_backgroundSetButton_clicked);
    // 处理接收消息
    connect(tcpSocket, &QTcpSocket::readyRead, this, &FriendChatWindow::onReadyRead);

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



    loadChatHistoryFromDatabase();

    qDebug() << "client connect to IP " << ipAddress;

    //sndMsg(UsrEnter,getIP());

}


QString FriendChatWindow::getIP(){
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

    // 指定获取ip地址
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


QString FriendChatWindow::getUsr(){
    return myName;
}


void FriendChatWindow::sndMsg(MsgType type,QString srvaddr)
{
    QString message = ui->textEdit->toPlainText();
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString address=getIP();

    out<<type;
    switch (type) {
    case Msg:

        out << myName << friendName << message << address;
                //tcpSocket->write(data);
                qDebug() <<"output:"<<myName<<friendName<<message<<"!"<<address;//检验

                // 清空输入框
                ui->textEdit->clear();
                 ui->textBrowser->append("You:\t" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\n"+ message);
                 //


        break;
    case UsrEnter:
        out<<address<<myName;
        break;
    case UsrLeft:
        out<<address<<myName;
        break;

    case Refuse:
     {
        out<<srvaddr;
        break;
    }
    }
    tcpSocket->write(data);

}


void FriendChatWindow::on_pushButton_clicked()
{

    sndMsg(Msg);
    /*
    // 发送按钮点击事件
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



    QString message = ui->textEdit->toPlainText();

    // 发送消息给服务器
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << myName << friendName << message << ipAddress;
    tcpSocket->write(data);
    qDebug() <<"output:"<<myName<<friendName<<message<<"!"<<ipAddress;//检验

    // 清空输入框
    ui->textEdit->clear();

    // 显示发送的消息
    ui->textBrowser->append("You: " + message);*/


}



void FriendChatWindow::onEnterPressed(){
    MsgType type=Msg;
    QString message = ui->textEdit->toPlainText();
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString address=getIP();

    out<<type;
    switch (type) {
    case Msg:

        out << myName << friendName << message << address;
                //tcpSocket->write(data);
                qDebug() <<"output:"<<myName<<friendName<<message<<"!"<<address;//检验

                // 清空输入框
                ui->textEdit->clear();
                ui->textBrowser->append("You:\t" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\n"+ message);


        break;
    case UsrEnter:
        out<<address<<myName;
        break;
    case UsrLeft:
        out<<address<<myName;
        break;

    case Refuse:
     {
        //out<<srvaddr;
        break;
    }
    }
    tcpSocket->write(data);


}

void FriendChatWindow::loadChatHistoryFromDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("chatrecords_2");
    db.setUserName("root");
    db.setPassword("hjnxxmhxs050804!");
    db.setPort(3306);

    if (!db.open()) {
        qDebug() << "Database connection failed.";
        return;
    }

    QSqlQuery query;
    QString queryString = "SELECT sender, receiver, message, timestamp FROM chatrecords_2 WHERE (sender=:myName AND receiver=:friendName) OR (sender=:friendName AND receiver=:myName) ORDER BY timestamp";
    query.prepare(queryString);
    query.bindValue(":myName", myName);
    query.bindValue(":friendName", friendName);

    if (!query.exec()) {
        qDebug() << "Query execution failed.";
        return;
    }

    ui->textBrowser->clear();

    while (query.next()) {
        QString sender = query.value(0).toString();
        QString receiver = query.value(1).toString();
        QString message = query.value(2).toString();
        QDateTime timestamp = query.value(3).toDateTime();



            QString formattedMessage = sender + "\t" + timestamp.toString("yyyy-MM-dd hh:mm:ss") + "\n" + message;
            ui->textBrowser->append(formattedMessage);


    }
    // 历史消息标记
    ui->textBrowser->append("—————以上为历史消息—————");


    db.close();
}

void FriendChatWindow::onReadyRead()
{
    // 处理接收到的消息






    QByteArray data = tcpSocket->readAll();
    QDataStream stream(&data, QIODevice::ReadOnly);
    MsgType type;

    stream >> type ;

    switch (type) {
    case Msg:{
        QString sender, message,receiver,ipAddress;
        stream >> sender >>receiver>> message>>ipAddress;
    qDebug() <<"receive:"<<sender<<receiver<<message<<ipAddress;//检验


    // 接收到的消息
    if(receiver==myName&&sender==friendName) {
        //time
        ui->textBrowser->append(sender + "\t" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\n" + message);

    }
    break;
    }
    case UsrEnter:{
        QString sender , ipAddress;
        stream >> sender >> ipAddress;
        qDebug() <<"receive:"<<sender<<"is online from server";//检验
        break;
    }
    case Reply:{
        do {
            QString sender;
            Status status;
            stream >> sender >> status;
            if(sender==friendName&&status==Online){
                onlineStatus->setPixmap(QPixmap(":/onlineIcon.png").scaled(80, 20, Qt::KeepAspectRatio));  // 缩放图标
                onlineStatus->setGeometry(360, 0, 80, 20);
                onlineStatus->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
                onlineStatus->show();  // 显示标签
                qDebug()<<friendName<<"is online";

            }
            else if(sender==friendName&&status==Offline){
                onlineStatus->setPixmap(QPixmap(":/offlineIcon.png").scaled(80, 20, Qt::KeepAspectRatio));
                onlineStatus->setGeometry(360, 0, 80, 20);
                onlineStatus->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
                onlineStatus->show();  // 显示标签
                qDebug()<<friendName<<"is offline";

            }
            stream >> type;
        } while (type==Reply);

        break;
    }

    }


}


FriendChatWindow::~FriendChatWindow()
{
    delete ui;
    delete tcpSocket;
}

void FriendChatWindow::on_pushButton_2_clicked()
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    MsgType type=Request;
    out<<type;
    //tcpSocket->write(data);


        tcpSocket->write(data);

        qDebug()<<"send request to server";



}

void FriendChatWindow::on_sendFile_clicked()
{

    //
        //QString filePath = QFileDialog::getOpenFileName(this, "Select File to Send", "", "All Files (*)");


            FileSend *fileSender = new FileSend(myName,friendName);
            fileSender->show();
            //qDebug()<<filePath;
            //fileSender->sendFile(filePath, friendName, myName);




}

void FriendChatWindow::on_recFile_clicked()
{

    //
        FileRec *fileRecDialog = new FileRec();
        fileRecDialog->show();
}

void FriendChatWindow::on_exportButton_clicked()
{
    // 当前登录用户的用户名和好友的用户名
        QString currentUser = myName;
        QString friendUser = friendName;

        // 导出文件名
        QString exportFileName = currentUser + "_chatwith_" + friendUser + ".txt";

        // 打开文件以写入内容
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

void FriendChatWindow::on_backgroundSetButton_clicked()
{
    // 用户选择的图片文件路径
    QString filePath = QFileDialog::getOpenFileName(this, "Select Background Image", "", "Images (*.png *.jpg *.jpeg *.bmp *.gif)");

    if (!filePath.isEmpty()) {
        // 背景图片
        QPixmap background(filePath);
        background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
        QPalette palette;
        palette.setBrush(QPalette::Background, background);
        this->setPalette(palette);
    }
}
