#include "messagehandle.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QDebug>

void messageHandle::paraseMsg(const char* szMessage)
{
    QJsonDocument dom;
    QJsonParseError error;
    dom = QJsonDocument::fromJson(szMessage,&error);
    if(error.error != QJsonParseError::NoError)
    {
        qDebug() << "response message is not json";
        return;
    }

    if(!dom.isObject())
    {
        qDebug() << "response message not contains object";
        return;
    }

    QJsonObject obj = dom.object();
    int msgId = -1;
    if(obj.contains("msgId") && obj.value("msgId").isDouble())
    {
        msgId = obj.value("msgId").toInt();
        notifyMessage(msgId,szMessage);
    }
    else
    {
        qDebug() << "no such msgId in list , msgId = " << msgId;
        return;
    }
}

int messageHandle::addMessage(message* pMsg)
{
    if(pMsg == nullptr)
    {
        return -1;
    }

    QMutexLocker locker(&m_mutex);
    m_listMsg.push_back(pMsg);
    return 0;
}

void messageHandle::deleteMessage(int msgId)
{
    QMutexLocker locker(&m_mutex);
    for(int i=0 ; i<m_listMsg.size();++i)
    {
        if(m_listMsg[i]->m_id == msgId)
        {
            delete m_listMsg[i];
            m_listMsg.removeAt(i);
            return;
        }
    }
}

void messageHandle::notifyMessage(int msgId,QString message)
{
     QMutexLocker locker(&m_mutex);
     for(int i=0 ; i<m_listMsg.size();++i)
     {
         if(m_listMsg[i]->m_id == msgId)
         {
             m_listMsg[i]->m_responseMsg = message;
             m_listMsg[i]->m_sem->release(1);
             return;
         }
     }
}

QString messageHandle::getMessage(int msgId)
{
    QMutexLocker locker(&m_mutex);
    for(int i=0 ; i<m_listMsg.size();++i)
    {
        if(m_listMsg[i]->m_id == msgId)
        {
            return m_listMsg[i]->m_responseMsg;
        }
    }
    return "";
}
