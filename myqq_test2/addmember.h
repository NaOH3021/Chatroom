// addmember.h

#ifndef ADDMEMBER_H
#define ADDMEMBER_H

#include <QWidget>

namespace Ui {
class addMember;
}

class addMember : public QWidget
{
    Q_OBJECT

public:
    explicit addMember(const QString &groupName, QWidget *parent = nullptr);
    ~addMember();
     bool userExists(const QString &username);

signals:
    void memberAdded(const QString &memberName);

private slots:
    void onAddMemberClicked();

private:
    Ui::addMember *ui;
    QString groupName;


};

#endif
