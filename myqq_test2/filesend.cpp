#include "filesend.h"
#include "ui_filesend.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QHostAddress>
#include <QFileDialog>
#include <QFileInfo>

FileSend::FileSend(QString sender, QString receiver, QWidget *parent) :

    ui(new Ui::FileSend),
    QWidget(parent),
    receiver(receiver),
    sender(sender),


    tcpSocket(new QTcpSocket(this))
{
    ui->setupUi(this);

    connect(tcpSocket, &QTcpSocket::connected, this, &FileSend::onConnected);
    connect(tcpSocket, &QTcpSocket::bytesWritten, this, &FileSend::onBytesWritten);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &FileSend::onDisconnected);
    //QString filePath = QFileDialog::getOpenFileName(this, "Select File to Send", "", "All Files (*)");
    //qDebug()<<filePath;
    //sendFile(filePath, receiver, sender);
}

FileSend::~FileSend()
{
    delete ui;
}

void FileSend::sendFile(const QString &filePath, const QString &receiver, const QString &sender)
{
    this->receiver = receiver;
    this->sender = sender;

    // 打开文件
    file.setFileName(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "打开文件读取错误：" << file.errorString();
        return;
    }

    // 连接服务器
    tcpSocket->connectToHost(QHostAddress("192.168.94.84"), 8666);
}

void FileSend::onConnected()
{
    // 发送文件数据
    QFileInfo fileInfo(file.fileName());
    qint64 fileSize = file.size();

    QByteArray metadata;
    QDataStream metadataStream(&metadata, QIODevice::WriteOnly);
    metadataStream << sender << receiver << fileInfo.fileName() << fileSize<<file.readAll();

    tcpSocket->write(metadata);

    // 移至文件开头
    //file.seek(0);

    // 发送文件内容
    //QByteArray fileData = file.readAll();
    //cpSocket->write(fileData);
}


void FileSend::onBytesWritten(qint64 bytes)
{
    // 传输进度
        int progress = static_cast<int>((bytes * 100) / file.size());
        ui->progressBar->setValue(progress);
    emit fileTransferProgress(bytes, file.size());

    if (bytes == file.size()) {
        tcpSocket->disconnectFromHost();
        file.close();
        emit fileTransferFinished();
    }
}


void FileSend::onDisconnected()
{
    qDebug() << "与服务器断开连接";
}

void FileSend::on_openFileBtn_clicked()
{
    QString FilePath = QFileDialog::getOpenFileName(this, "Select File to Send", "", "All Files (*)");
    this->FilePath=FilePath;
    qDebug()<<FilePath;

    // 进度条
        ui->progressBar->setValue(0);
}

void FileSend::on_fileSendBtn_clicked()
{
    qDebug()<<FilePath;
    sendFile(FilePath, receiver, sender);
}

void FileSend::on_closeBtn_clicked()
{
    this->close();
}
