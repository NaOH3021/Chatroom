#ifndef CHATRECORDS_H
#define CHATRECORDS_H

#include <QWidget>
#include <QList>
#include <QTableWidget>

#include <QtSql/QSqlDatabase>
#include <QNetworkInterface>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "chatsearch.h"

// 结构体用于表示聊天记录
/*struct ChatRecord {
    QString sender;
    QString receiver;
    QString message;
    QString timestamp;
};*/

namespace Ui {
class ChatRecords;
}

class ChatRecords : public QWidget
{
    Q_OBJECT

public:
    explicit ChatRecords(QString username,QWidget *parent = nullptr);
    ~ChatRecords();
    QString getIP();
    bool deleteChatRecord(int recordId);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    // 查询按钮的槽函数
    void on_queryButton_clicked();

    void onDeleteButtonClicked();


    //void on_tableWidget_itemClicked(QTableWidgetItem *item);
    void on_tableWidget_itemSelectionChanged();

    void on_pushButton_clicked();

    void on_exportButton_clicked();

    void onEnterPressed();

private:
    Ui::ChatRecords *ui;

    // 查询聊天记录的函数
    QList<ChatRecord> getChatRecords(const QString &account) const;

    QSqlDatabase database;
    QString username;

private:

    int selectedRecordId;

};

#endif
