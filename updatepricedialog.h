#ifndef UPDATEPRICEDIALOG_H
#define UPDATEPRICEDIALOG_H

#include <QDialog>
#include <QDebug>
namespace Ui {
class updatePriceDialog;
}

class updatePriceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit updatePriceDialog(QWidget *parent = nullptr);
    ~updatePriceDialog();

private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();
    void getDrugMessage(QString drug_id,QString drug_name,QString drug_buyPrice,QString drug_Price);

private:
    Ui::updatePriceDialog *ui;
public:
    QString MyDrugId;
    QString MyDrugName;
    QString MyDrugBuyPrice;
    QString MyDrugSalePrice;
};

#endif // UPDATEPRICEDIALOG_H
