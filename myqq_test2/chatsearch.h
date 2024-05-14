#ifndef CHATSEARCH_H
#define CHATSEARCH_H

#include <QWidget>
#include <QList>
#include <QString>
#include <QWidget>
#include <QList>
#include <QTableWidget>
#include <QSqlDatabase>
#include <QtSql/QSqlDatabase>
#include <QNetworkInterface>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

namespace Ui {
class ChatSearch;
}

struct ChatRecord {
    QString sender;
    QString receiver;
    QString message;
    QString timestamp;
};

class ChatSearch : public QWidget
{
    Q_OBJECT

public:
    explicit ChatSearch(QSqlDatabase database, QString username,QWidget *parent = nullptr);
    ~ChatSearch();
    void exportToTextFile(const QString &fileName);
    QString getIP();
    bool deleteChatRecord(int recordId);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_searchButton_clicked();
    void on_exportButton_clicked();

    void onEnterPressed();

    void on_deleteButton_clicked();

    void on_tableWidget_itemSelectionChanged();

private:
    Ui::ChatSearch *ui;

    QList<ChatRecord> getChatRecords(const QString &searchType, const QString &searchValue) const;
    void populateTable(const QList<ChatRecord> &records);
    QString username;
    QSqlDatabase database;

    int selectedRecordId;
};

#endif
