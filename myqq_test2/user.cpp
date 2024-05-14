// Server/user.cpp
#include "user.h"

#include <QTextStream>
#include <QDebug>
#include <QHostAddress>
#include <QNetworkInterface>

User::User(const QString& username, const QString& password)
    : username(username), password(password) {
    // 初始化
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

bool User::userExists(const QString &username) {
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

QString User::getIp(){
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

    //获取ip地址
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

QString User::getUsername() const {
    return username;
}

QString User::getPassword() const {
    return password;
}
