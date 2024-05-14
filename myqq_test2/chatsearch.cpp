#include "chatsearch.h"
#include "ui_chatsearch.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QKeyEvent>

ChatSearch::ChatSearch(QSqlDatabase database, QString username, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatSearch),
    database(database),
    username(username)
{
    ui->setupUi(this);

    // 搜索类型选项
    ui->comboBox->addItem("发送者");
    ui->comboBox->addItem("接收者");
    ui->comboBox->addItem("消息内容");
    ui->comboBox->addItem("时间");

    // 添加背景图片
        QPixmap bkgnd(":/chatsearch_pic.jpg");
        bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);

        QPalette palette;
        palette.setBrush(QPalette::Background, bkgnd);
        this->setPalette(palette);


        connect(ui->deleteButton, &QPushButton::clicked, this, &ChatSearch::on_deleteButton_clicked);
    // 连接到数据库
    /*database.setHostName("127.0.0.1");
    database.setPort(3306);
    database.setDatabaseName("chatrecords_2");
    database.setUserName("root");
    database.setPassword("hjnxxmhxs050804!");*/

    if (!database.open()) {
        qDebug() << "Failed to connect to the database:" << database.lastError().text();
    }
}

ChatSearch::~ChatSearch()
{
    delete ui;
}

void ChatSearch::on_searchButton_clicked()
{
    //搜索类型和搜索内容
    QString searchType = ui->comboBox->currentText();
    QString searchValue = ui->valueLineEdit->text();

    // 查询语句
    QString queryString;
    qDebug() << "Search Type:" << searchType << "Search Value:" << searchValue;

    if (searchType == "发送者") {
            queryString = "SELECT * FROM chatrecords_2 WHERE sender = :value AND (sender = :username OR receiver = :username OR :username = 'admin')";
        } else if (searchType == "接收者") {
            queryString = "SELECT * FROM chatrecords_2 WHERE receiver = :value AND (sender = :username OR receiver = :username OR :username = 'admin')";
    } else if (searchType == "消息内容") {
        queryString = "SELECT * FROM chatrecords_2 WHERE message LIKE :value AND (sender = :username OR receiver = :username)";
    } else if (searchType == "时间") {
        // 时间戳转化
        QDateTime timestamp = QDateTime::fromString(searchValue, "yyyy-MM-dd hh:mm:ss");
        queryString = "SELECT * FROM chatrecords_2 WHERE timestamp = :value AND (sender = :username OR receiver = :username)";
    }

    // 查询
    QSqlQuery query;
    query.prepare(queryString);
    query.bindValue(":value", searchValue);
    query.bindValue(":username", username);

    if (query.exec()) {
        // 清空表格
        ui->tableWidget->clearContents();
        ui->tableWidget->setRowCount(0);

        // 填充表格
        int row = 0;
        while (query.next()) {
            ui->tableWidget->insertRow(row);
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(query.value("sender").toString()));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(query.value("receiver").toString()));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(query.value("message").toString()));
            ui->tableWidget->setItem(row, 3, new QTableWidgetItem(query.value("timestamp").toDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            ++row;
        }
    } else {
        qDebug() << "Failed to execute query:" << query.lastError().text();
    }

}

QString ChatSearch::getIP(){
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

void ChatSearch::keyPressEvent(QKeyEvent *event)
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

void ChatSearch::onEnterPressed(){
    // 搜索类型和搜索内容
    QString searchType = ui->comboBox->currentText();
    QString searchValue = ui->valueLineEdit->text();

    // 查询语句
    QString queryString;
    qDebug() << "Search Type:" << searchType << "Search Value:" << searchValue;

    if (searchType == "发送者") {
            queryString = "SELECT * FROM chatrecords_2 WHERE sender = :value AND (sender = :username OR receiver = :username OR :username = 'admin')";
        } else if (searchType == "接收者") {
            queryString = "SELECT * FROM chatrecords_2 WHERE receiver = :value AND (sender = :username OR receiver = :username OR :username = 'admin')";
    } else if (searchType == "消息内容") {
        queryString = "SELECT * FROM chatrecords_2 WHERE message LIKE :value AND (sender = :username OR receiver = :username)";
    } else if (searchType == "时间") {
        // 转换输入的时间字符串为时间戳
        QDateTime timestamp = QDateTime::fromString(searchValue, "yyyy-MM-dd hh:mm:ss");
        queryString = "SELECT * FROM chatrecords_2 WHERE timestamp = :value AND (sender = :username OR receiver = :username)";
    }

    // 查询
    QSqlQuery query;
    query.prepare(queryString);
    query.bindValue(":value", searchValue);
    query.bindValue(":username", username);

    if (query.exec()) {
        // 清空表格
        ui->tableWidget->clearContents();
        ui->tableWidget->setRowCount(0);

        // 填充表格
        int row = 0;
        while (query.next()) {
            ui->tableWidget->insertRow(row);
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(query.value("sender").toString()));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(query.value("receiver").toString()));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(query.value("message").toString()));
            ui->tableWidget->setItem(row, 3, new QTableWidgetItem(query.value("timestamp").toDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            ++row;
        }
    } else {
        qDebug() << "Failed to execute query:" << query.lastError().text();
    }

}

void ChatSearch::on_exportButton_clicked()
{
    // 搜索结果的行数
    int rowCount = ui->tableWidget->rowCount();

    if (rowCount == 0) {
        // 显示警告
        QMessageBox::warning(this, "警告", "表格内容为空，无法导出到文件！");
        return;
    }

    // 搜索类型和搜索内容
    QString searchType = ui->comboBox->currentText();
    QString searchValue = ui->valueLineEdit->text();

    // 文件名
    //QString fileName = QString("%1.txt").arg(searchValue);

    // 导出到文件
    exportToTextFile(username + "_search.txt");

    // 导出成功
    QMessageBox::information(this, "成功", QString("导出到文件  成功！"));
}

void ChatSearch::exportToTextFile(const QString &fileName)
{
    QFile file(fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // 写入表头
        out << "Sender\tReceiver\tMessage\tTimestamp\n";

        // 写入表格内容
        int rowCount = ui->tableWidget->rowCount();
        int columnCount = ui->tableWidget->columnCount();

        for (int row = 0; row < rowCount; ++row) {
            for (int col = 0; col < columnCount; ++col) {
                out << ui->tableWidget->item(row, col)->text() << "\t";
            }
            out << "\n";
        }

        file.close();
    } else {
        qDebug() << "Failed to open file for writing:" << file.errorString();
    }
}

void ChatSearch::on_deleteButton_clicked()
{
    if (selectedRecordId != -1) {
        // 删除
        QSqlQuery query;
        query.prepare("DELETE FROM chatrecords_2 WHERE id = :id");
        query.bindValue(":id", selectedRecordId);

        if (query.exec()) {
            qDebug() << "Chat record deleted successfully!";
        } else {
            qDebug() << "Failed to delete chat record:" << query.lastError().text();
        }

        // 刷新聊天记录
        onEnterPressed();
    } else {
        qDebug() << "No record selected for deletion.";
    }

}

void ChatSearch::on_tableWidget_itemSelectionChanged()
{
    // 当前选中
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

bool ChatSearch::deleteChatRecord(int recordId)
{
    QSqlQuery query(database);

    // 删除
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
