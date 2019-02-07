#include "client.h"
#include <QDebug>

Client::Client()
{

}

Client::Client(const QUrl &url, QObject *parent):
    m_url(url),
    QObject(parent)
{
    qDebug() << "websocket server: " << url;
    connect(&m_webSocket,&QWebSocket::connected,this,&Client::onConnected);
    connect(&m_webSocket,&QWebSocket::disconnected,this,&Client::closed);
    m_webSocket.open(url);
}

Client::~Client()
{

}

void Client::onConnected()
{
    qDebug() << "client connect to server succ";
    connect(&m_webSocket,&QWebSocket::textMessageReceived,this,&Client::onTextMessageReceived);
    m_webSocket.sendTextMessage("hello word!!");
}

void Client::onTextMessageReceived(QString qStrMessage)
{

}
