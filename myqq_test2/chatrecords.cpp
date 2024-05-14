#include "chatrecords.h"
#include "ui_chatrecords.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include <QDateTime>
#include <QMessageBox>
#include <QKeyEvent>


ChatRecords::ChatRecords(QString username, QWidget *parent) :
    username(username),
    QWidget(parent),
    ui(new Ui::ChatRecords),
    //selectedRow(0),
    database(QSqlDatabase::addDatabase("QODBC"))
{
    ui->setupUi(this);
    QFile file("user_data.txt");  // 实际的文件路径
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList parts = line.split(',');
                if (!parts.isEmpty()) {
                    QString account = parts.first().trimmed();
                    ui->comboBox->addItem(account);
                }
            }
            file.close();
        } else {
            qDebug() << "Failed to open user_data.txt";
        }
        int index = ui->comboBox->findText(username);
            if (index != -1) {
                ui->comboBox->setCurrentIndex(index);
            } else {
                qDebug() << "Username not found in the combo box.";
            }

            // 添加背景图片
                QPixmap bkgnd(":/chatrecord_pic.jpg");
                bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);

                QPalette palette;
                palette.setBrush(QPalette::Background, bkgnd);
                this->setPalette(palette);

    // 初始化账户
    // 从数据库中获取账户列表
    // Database
        database.setHostName("127.0.0.1");
        database.setPort(3306);
        database.setDatabaseName("chatrecords_2");
        database.setUserName("root");
        database.setPassword("hjnxxmhxs050804!");
        connect(ui->deleteButton, &QPushButton::clicked, this, &ChatRecords::onDeleteButtonClicked);


        if (!database.open()) {
            qDebug() << "Failed to connect to the database:" << database.lastError().text();
            //return;
        }
}

ChatRecords::~ChatRecords()
{
    delete ui;
}

void ChatRecords::on_queryButton_clicked()
{
    // 获取选择的账户
    QString selectedAccount = ui->comboBox->currentText();

    // 查询数据库，获取聊天记录
    // 数据库查询
    QList<ChatRecord> records = getChatRecords(selectedAccount);

    // 清空表格
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(records.size());

    // 填充表格
    for (int i = 0; i < records.size(); ++i) {
        const ChatRecord &record = records.at(i);
        QTableWidgetItem *senderItem = new QTableWidgetItem(record.sender);
        QTableWidgetItem *receiverItem = new QTableWidgetItem(record.receiver);
        QTableWidgetItem *messageItem = new QTableWidgetItem(record.message);
        QTableWidgetItem *timestampItem = new QTableWidgetItem(record.timestamp);

        ui->tableWidget->setItem(i, 0, senderItem);
        ui->tableWidget->setItem(i, 1, receiverItem);
        ui->tableWidget->setItem(i, 2, messageItem);
        ui->tableWidget->setItem(i, 3, timestampItem);
    }
}

QString ChatRecords::getIP(){
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

    //获取ip地址
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

void ChatRecords::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        onEnterPressed();
    }
    else
    {
        // 处理其他键盘事件
        QWidget::keyPressEvent(event);
    }
}

void ChatRecords::onEnterPressed(){
    // 获取选择的账户
    QString selectedAccount = ui->comboBox->currentText();

    // 查询数据库，获取聊天记录
    // 数据库查询
    QList<ChatRecord> records = getChatRecords(selectedAccount);

    // 清空表格
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(records.size());

    // 填充表格
    for (int i = 0; i < records.size(); ++i) {
        const ChatRecord &record = records.at(i);
        QTableWidgetItem *senderItem = new QTableWidgetItem(record.sender);
        QTableWidgetItem *receiverItem = new QTableWidgetItem(record.receiver);
        QTableWidgetItem *messageItem = new QTableWidgetItem(record.message);
        QTableWidgetItem *timestampItem = new QTableWidgetItem(record.timestamp);

        ui->tableWidget->setItem(i, 0, senderItem);
        ui->tableWidget->setItem(i, 1, receiverItem);
        ui->tableWidget->setItem(i, 2, messageItem);
        ui->tableWidget->setItem(i, 3, timestampItem);
    }

}

