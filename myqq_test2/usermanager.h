// Server/usermanager.h
#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>
#include <QList>
#include "user.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>


class UserManager {
public:
    UserManager();

    bool registerUser(const QString& username, const QString& password);
    bool loginUser(const QString& username, const QString& password);

    QString getIp();


private:
    QList<User> users;
    QSqlDatabase database;

};

#endif
