#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QMainWindow>

namespace Ui {
class registerWindow;
}

class registerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit registerWindow(QWidget *parent = nullptr);
    ~registerWindow();

private:
    Ui::registerWindow *ui;
};

#endif // REGISTERWINDOW_H