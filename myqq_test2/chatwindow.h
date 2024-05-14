#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QTcpSocket>
#include <QNetworkInterface>
#include "friendchatwindow.h"
#include "groupchatwindow.h"
#include "chatrecords.h"


namespace Ui {
class chatwindow;
}

//enum MsgType{Msg,UsrEnter,UsrLeft,FileName,Refuse};//区分不同的广播
//enum Status{Online,Offline};
class chatwindow : public QWidget
{
    Q_OBJECT

public:
    explicit chatwindow(const QString &username, QWidget *parent = nullptr);
    ~chatwindow();
    bool userExists(const QString &username);

    QString getIP();
    void sndMsg(MsgType type,QString srvaddr , QString username);
    bool findGroup(QString groupName);
    bool isMyfriend(QString friendName);
    Status m_status=Offline;
    //dys:6
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void sendMessage();
    void addFriend();
    void addGroup();
    void onListWidgetGroupContextMenu(const QPoint &pos);


private slots:

    void onReadyRead();

    void on_listWidgetFriend_itemDoubleClicked(QListWidgetItem *item);

    //void on_listWidgetGroup_doubleClicked(const QModelIndex &index);

    void on_listWidgetGroup_itemDoubleClicked(QListWidgetItem *item);

    void on_sendButton_clicked();

    void on_pushButton_clicked();

    void onEnterPressed();

    //void on_ButtonAddGroup_clicked();

public slots:
    void deleteFriend();
    void deleteGroup();

private:
    QString username;
    QTextEdit *chatDisplay;
    QLineEdit *messageInput;
    QPushButton *sendButton;
    QListWidget *listWidgetFriend;
    QPushButton *buttonAddFriend;

    QTcpSocket *tcpSocket;
    Ui::chatwindow *ui;
private:
    QListWidgetItem *currentItem;




    void setupFriendList();
    void setupGroupList();
protected:
    void closeEvent(QCloseEvent *event) override;



};

#endif
