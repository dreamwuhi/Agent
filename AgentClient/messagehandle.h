#ifndef MESSAGEHANDLE_H
#define MESSAGEHANDLE_H
#include <QString>
#include <QList>
#include <QSemaphore>
#include <QMutex>
#include <QMutexLocker>

class message
{
public:
    message(int id, QSemaphore* pSem) : m_id(id), m_sem(pSem)
    {}
    ~message()
    {
        if(m_sem)
        {
            delete m_sem;
        }
    }
    int m_id;
    QString m_responseMsg;
    QSemaphore* m_sem;
};

class messageHandle
{
public:
    static messageHandle* getInstance()
    {
        static messageHandle inst;
        return &inst;
    }

    int getNextMessageId()
    {
        QMutexLocker locker(&m_idMutex);
        return g_id++;
    }

    void paraseMsg(const char* szMessage);
private:
    messageHandle(){}
    ~messageHandle(){}
public:
    int addMessage(message* pMsg);
    void deleteMessage(int msgId);
    void notifyMessage(int msgId,QString message);
    QString getMessage(int msgId);
private:
    QList<message*> m_listMsg;
    QMutex m_mutex;
    QMutex m_idMutex;
    int g_id;
};

#endif // MESSAGEHANDLE_H
