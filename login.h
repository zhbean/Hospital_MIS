#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QDateTime>
#include <QTimer>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private:
    Ui::Login *ui;
public slots:
    void timerUpdate(void);
};

#endif // LOGIN_H
