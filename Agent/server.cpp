#include "server.h"
#include <QtWebSockets/qwebsocket.h>
#include <QtWebSockets/qwebsocketserver.h>
#include <QDebug>

Server::Server()
{

}

Server::Server(int nPort, QObject *parent):
    QObject(parent),
    m_pWebSocketServer(new QWebSocketServer("Agent-Server",QWebSocketServer::NonSecureMode,this)),
    m_nPort(nPort)
{

}

Server::~Server()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(),m_clients.end());
}

int Server::init()
{
    if(false == m_pWebSocketServer->listen(QHostAddress::Any,m_nPort))
    {
        qDebug() << QString("agent-server listen fail , port = [%d]").arg(m_nPort);
        return -1;
    }
    connect(m_pWebSocketServer,&QWebSocketServer::newConnection,
            this,&Server::onNewConnection);
    connect(m_pWebSocketServer,&QWebSocketServer::closed,
            this,&Server::closed);
    qDebug() << QString("agent-server listen succ");
    return 0;
}

void Server::onNewConnection()
{
    QWebSocket* pSocket = m_pWebSocketServer->nextPendingConnection();
    connect(pSocket,&QWebSocket::textMessageReceived,this,&Server::processTextMessage);//收到客户端文本消息
    connect(pSocket,&QWebSocket::binaryMessageReceived,this,&Server::processBinaryMessgae);//收到客户端二进制消息
    connect(pSocket,&QWebSocket::disconnected,this,&Server::socketDisconnected);//客户端离线
    m_clients.push_back(pSocket);
}

void Server::processTextMessage(QString qStrMessage)
{
     qDebug() << "agent-server receive message from client, text = " << qStrMessage;
     QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
     if (pClient)
     {
         pClient->sendTextMessage("heartbeat-server");
     }
}

void Server::processBinaryMessgae(QByteArray qStrMessage)
{

}

void Server::socketDisconnected()
{
    QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
    if(pClient)
    {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
        qDebug() << QString("client disconnect");
    }
}
