#include "login.h"
#include "ui_login.h"
#include <QFile>
#include <QTextStream>

#include <QDebug>

#include <confirm.h>

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login),
    tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);
    // 添加背景图片
    QPixmap bkgnd(":/bcg.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);

    // 设置图片
        QPixmap pixmap(":/qqpic.jpg");
        ui->label->setPixmap(pixmap.scaled(ui->label->size(), Qt::KeepAspectRatio));
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

       ui->usernameLineEdit->setFocus();
    //处理接收消息
    connect(ui->passwordLineEdit, &QLineEdit::returnPressed, this, &login::onEnterPressed);
    connect(ui->usernameLineEdit, &QLineEdit::returnPressed, this, &login::onEnterPressed_2);

    connect(tcpSocket, &QTcpSocket::readyRead, this, &login::onReadyRead);
    QString ipAddress;
    ipAddress=getIP();

    // 按钮正常状态和悬停状态
    ui->LoginButton->setStyleSheet("QPushButton {"
                                   "    background-color: #87CEFA;"  // 浅蓝色
                                   "    color: black;"  // 黑色字体
                                   "    border: none;"
                                   "    border-radius: 5px;"
                                   "}"
                                   "QPushButton:hover {"
                                   "    background-color: #77b5fe;"  // 悬停时
                                   "}");

    //
    ui->RegisterButton->setStyleSheet("QPushButton {"
                                   "    background-color: #87CEFA;"  // 浅蓝色
                                   "    color: black;"  // 黑色字体
                                   "    border: none;"
                                   "    border-radius: 5px;"
                                   "}"
                                   "QPushButton:hover {"
                                   "    background-color: #77b5fe;"  // 悬停时
                                   "}");



    tcpSocket->connectToHost(QHostAddress(ipAddress), 1234);


}

login::~login()
{
    delete ui;
}


QString login::getIP(){
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


void login::on_RegisterButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    confirm *confirmDialog = new confirm(password);

    // 连接信号和槽函数
    connect(confirmDialog, &confirm::passwordConfirmed, this, [this, username, password](bool confirmed) {
        if (confirmed) {
            // 进行注册
            if (userExists(username)) {
                qDebug() << "User already exists, registration rejected";
                QMessageBox::warning(this, "Registration Failed", "Username already exists. Please choose a different username.");
                return;
            }

            QFile file("user_data.txt");
            if (!file.open(QIODevice::Append | QIODevice::Text)) {
                qDebug() << "Error opening file for writing";
                return;
            }

            QTextStream out(&file);
            out << username << "," << password << "\n";
            file.close();

            // 好友列表文件
            QString friendListFileName = "friend_list_" + username + ".txt";
            QFile friendListFile(friendListFileName);
            if (!friendListFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qDebug() << "Error opening friend list file for writing";
                return;
            }

            QString groupListFileName = "group_list_" + username + ".txt";
            QFile groupListFile(groupListFileName);
            if (!groupListFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qDebug() << "Error opening group list file for writing";
                return;
            }
            qDebug() << "User registered successfully";

            // 注册成功
            QMessageBox::information(this, "Registration Successful", "User registered successfully. You can now log in.");
        }
        else {
            // 用户取消
            qDebug() << "Password confirmation failed";
        }
    });

    confirmDialog->show();
    return ;
}

void login::on_LoginButton_clicked()
{
        QString username = ui->usernameLineEdit->text();
        QString password = ui->passwordLineEdit->text();

        // 进行验证
        QFile file("user_data.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "Error opening file for reading";
            return;
        }

        QTextStream in(&file);
        bool found = false;
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList data = line.split(",");
            if (data.size() == 2 && data[0] == username && data[1] == password) {
                found = true;
                break;
            }
        }

        file.close();

        if (found) {
            qDebug() << username<<"Login successful";

            if(username=="admin"){
                administrator *Administrator=new administrator;
                Administrator->show();
            }
            sndMsg(UsrEnter,getIP(),username);

            //close();

            chatwindow *chatWindow = new chatwindow(username);
            chatWindow->show();

            // 登录后
        } else {
            qDebug() << "Login failed";
            //登录失败
            QMessageBox::warning(this, "Login Failed", "Incorrect username or password. Please try again.");

        }

}

void login::onEnterPressed()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    // 读取用户信息
    QFile file("user_data.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening file for reading";
        return;
    }

    QTextStream in(&file);
    bool found = false;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList data = line.split(",");
        if (data.size() == 2 && data[0] == username && data[1] == password) {
            found = true;
            break;
        }
    }

    file.close();

    if (found) {
        qDebug() << username<<"Login successful";

        if(username=="admin"){
            administrator *Administrator=new administrator;
            Administrator->show();
        }
        sndMsg(UsrEnter,getIP(),username);

        //close();

        chatwindow *chatWindow = new chatwindow(username);
        chatWindow->show();

        // 登录后
    } else {
        qDebug() << "Login failed";
        // 登录失败
        QMessageBox::warning(this, "Login Failed", "Incorrect username or password. Please try again.");

    }
}

void login::onEnterPressed_2(){

    ui->passwordLineEdit->setFocus();
}

void login::sndMsg(MsgType type, QString srvaddr,QString username){

    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString address=getIP();
    out<<type<<address<<username;

    tcpSocket->write(data);

}

void login::onReadyRead(){
    QByteArray data = tcpSocket->readAll();
    QDataStream stream(&data, QIODevice::ReadOnly);
    MsgType type;

    stream >> type ;
    switch (type) {
    case UsrEnter:{
        QString sender , ipAddress;
        stream >> sender >> ipAddress;
        qDebug() <<"receive:"<<sender<<"is online from server";//检验

    }
    }




}
