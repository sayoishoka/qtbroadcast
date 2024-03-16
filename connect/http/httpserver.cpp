#include"connect/http/httpserver.h"
httpserver *httpserver::servers = new httpserver();
httpserver::httpserver(QObject *parent) : QTcpServer(parent) {
    // 构造函数的实现
}
