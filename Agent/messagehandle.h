#ifndef MESSAGEHANDLE_H
#define MESSAGEHANDLE_H
#include <QJsonDocument>
#include <QString>

class messageHandle
{
public:
    messageHandle(){}
    ~messageHandle(){}
public:
    void parseMessage(const char* szMessage);
    QString getResultMessage()
    {
        return m_strResultMessage;
    }
    void registerMsg(QJsonDocument& dom);
private:
    QString m_strResultMessage = "";
};

#endif // MESSAGEHANDLE_H
