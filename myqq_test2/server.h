// Server/server.h
#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include "usermanager.h"
#include "QFile"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>


class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server(QObject *parent = nullptr);
    bool userExists(const QString &username);
    QString getIp();

protected:
    void incomingConnection(qintptr socketDescriptor) override;
    //重写 修改虚函数

private slots:
    void readClient();
    void userDisconnected();

private:
    QList<QTcpSocket*> clients;
    UserManager userManager;
    QSqlDatabase database;

};

#endif
