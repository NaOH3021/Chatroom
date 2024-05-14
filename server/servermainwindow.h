// servermainwindow.h
#ifndef SERVERMAINWINDOW_H
#define SERVERMAINWINDOW_H

#include <QMainWindow>
#include <QNetworkInterface>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

QT_BEGIN_NAMESPACE
namespace Ui { class ServerMainWindow; }
QT_END_NAMESPACE

enum MsgType{Msg,UsrEnter,UsrLeft,FileName,Refuse,Request,Reply,Group};//区分不同的广播
enum Status{Online,Offline};

class ServerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ServerMainWindow(QWidget *parent = nullptr);
    ~ServerMainWindow();
    QTcpSocket* findClientByIpAddress(const QString& ipAddress);
    QString getIP();


private slots:
    void newConnection();
    void onReadyRead();

private:
    Ui::ServerMainWindow *ui;
    QTcpServer *tcpServer;
    QList<QTcpSocket*> clients;
    QSqlDatabase database;
};

#endif // SERVERMAINWINDOW_H
