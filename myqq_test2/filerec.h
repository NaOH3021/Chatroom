// filerec.h
#ifndef FILEREC_H
#define FILEREC_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include "friendchatwindow.h"

namespace Ui {
class FileRec;
}

class FileRec : public QWidget
{
    Q_OBJECT

public:
    explicit FileRec(QWidget *parent = nullptr);
    ~FileRec();

signals:
    void fileReceived(const QString &filePath);
    void connectionClosed();

private slots:
    //void on_newConnection();
    //void on_readyRead();
    void on_closeBtn_clicked();

private slots:
    void newConnection();
    void onReadyRead();


private:
    Ui::FileRec *ui;
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;
    QFile file;
    qint64 fileSize;
    qint64 bytesReceived;
    QList<QTcpSocket*> clients;
    QString receiver;
    QString sender;
};

#endif
