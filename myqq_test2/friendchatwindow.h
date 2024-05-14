// friendchatwindow.h
#ifndef FRIENDCHATWINDOW_H
#define FRIENDCHATWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QLabel>
#include "filerec.h"
#include "filesend.h"
//#include "groupchatwindow.h"


namespace Ui { class FriendChatWindow; }



enum MsgType{Msg,UsrEnter,UsrLeft,FileName,Refuse,Request,Reply,Group};//区分不同的广播
enum Status{Online,Offline};

class FriendChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    FriendChatWindow(const QString& friendName, const QString& myName, QWidget *parent = nullptr);
    ~FriendChatWindow();
    void usrEnter(QString usrname,QString ipaddr);//新用户加入
    void usrLeft(QString usrname,QString time);//用户离开
    void sndMsg(MsgType type,QString srvaddr="");//广播ip
    QString getIP();
    QString getUsr();




protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_pushButton_clicked();
    void onReadyRead();
    // friendchatwindow.h

    private slots:
        void on_backgroundSetButton_clicked();


   // void processPendingDatagrams();

    void on_pushButton_2_clicked();

    void on_sendFile_clicked();

    void on_recFile_clicked();

    void on_exportButton_clicked();

    void onEnterPressed();

private:
    Ui::FriendChatWindow *ui;
    QString friendName;
    QString myName;
    QTcpSocket *tcpSocket;
    QLabel* onlineStatus;
    // 从数据库中加载聊天记录
        void loadChatHistoryFromDatabase();

};

#endif
