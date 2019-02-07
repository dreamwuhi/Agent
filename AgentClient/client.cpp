#include "client.h"
#include <QDebug>
#include <QThread>
#include <QThreadPool>
#include <QMutexLocker>

Client::Client()
{

}

Client::Client(const QUrl &url, QObject *parent):
    m_url(url),
    QObject(parent)
{
    m_pHeartbeatTask = new HeartBeatTask(this);
    connect(m_pHeartbeatTask,&HeartBeatTask::send,this,&Client::onSend);
    QThreadPool::globalInstance()->start(m_pHeartbeatTask);

    qDebug() << "websocket server: " << url;
    connect(&m_webSocket,&QWebSocket::connected,this,&Client::onConnected);
    connect(&m_webSocket,&QWebSocket::disconnected,this,&Client::closed);
    typedef void(QWebSocket:: *errorSingnal)(QAbstractSocket::SocketError);
    connect(&m_webSocket,static_cast<errorSingnal>(&QWebSocket::error),this,&Client::onError);

    connect(&m_webSocket,&QWebSocket::textMessageReceived,this,&Client::onTextMessageReceived);//客户端收到服务端消息
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
    //m_webSocket.close();
    QMutexLocker locker(&m_MutexConnect);
    m_bConnect = false;
    connectToServer();
}

bool Client::isExit()
{
    QMutexLocker locker(&m_MutexExit);
    return m_bExit;
}

bool Client::isConnect()
{
    QMutexLocker locker(&m_MutexConnect);
    return m_bConnect;
}

int Client::sendMessage(const QString& qStrMessage)
{
    m_webSocket.sendTextMessage(qStrMessage);
    return 0;
}

void Client::onConnected()
{
    qDebug() << "client connect to server succ";
    //connect(&m_webSocket,&QWebSocket::textMessageReceived,this,&Client::onTextMessageReceived);//客户端收到服务端消息
    QMutexLocker locker(&m_MutexConnect);
    m_bConnect = true;
}

void Client::onTextMessageReceived(QString qStrMessage)
{
    qDebug()<<"get textmessage from server, text = " << qStrMessage;
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

void Client::onSend(QString qStrMessage)
{
    sendMessage(qStrMessage);
}


HeartBeatTask::HeartBeatTask(Client *pClient):
    m_parent(pClient)
{

}

void HeartBeatTask::run()
{
    if(m_parent == nullptr)
    {
        qDebug()<<"m_parent is null, exit heartbeattask";
        return;
    }
    int iCount = 0;
    for(;;)
    {
        if(true == m_parent->isExit())
        {
            return;
        }
        //轮询等网络建立成功
        if(true == m_parent->isConnect() && iCount == 0)
        {
            //建立成功了，可以发送心跳
            //m_parent->sendMessage("heartbeat-client");
            emit send("heartbeat-client");
            iCount++;
        }
        else if(iCount != 0)
        {
            QThread::msleep(1000);
            if(++iCount == 5)
            {
                iCount = 0;
            }
        }
        else
        {
            QThread::msleep(2000);
        }
    }
}
