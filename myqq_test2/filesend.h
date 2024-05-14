#ifndef FILESEND_H
#define FILESEND_H

#include <QWidget>
#include <QTcpSocket>
#include <QFile>

namespace Ui {
class FileSend;
}

class FileSend : public QWidget
{
    Q_OBJECT

public:
    explicit FileSend(QString sender,QString receiver,QWidget *parent = nullptr);
    ~FileSend();

    void sendFile(const QString &filePath, const QString &receiver, const QString &sender);

signals:
    void fileTransferProgress(qint64 bytesSent, qint64 fileSize);
    void fileTransferFinished();

private slots:
    void onConnected();
    void onBytesWritten(qint64 bytes);
    void onDisconnected();

    void on_openFileBtn_clicked();

    void on_fileSendBtn_clicked();

    void on_closeBtn_clicked();

private:
    Ui::FileSend *ui;

    QTcpSocket *tcpSocket;
    QFile file;
    QString receiver;
    QString sender;
    QString FilePath;
};

#endif
