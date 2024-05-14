#ifndef LOGIN_H
#define LOGIN_H

#include <QFile>
#include <QMessageBox>

#include <QTextStream>
#include <QDebug>

#include <QWidget>

#include "chatwindow.h"
#include <QTcpSocket>
#include <QNetworkInterface>

#include "administrator.h"

namespace Ui {
class login;
}

//enum MsgType{Msg,UsrEnter,UsrLeft,FileName,Refuse};//区分不同的广播

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    bool userExists(const QString &username) {
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

    ~login();
     QString getIP();
     void sndMsg(MsgType type,QString srvaddr , QString username);

private slots:
    void on_LoginButton_clicked();

    void on_RegisterButton_clicked();

    void onReadyRead();

    void onEnterPressed();

    void onEnterPressed_2();

private:
    Ui::login *ui;
    QTcpSocket *tcpSocket;
};

#endif
