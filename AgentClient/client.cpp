#include "client.h"
#include <QDebug>
#include <QThread>

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
    typedef void(QWebSocket:: *errorSingnal)(QAbstractSocket::SocketError);
    connect(&m_webSocket,static_cast<errorSingnal>(&QWebSocket::error),this,&Client::onError);
}

Client::~Client()
{

}

void Client::connectToServer()
{
    m_webSocket.open(m_url);
}

void Client::reconnect()
{
    qDebug()<<"client bengin to reconnect...";
    m_webSocket.abort();
    connectToServer();
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

void Client::onError(QAbstractSocket::SocketError error)
{
    if(error == QAbstractSocket::ConnectionRefusedError)
    {
        qDebug()<<"connect to server fail , error = The connection was refused by the peer (or timed out).";
        QThread::msleep(2000);
        reconnect();
    }
    else if(error == QAbstractSocket::RemoteHostClosedError)
    {
        qDebug()<<"server close, error = The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.";
        QThread::msleep(2000);
        reconnect();
    }
}

