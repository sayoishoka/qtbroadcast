#ifndef MYTREEWIDGET_H
#define MYTREEWIDGET_H

#include "qdebug.h"
#include <QTreeWidget>
#include <QDropEvent>
class MyTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit MyTreeWidget(QWidget *parent = nullptr) : QTreeWidget(parent) {}
signals:
    void itemMoved(QString deviceName, QString groupName);
protected:
    void dropEvent(QDropEvent *event)
    {
        // 获取被移动的项目
        QTreeWidgetItem *movedItem = currentItem();

        // 获取目标项目
        QTreeWidgetItem *targetItem = itemAt(event->pos());

        // 如果项目被移动到了新的位置
        if (movedItem && targetItem && movedItem != targetItem) {
            // 获取设备和分组的名称
            QString deviceName = movedItem->text(0);
            QString groupName = targetItem->text(0);

            // 发出itemMoved信号
            emit itemMoved(deviceName, groupName);
        }

        QTreeWidget::dropEvent(event);  // 调用基类的dropEvent方法
    }

    void dragMoveEvent(QDragMoveEvent *event)
    {
        // 获取被拖动的项目和目标项目
        QTreeWidgetItem *movedItem = currentItem();
        QTreeWidgetItem *targetItem = itemAt(event->pos());

        // 如果被拖动的项目是设备，且目标项目是分组，则接受这个拖放操作
        if (movedItem && targetItem && movedItem->parent() && !targetItem->parent()) {
            event->accept();
        } else {
            event->ignore();
        }
    }

};

#endif // MYTREEWIDGET_H
