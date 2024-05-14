#ifndef CONFIRM_H
#define CONFIRM_H

#include <QWidget>
#include "ui_confirm.h"

namespace Ui {
class confirm;
}

class confirm : public QWidget
{
    Q_OBJECT

public:
    explicit confirm(QString password,QWidget *parent = nullptr);
    ~confirm();

signals:
    void passwordConfirmed(bool result);

private slots:
    void on_confirmButton_clicked();

private:
    Ui::confirm *ui;
    QString password;
};

#endif
