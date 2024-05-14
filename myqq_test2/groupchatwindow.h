#ifndef GROUPCHATWINDOW_H
#define GROUPCHATWINDOW_H

#include <QWidget>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QLabel>
#include "friendchatwindow.h"
#include <QFile>
#include "addmember.h"


namespace Ui {
class groupchatwindow;
}

class groupchatwindow : public QWidget
{
    Q_OBJECT

public:
    explicit groupchatwindow(const QString& groupName, const QString& myName, QWidget *parent=nullptr);
    ~groupchatwindow();
    void usrEnter(QString usrname,QString ipaddr);//新用户加入
    void usrLeft(QString usrname,QString time);//用户离开
    void sndMsg(MsgType type,QString srvaddr="");//广播ip
    QString getIP();
    QString getUsr();
    bool findMember(QString memberName);
    bool findGroup(QString groupName);
    bool userExists(const QString &username);
    void membernewer();


protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_sndButton_clicked();
    void on_statusNewer_clicked();
    void onReadyRead();

    void on_pushButton_clicked();
    //void onAddMemberClicked();
    void onMemberAdded(const QString &memberName);

    void on_pushButton_2_clicked();

    void onEnterPressed();

private slots:
    void on_backgroundSetButton_clicked();


private:
    Ui::groupchatwindow *ui;
    QString groupName;
    QString myName;
    QTcpSocket *tcpSocket;
    QLabel* onlineStatus;

};

#endif
