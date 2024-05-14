#include "adm_dnu.h"
#include "ui_adm_dnu.h"
#include <QFile>
#include <QTextStream>

Administrator::Administrator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Administrator)
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
                QStringList parts = line.split(","); // 用户名和密码之间用,分隔
                QString username = parts.at(0); // 获取用户名
                ui->listWidget->addItem(username);
    }

    file.close();
    connectToServer();
    connect(ui->statusNewer, &QPushButton::clicked, this, &Administrator::requestOnlineMembers);
    //connect(ui->pushButton, &QPushButton::clicked, this, &Administrator::handleButtonClick);
}

void Administrator::handleOnlineMembers(const QString &onlineMembers)
{
    // 在线成员列表
    QStringList membersList = onlineMembers.split(" ", Qt::SkipEmptyParts);

    // 显示在线成员
    ui->listWidget->clear();
    for (const QString &member : membersList)
    {
        ui->listWidget->addItem(member + " Offline");
    }
}

void Administrator::on_statusNewer_clicked()
{
    // 处理更新在线状态按钮点击
    // 发送请求更新在线成员信号给服务器
    emit requestOnlineMembers();
}
Administrator::~Administrator()
{
    delete ui;
}


void Administrator::requestOnlineMembers()
{
    QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << "RequestOnlineMembers";
        tcpSocket->write(data);
}

void Administrator::connectToServer()
{
    QString ipAddress = "192.168.115.1";  // 服务器 IP 地址
    quint16 port = 1234;  // 服务器端口

    tcpSocket->connectToHost(ipAddress, port);

    // 等待连接成功
    if (tcpSocket->waitForConnected(3000))
    {
        qDebug() << "Connected to server!";
    }
    else
    {
        qDebug() << "Failed to connect to server!";
    }
}
