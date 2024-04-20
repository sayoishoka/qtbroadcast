#ifndef __INIT_H__
#define __INIT_H__

#include <QObject>

class QTimer;
class GroupModule;
class FileModule;
class ProgramModule;
class PlayModule;

class InitServer : public QObject {
    Q_OBJECT
public:
    explicit InitServer(QString, int);
    ~InitServer();

private:
    void InitModule();
};

#endif  //__INIT_H__
