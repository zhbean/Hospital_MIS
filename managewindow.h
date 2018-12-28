#ifndef MANAGEWINDOW_H
#define MANAGEWINDOW_H

#include <QMainWindow>

namespace Ui {
class manageWindow;
}

class manageWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit manageWindow(QWidget *parent = nullptr);
    ~manageWindow();

private:
    Ui::manageWindow *ui;
};

#endif // MANAGEWINDOW_H
