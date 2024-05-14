// filerec.cpp
#include "filerec.h"
#include "ui_filerec.h"

#include <QFileDialog>
#include <QStandardPaths>

FileRec::FileRec(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileRec),
    tcpServer(new QTcpServer(this)),
    tcpSocket(nullptr),
    fileSize(0),
    bytesReceived(0)
{
    ui->setupUi(this);

    //connect(tcpServer, &QTcpServer::newConnection, this, &FileRec::newConnection);
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

    // 获取ip地址
    for (const QHostAddress &address : qAsConst(ipAddressesList)) {
        if (address != QHostAddress::LocalHost && address.toIPv4Address()) {
            if (address.toString().startsWith("192.168.")) {
                ipAddress = address.toString();
                break;
            }
        }
    }
    qDebug()<<ipAddress;
    if (ipAddress.isEmpty()) {
        qDebug() << "Unable to determine LAN IP address. Using localhost.";
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    }

    if (!tcpServer->listen(QHostAddress("0.0.0.0"),8666)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << "Server started on IP " << ipAddress << " port " << tcpServer->serverPort();
    }

    connect(tcpServer, &QTcpServer::newConnection, this, &FileRec::newConnection);

    qDebug() << "Server listening on IP " << ipAddress << " port " << tcpServer->serverPort();
}

FileRec::~FileRec()
{
    delete ui;
}

void FileRec::newConnection()
{
    qDebug() << "New connection established!";

    // 新连接的 socket
    QTcpSocket* clientSocket = tcpServer->nextPendingConnection();
    if (clientSocket) {
        // 处理接收到的数据
        connect(clientSocket, &QTcpSocket::readyRead, this, &FileRec::onReadyRead);

        // 将新连接的客户端加入列表
        clients.append(clientSocket);
    }
}

void FileRec::onReadyRead()
{

    QString sender,receiver,FileName;
    for (QTcpSocket* client : clients) {
        if (client->bytesAvailable() > 0) {
            qDebug()<<"55555";
            QByteArray data = client->readAll();
            QDataStream in(data);
            in>>sender>>receiver>>FileName>>fileSize;
            qDebug()<<sender<<receiver<<FileName<<fileSize;
            if (fileSize == 0) {
                if (client->bytesAvailable() < sizeof(qint64)) {
                    qDebug()<<"?";
                    return;
                }

                //in >> fileSize;
            }
            qDebug()<<"6";

                            // 文件内容
                            QByteArray fileData;
                            in >> fileData;

                            // 桌面路径
                            QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

                            // 文件的路径
                            QString fileName = QFileDialog::getSaveFileName(this, "Save File", desktopPath, "All Files (*)");

                            if (!fileName.isEmpty()) {
                                QFile receivedFile(fileName);
                                if (receivedFile.open(QIODevice::WriteOnly)) {
                                    receivedFile.write(fileData);
                                    receivedFile.close();
                                    emit fileReceived(fileName);
                                }
                            }

                            client->close();
                            tcpServer->close();
                            emit connectionClosed();


}
        // 传输进度
        bytesReceived+=client->bytesAvailable();
                   int progress = static_cast<int>((bytesReceived * 100) / fileSize);
                   ui->progressBar->setValue(progress);
    }
}

void FileRec::on_closeBtn_clicked()
{


    tcpServer->close();
    emit connectionClosed();
    this->close();
}

