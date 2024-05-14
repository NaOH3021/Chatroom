#include "administrator.h"
#include "ui_administrator.h"

administrator::administrator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::administrator)
{
    ui->setupUi(this);
    tcpSocket = new QTcpSocket(this);

    // 打开文件）
    QFile file("user_data.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 处理文件打开错误
        return;
    }

    // 读取文件内容添加到QList
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
                QStringList parts = line.split(","); // 假设用户名和密码之间用,分隔
                QString username = parts.at(0); // 获取用户名
                ui->listWidget->addItem(username);
    }

    // 添加背景图片
        QPixmap bkgnd(":/admin.jpg.jpg");
        bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);

        QPalette palette;
        palette.setBrush(QPalette::Background, bkgnd);
        this->setPalette(palette);


    connect(tcpSocket, &QTcpSocket::readyRead, this, &administrator::onReadyRead);
    tcpSocket->connectToHost(QHostAddress(getIP()), 1234);
    file.close();


}

administrator::~administrator()
{
    delete ui;
}

QString administrator::getIP(){
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

    if (ipAddress.isEmpty()) {
        qDebug() << "Unable to determine LAN IP address. Using localhost.";
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    }
    return ipAddress;
}

void administrator::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        onEnterPressed();
    }
    else
    {
        // 其他键盘事件
        QWidget::keyPressEvent(event);
    }
}

void administrator::onEnterPressed(){
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    MsgType type=Request;
    out<<type;
    //tcpSocket->write(data);


        tcpSocket->write(data);

        qDebug()<<"send request to server";



}

void administrator::on_statusNewer_2_clicked()
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    MsgType type=Request;
    out<<type;
    //tcpSocket->write(data);


        tcpSocket->write(data);

        qDebug()<<"send request to server";



}
void administrator::onReadyRead(){

        QByteArray data = tcpSocket->readAll();
        QDataStream stream(&data, QIODevice::ReadOnly);
        MsgType type;
        stream >> type ;

        switch (type) {
        case Reply:{
            do {
                QString username;
                Status status;
                //ui->listWidget->clear();
                stream >> username >> status;
                if(username=="") continue;
                if(status==Offline) ui->listWidget->addItem(username + " Offline");

                if(status==Online) ui->listWidget->addItem(username + " Online");
                stream >> type ;

            }
            while (type==Reply);


            break;

        }
        case UsrEnter:{
             QString ipAdress,sender;
             stream>>ipAdress>>sender;
             ui->listWidget->addItem(sender + " Online");
             break;

        }
        case UsrLeft:{
             QString ipAdress,sender;
             stream>>ipAdress>>sender;
             ui->listWidget->addItem(sender + " Offline");
             break;

        }
        }




}
