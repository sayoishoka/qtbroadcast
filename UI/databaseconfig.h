#ifndef DATABASECONFIG_H
#define DATABASECONFIG_H

#include <QWidget>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui {
class DatabaseConfig;
}
QT_END_NAMESPACE

class DatabaseConfig : public QWidget
{
    Q_OBJECT

public:
    QSettings *settings;
    QString ip;
    int port;
    QString database;
    QString username;
    QString userpassword;
signals:
    void Showdbconfig();
private:
    //soundmanagemant *s;
public:
    explicit DatabaseConfig(QWidget *parent = nullptr);
    ~DatabaseConfig();

    static DatabaseConfig* getDatabaseConfig(){
        static DatabaseConfig instance;
        return &instance;
    };
private slots:
    void on_yes_clicked();

    void on_no_clicked();

private:
//    static DatabaseConfig *a;
    Ui::DatabaseConfig *ui;
};

#endif // DATABASECONFIG_H
