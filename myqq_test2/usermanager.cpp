// Server/usermanager.cpp
#include "usermanager.h"

#include <QHostAddress>
#include <QNetworkInterface>

UserManager::UserManager() {
    // 初始化已有用户数据
    // 加载用户信息
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

bool UserManager::registerUser(const QString& username, const QString& password) {
    //用户注册
    // 检查用户名是否存在
    users.append(User(username, password));
    return true;
}

QString UserManager::getIp(){
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

bool UserManager::loginUser(const QString& username, const QString& password) {
    //用户登录
    //检查用户名和密码是否匹配
    for (const User& user : users) {
        if (user.getUsername() == username && user.getPassword() == password) {
            return true;  // 登录成功
        }
    }
    return false;  // 登录失败
}
