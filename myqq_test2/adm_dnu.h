#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>

namespace Ui {
class Administrator;
}

class Administrator : public QWidget
{
    Q_OBJECT

public:
    explicit Administrator(QWidget *parent = nullptr);
    ~Administrator();
    void handleButtonClick();
    void connectToServer();
    //void updateOnlineMembers(const QStringList& members);

signals:
   // void requestOnlineMembers();

        void sendOnlineMembers(const QList<QString>& onlineMembers);

public slots:
    //void updateOnlineMembers(const QStringList &members);
    void handleOnlineMembers(const QString &onlineMembers);
    void requestOnlineMembers();

private slots:
    void on_statusNewer_clicked();


  //员列表的信号

private:
    Ui::Administrator *ui;
    QTcpSocket *tcpSocket;


};

#endif
