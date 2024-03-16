#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QMenuBar>
#include <QDateTime>
#include <QTimer>


namespace Ui {
class Menu;
}

class Menu : public QWidget
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);
    ~Menu();
    static void UserName(QString name);
private slots:
    void Open_Function(QTreeWidgetItem *item, int column);
    void client_state_yes();
    void client_state_no();
private:
    void timerr();//定时器
    void init();
    static QString usena;
    void signal_slotConnection();
    //菜单栏
    QMenuBar *pMenuBar;
    //菜单项
    QMenu *pMenuTest1;
    //子菜单
    QAction *pActionTest1;
    QAction *pActionTest2;
    QAction *pActionTest3;
    QAction *pActionTest4;

    QList<QAction*> list1;
    Ui::Menu *ui;
};

#endif // MENU_H
