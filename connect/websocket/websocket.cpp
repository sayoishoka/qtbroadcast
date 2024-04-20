#include "websocket.h"
#include "connect/dispatcher.h"
#include "qjsonarray.h"
#include "qjsonobject.h"

websocket::websocket(QObject *parent)
    : QObject{parent}
{

    m_pWebSocketServer = new QWebSocketServer("Server", QWebSocketServer::NonSecureMode, this);
    m_pClientSocket = nullptr;
    if (m_pWebSocketServer->listen(QHostAddress::Any, 15000)) {
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &websocket::onNewConnection);

    }
}



void websocket::onNewConnection()
{
    qDebug()<<"new connection";
    m_pClientSocket = m_pWebSocketServer->nextPendingConnection();
    connect(m_pClientSocket, &QWebSocket::binaryMessageReceived, this, &websocket::receiveData);
    connect(m_pClientSocket, &QWebSocket::textMessageReceived, this, &websocket::receivetextData);
    connect(m_pClientSocket, &QWebSocket::disconnected, this, &websocket::socketDisconnected);
}

void websocket::receivetextData(const QString &message){
    qDebug()<<"text:"<<message;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());

    // 将QJsonDocument转换为QJsonArray
    QJsonArray jsonArray = doc.array();
    qDebug()<<"size:"<<jsonArray.size();
    qDebug()<<jsonArray;
    // 遍历数组中的每个对象
    for (int i = 0; i < jsonArray.size(); ++i) {
        QJsonObject json = jsonArray[i].toObject();
        QJsonObject writejs = Dispatcher::getDispatcher()->Dispatch(json.value("request").toString(),json);

        QJsonDocument doc(writejs);
        QByteArray data = doc.toJson();

        // 通过WebSocket发送数据
        m_pClientSocket->sendTextMessage(data);

    }

}

void websocket::socketDisconnected()
{
    if (m_pClientSocket) {
        m_pClientSocket->deleteLater();
    }
}

void websocket::receiveData(const QByteArray &arr){
    qDebug()<<"新的数据"<<arr;
    // 将接收到的数据转换为QJsonDocument
    QJsonDocument doc = QJsonDocument::fromJson(arr);
    // 将QJsonDocument转换为QJsonArray
    QJsonArray jsonArray = doc.array();
    qDebug()<<"size:"<<jsonArray.size();
    qDebug()<<jsonArray;
    // 遍历数组中的每个对象
    for (int i = 0; i < jsonArray.size(); ++i) {
        QJsonObject json = jsonArray[i].toObject();
        QJsonObject writejs = Dispatcher::getDispatcher()->Dispatch(json.value("request").toString(),json);

        QJsonDocument doc(writejs);
        QByteArray data = doc.toJson();

        // 通过WebSocket发送数据
        m_pClientSocket->sendTextMessage(data);

    }
}
