#ifndef ADDUPDATEDEV_H
#define ADDUPDATEDEV_H

#include <QWidget>
#include<QMap>
#include<QSqlQuery>
#include<UI/Mydata.h>
namespace Ui {
class addUpdateDev;
}

class addUpdateDev : public QWidget
{
    Q_OBJECT

public:
    explicit addUpdateDev(QWidget *parent = nullptr);
    ~addUpdateDev();

signals:
    void addDevData(dev_Data devdata);

private slots:
    void on_NoSpin_valueChanged(int arg1);

    void on_nameLineEdit_textChanged(const QString &arg1);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::addUpdateDev *ui;
    void prepare();

    std::vector<int> devNos;
    std::vector<QString> devNames;
    std::map<QString,int> groupMap;
};

#endif // ADDUPDATEDEV_H
