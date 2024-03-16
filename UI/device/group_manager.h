#ifndef GROUP_MANAGER_H
#define GROUP_MANAGER_H

#include "UI/device/mytreewidget.h"
#include "qtreewidget.h"
#include <QWidget>
#include<QDropEvent>

namespace Ui {
class group_manager;
}

class group_manager : public QWidget
{
    Q_OBJECT

public:
    explicit group_manager(QWidget *parent = nullptr);
    ~group_manager();


private slots:
    void onItemMoved(QString deviceName, QString groupName);
    void onItemClicked(QTreeWidgetItem *item, int column);
private:
    Ui::group_manager *ui;
    void updateTree();
    MyTreeWidget *myTreeWidget;
};

#endif // GROUP_MANAGER_H
