#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFile>
#include <QTextStream>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QtGlobal>
#include <QCoreApplication>
//#include <windows.h>
#include <chatwindow.h>
#include <QKeyEvent>


namespace Ui {
class administrator;
}

//enum MsgType{Msg,UsrEnter,UsrLeft,FileName,Refuse,Request,Reply};
//enum Status{Online,Offline};

class administrator : public QWidget
{
    Q_OBJECT

public:
    explicit administrator(QWidget *parent = nullptr);
    ~administrator();
    QString getIP();

private:
    Ui::administrator *ui;
    QTcpSocket *tcpSocket;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:

    void onReadyRead();
    //void on_statusNewer_clicked();

    void on_statusNewer_2_clicked();

    void onEnterPressed();
};

#endif
