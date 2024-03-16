#ifndef DEVICERENT_MANAGER_H
#define DEVICERENT_MANAGER_H

#include "qsqlquery.h"
#include <QWidget>

namespace Ui {
class devicerent_manager;
}

class devicerent_manager : public QWidget
{
    Q_OBJECT

public:
    explicit devicerent_manager(QWidget *parent = nullptr);
    ~devicerent_manager();

private slots:
    void updaterentstatus(int i, int devNo);
private:
    Ui::devicerent_manager *ui;
    void prepare();
    void selectall();
    void showdata(QSqlQuery query);
};

#endif // DEVICERENT_MANAGER_H
