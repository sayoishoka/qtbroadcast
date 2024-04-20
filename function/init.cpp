#include "init.h"
#include "connect/websocket/websocket.h"
#include "device.h"
#include "file.h"
#include "function/monitor.h"
#include "group.h"

#include "play.h"
#include "program.h"
#include"connect/tcp_ip/tcplink.h"

#include <lib/NAudioServerLib.h>
#include <QDebug>
#include <QTimer>
#include <string>

InitServer::InitServer(QString ipAddress, int port)
{
    char *ip = ipAddress.toUtf8().data();
    qDebug() << na_server_start(ip, port);

    InitModule();
}

InitServer::~InitServer()
{
    na_server_stop();
}

void InitServer::InitModule()
{
    //tcp连接
    TcpLink::getTcpLink();
    //websocket连接
    websocket::getwebsocket();

    // 设备模块
    DeviceModule::getInstance();

    // 分组模块
    GroupModule::getInstance();

    // 文件模块
    FileModule::getInstance();

    // 节目模块
    ProgramModule::getInstance();

    // 播放模块
    PlayModule::getInstance();

    //监听模块
    MonitorModule::getInstance ();

}
