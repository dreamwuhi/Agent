#include "client.h"
#include <QDebug>
#include <QThread>
#include <QThreadPool>
#include <QMutexLocker>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

Client::Client()
{

}

Client::Client(const QUrl &url, QObject *parent):
    m_url(url),
    QObject(parent)
{
    m_pHeartbeatTask = new HeartBeatTask(this);
    connect(m_pHeartbeatTask,&HeartBeatTask::send,this,&Client::onSend);
    connect(m_pHeartbeatTask,&HeartBeatTask::heartBeatOvertime,this,&Client::onReconnect);
    QThreadPool::globalInstance()->start(m_pHeartbeatTask);

    qDebug() << "websocket server: " << url;
    connect(&m_webSocket,&QWebSocket::connected,this,&Client::onConnected);
    //connect(&m_webSocket,&QWebSocket::disconnected,this,&Client::closed);
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
    QMutexLocker locker(&m_MutexConnect);
    m_bConnect = true;//网络建立成功的标记

    RegisterTask* pRegisterTask = new RegisterTask;
    connect(pRegisterTask,&RegisterTask::send,this,&Client::onSend);
    QThreadPool::globalInstance()->start(pRegisterTask);
}

void Client::onTextMessageReceived(QString qStrMessage)
{
    //qDebug()<<"get textmessage from server, text = " << qStrMessage;
    m_pHeartbeatTask->setLastHeart();
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

void Client::onReconnect()
{
    reconnect();
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
            sendHeartBeat();
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

void HeartBeatTask::setLastHeart()
{
    QMutexLocker locker(&m_MutexHeart);
    if(m_vecHeart.empty())
    {
        m_vecHeart.push_back(1);
        return;
    }

    int index = m_vecHeart.size();
    m_vecHeart[index-1] = 1;
}

void HeartBeatTask::sendHeartBeat()
{
    QMutexLocker locker(&m_MutexHeart);
    //检查前2次心跳是否正常
    if(m_vecHeart.empty() || m_vecHeart.size() == 1)
    {
    }
    else
    {
        int index = m_vecHeart.size();
        if(m_vecHeart[index-1] == 0 && m_vecHeart[index-2] == 0)
        {
            //心跳超时
            qDebug()<<"heartbeat overtime";
            emit heartBeatOvertime();
            m_vecHeart.clear();
            return;
        }
    }
    emit send("heartbeat-client");
    m_vecHeart.push_back(0);
}

RegisterTask::RegisterTask()
{

}

void RegisterTask::run()
{
    std::string strUsername = "";
    std::string strPassword = "";
    std::string strConfigmPassword = "";

    std::cout << "please input username: ";
    std::cin >> strUsername;
    std::cout << "please input password: ";
    std::cin >> strPassword;
    std::cout << "please confim password: ";
    std::cin >> strConfigmPassword;

    if(strUsername.empty())
    {
        std::cout << "error: username is empty"<< std::endl;
        emit exitRegister();
        return;
    }

    if(strUsername.length() > 16)
    {
        std::cout << "error: username should be less than 16"<<std::endl;
        emit exitRegister();
        return;
    }

    if(strPassword.empty())
    {
        std::cout<<"error: password is empty" << std::endl;
        emit exitRegister();
        return;
    }

    if(strPassword.length() > 32)
    {
        std::cout << "error: password should be less than 32"<<std::endl;
        emit exitRegister();
        return;
    }

    if(strConfigmPassword.empty())
    {
        std::cout<<"error: confirm password is empty"<<std::endl;
        emit exitRegister();
        return;
    }

    if(strConfigmPassword.length() > 32)
    {
        std::cout<<"error: confirm password should be less than 32"<<std::endl;
        emit exitRegister();
        return;
    }

    if(strConfigmPassword != strPassword)
    {
        std::cout << "error: password is not equal";
        emit exitRegister();
        return;
    }

    emit send(getRegisterMsg(strUsername.c_str(),strPassword.c_str()));
}

QString RegisterTask::getRegisterMsg(const QString& username, const QString& password)
{
    QJsonDocument dom;
    QJsonObject paramsObj;
    paramsObj.insert("username",username);
    paramsObj.insert("password",password);
    QJsonObject json;
    json.insert("type","register");
    json.insert("params",QJsonValue(paramsObj));
    dom.setObject(json);
    return dom.toJson(QJsonDocument::Compact);
}