QList<ChatRecord> ChatRecords::getChatRecords(const QString &account) const
{
    // 获取聊天记录
    // 数据库查询
    // 返回一个包含聊天记录的列表
    QList<ChatRecord> records;
    QSqlQuery query;
        query.prepare("SELECT sender, receiver, message, timestamp FROM chatrecords_2 WHERE sender = :sender OR receiver = :receiver");
        query.bindValue(":sender", account);
        query.bindValue(":receiver", account);

        if (query.exec()) {
                while (query.next()) {
                    ChatRecord record;
                    record.sender = query.value("sender").toString();
                    record.receiver = query.value("receiver").toString();
                    record.message = query.value("message").toString();
                    record.timestamp = query.value("timestamp").toDateTime().toString("yyyy-MM-dd hh:mm:ss");
                    if(record.sender==username||record.receiver==username){
                        records.append(record);
                    }

                }
            } else {
                qDebug() << "Failed to execute query:" << query.lastError().text();
            }



    // sth wrong?????

    return records;
}

bool ChatRecords::deleteChatRecord(int recordId)
{
    QSqlQuery query(database);

    // 删除操作
    query.prepare("DELETE FROM chatrecords_2 WHERE id = :id");
    query.bindValue(":id", recordId);

    if (query.exec()) {
        qDebug() << "Chat record deleted successfully!";
        qDebug() << "Selected Record ID:" << recordId;

        return true;
    } else {
        qDebug() << "Failed to delete chat record:" << query.lastError().text();
        return false;
    }
}

void ChatRecords::onDeleteButtonClicked()
{
    if (selectedRecordId != -1) {
        // 删除操作
        QSqlQuery query;
        query.prepare("DELETE FROM chatrecords_2 WHERE id = :id");
        query.bindValue(":id", selectedRecordId);

        if (query.exec()) {
            qDebug() << "Chat record deleted successfully!";
        } else {
            qDebug() << "Failed to delete chat record:" << query.lastError().text();
        }

        // 刷新聊天记录
        on_queryButton_clicked();
    } else {
        qDebug() << "No record selected for deletion.";
    }
}


void ChatRecords::on_tableWidget_itemSelectionChanged()
{
    // 获取当前选中
    QList<QTableWidgetItem *> selectedItems = ui->tableWidget->selectedItems();

    if (selectedItems.size() > 0) {
        // 获取选中行的第一列
        QString sender = selectedItems.at(0)->text();

        // 查询数据库
        QSqlQuery query;
        query.prepare("SELECT id FROM chatrecords_2 WHERE sender = :sender");
        query.bindValue(":sender", sender);

        if (query.exec() && query.next()) {
            selectedRecordId = query.value("id").toInt();
        } else {
            qDebug() << "Failed to get selected record ID:" << query.lastError().text();
        }
    }
}

void ChatRecords::on_pushButton_clicked()
{
    ChatSearch *chatsearch=new ChatSearch(database,username);
    chatsearch->show();
}

void ChatRecords::on_exportButton_clicked()
{
    // 获取用户名
    QString currentUser = username;

    // 导出文件名
    QString exportFileName = "chatrecords_" + currentUser + ".txt";

    // 写入内容
    QFile exportFile(exportFileName);
    if (exportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&exportFile);

        // 写入表头
        stream << "Sender\tReceiver\tMessage\tTimestamp\n";

        // 写入文件
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
                QTableWidgetItem *item = ui->tableWidget->item(row, col);
                stream << item->text() << "\t";
            }
            stream << "\n";
        }

        exportFile.close();
        qDebug() << "Chat records exported to file: " << exportFileName;
        // 导出成功
                QMessageBox::information(this, "Success", "Chat records exported successfully.");
    } else {
        qDebug() << "Failed to open file for writing: " << exportFileName;
        // 导出失败
                QMessageBox::critical(this, "Error", "Failed to export chat records.");

    }
}
