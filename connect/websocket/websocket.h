#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <QObject>
#include<QWebSocket>
#include<QWebSocketServer>
#include <QJsonDocument>
class websocket : public QObject
{
    Q_OBJECT
public:
    explicit websocket(QObject *parent = nullptr);
    static websocket* getwebsocket(){
        static websocket instance;
        return &instance;
    };
private:
    QWebSocketServer *m_pWebSocketServer;
    QWebSocket *m_pClientSocket;


    void receivetextData(const QString &message);
private slots:
    void onNewConnection();
    void socketDisconnected();
    void receiveData(const QByteArray &arr);

signals:
};

#endif // WEBSOCKET_H
