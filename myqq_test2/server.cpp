// Server/server.cpp
#include "server.h"

#include <qnetworkinterface.h>

Server::Server(QObject *parent) : QTcpServer(parent)
{
    // 初始化
    if (!listen(QHostAddress::Any, 12345)) {
        qDebug() << "Error: Unable to start server.";
        return;
    }
    qDebug() << "Server started on port 12345.";
    database.setHostName("127.0.0.1");
    database.setPort(3306);
    database.setDatabaseName("chatrecords_2");
    database.setUserName("root");
    database.setPassword("hjnxxmhxs050804!");


    if (!database.open()) {
        qDebug() << "Failed to connect to the database:" << database.lastError().text();
        //return;
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    // 新连接
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketDescriptor);

    connect(client, &QTcpSocket::readyRead, this, &Server::readClient);
    connect(client, &QTcpSocket::disconnected, this, &Server::userDisconnected);

    clients.append(client);

    qDebug() << "New client connected.";
}

void Server::readClient()
{
    // 客户端的消息
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client)
        return;

    while (client->canReadLine()) {
        QString message = QString::fromUtf8(client->readLine()).trimmed();
        // 接收到的消息
    }
}

bool Server:: userExists(const QString &username) {
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

void Server::userDisconnected()
{
    // 用户断开连接
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (!client)
        return;

    clients.removeOne(client);
    client->deleteLater();

    qDebug() << "Client disconnected.";
}

QString Server::getIp(){
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
