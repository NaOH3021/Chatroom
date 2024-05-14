// Server/user.h
#ifndef USER_H
#define USER_H
#include "QFile"

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>


class User {
public:
    User(const QString& username, const QString& password);

    QString getUsername() const;  // 添加获取用户名的方法
    QString getPassword() const;  // 添加获取密码的方法
    bool userExists(const QString &username);

    QString getIp();
    //获取用户ip




private:
    QString username;
    QString password;
    QSqlDatabase database;

};

#endif
