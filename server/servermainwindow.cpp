// servermainwindow.cpp
#include "servermainwindow.h"
#include "ui_servermainwindow.h"
#include <QDateTime>
#include <QDir>
#include <QTcpSocket>

ServerMainWindow::ServerMainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::ServerMainWindow),
      tcpServer(new QTcpServer(this)),
    database(QSqlDatabase::addDatabase("QODBC"))
{
    ui->setupUi(this);

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
    qDebug()<<ipAddress;
    if (ipAddress.isEmpty()) {
        qDebug() << "Unable to determine LAN IP address. Using localhost.";
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    }
    /*QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    if (!db.isValid()) {
        qWarning() << "Could not load MySQL driver.";
        qWarning() << db.lastError();
        //
    }*/

    QString currentDir = QDir::currentPath();
    qDebug() << "Current Working Directory: " << currentDir;

    // Database connection
        database.setHostName("192.168.94.107");
        database.setPort(3306);
        database.setDatabaseName("chatrecords_2");
        database.setUserName("root");
        database.setPassword("hjnxxmhxs050804!");

        if (!database.open()) {
            qDebug() << "Failed to connect to the database:" << database.lastError().text();
            //return;
        }
    if (!tcpServer->listen(QHostAddress("0.0.0.0"),1234)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << "Server started on IP " << ipAddress << " port " << tcpServer->serverPort();
    }

    connect(tcpServer, &QTcpServer::newConnection, this, &ServerMainWindow::newConnection);

    qDebug() << "Server listening on IP " << ipAddress << " port " << tcpServer->serverPort();

}

ServerMainWindow::~ServerMainWindow()
{
    // 关闭服务器
    tcpServer->close();
     database.close();

    // 断开所有客户端的连接
    for (QTcpSocket* client : clients) {
        client->disconnectFromHost();
        client->deleteLater();
    }

    delete ui;
}

void ServerMainWindow::newConnection()
{
    qDebug() << "New connection established!";

    // 获取新连接的 socket
    QTcpSocket* clientSocket = tcpServer->nextPendingConnection();
    if (clientSocket) {
        // 连接信号到槽函数，处理接收到的数据
        connect(clientSocket, &QTcpSocket::readyRead, this, &ServerMainWindow::onReadyRead);

        // 将新连接的客户端加入列表
        clients.append(clientSocket);
    }
}

void ServerMainWindow::onReadyRead()
{
    for (QTcpSocket* client : clients) {
        if (client->bytesAvailable() > 0) {
            QByteArray data = client->readAll();
            QDataStream stream(&data, QIODevice::ReadOnly);

            QString sender, message, receiver, ipAddress, groupName;
            MsgType type;
            stream >> type;

            switch (type) {
            case Msg: {
                stream >> sender >> receiver >> message >> ipAddress;
                qDebug() << "receive:" << sender << receiver << message << "!" << ipAddress;
                QSqlQuery query;
                QString sqlQuery = QString("INSERT INTO chatrecords_2 (sender, receiver, message, timestamp) VALUES ('%1', '%2', '%3', CURRENT_TIMESTAMP)")
                                    .arg(sender).arg(receiver).arg(message);

                if (!query.exec(sqlQuery)) {
                    qDebug() << "Failed to insert chat record into the database:" << query.lastError().text();
                }



                // Find the target client by receiver's IP address
                for (QTcpSocket* otherClient : clients) {
                    if (otherClient != client) {
                        otherClient->write(data);
                    }
                }


                break;
            }
            case UsrEnter: {
                stream >> ipAddress >> sender;
                qDebug() << sender << "is online on"<<ipAddress; // 检验

                // Send UsrEnter message to all clients except the sender
                for (QTcpSocket* otherClient : clients) {
                    if (otherClient != client) {
                        otherClient->write(data);
                    }
                }

                break;
            }
            case UsrLeft:{
                stream >> ipAddress >> sender;
                qDebug() << sender << "is offline!!!"<<ipAddress; // 检验
                for (QTcpSocket* otherClient : clients) {

                        otherClient->write(data);

                }


                break;
            }
            case Request:{
                qDebug() << "send request to all the clients"; // 检验
                for (QTcpSocket* otherClient : clients) {

                        otherClient->write(data);

                }
                //要不要if？


                break;
            }
            case Reply:{
                Status status;
                qDebug() << "receive reply from all the clients"; // 检验
                stream >> sender >> status ;
                for (QTcpSocket* otherClient : clients) {

                        otherClient->write(data);

                }
                //要不要if？
                break;

            }
            case Group:{

                stream >> sender >> groupName >> message >> ipAddress;
                qDebug() << "receive group chat:" << sender << groupName << message << "!" << ipAddress;
                // 将消息写入数据库
                    QString queryString = "INSERT INTO chatrecords_2 (sender, receiver, message, timestamp) VALUES (:sender, :receiver, :message, :timestamp)";

                    QSqlQuery query;
                    query.prepare(queryString);
                    query.bindValue(":sender", sender);
                    query.bindValue(":receiver", groupName);
                    query.bindValue(":message", message);
                    query.bindValue(":timestamp", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

                    if (!query.exec()) {
                        qDebug() << "Failed to insert group message into database: " << query.lastError().text();
                    }
                for (QTcpSocket* otherClient : clients) {
                    if (otherClient != client) {
                        otherClient->write(data);

                    }
                }

            }
            case FileName:{

            }
            }
        }
    }
}

QString ServerMainWindow::getIP(){
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

QTcpSocket* ServerMainWindow::findClientByIpAddress(const QString& ipAddress)
{
    for (QTcpSocket* client : clients) {
        if (client->peerAddress().toString() == ipAddress) {
            return client;
        }
    }
    return nullptr;
}
