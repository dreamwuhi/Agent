#include "messagehandle.h"
#include <QDebug>
#include <QJsonObject>
#include "database.h"

void messageHandle::parseMessage(const char *szMessage)
{
    if(szMessage == nullptr)
    {
        qDebug() << "input message is empty";
        return;
    }

    QJsonDocument dom;
    QJsonParseError error;
    dom = QJsonDocument::fromJson(szMessage,&error);
    if(error.error != QJsonParseError::NoError)
    {
        qDebug() << "parse json fail, error = " << error.errorString();
        return;
    }

    if(!dom.isObject())
    {
        qDebug() << "json not contains an object";
        return;
    }

    if(!dom.object().contains("type"))
    {
        qDebug() << "json not contains type";
        return;
    }

    if(!dom.object().contains("msgId"))
    {
        qDebug() << "json not contains msgId";
        return;
    }

    QJsonValue value = dom.object().value("type");
    QString msgType = value.toString();
    if(msgType == "register")
    {
        registerMsg(dom);
    }
    else if(msgType == "signin")
    {
        signinMsg(dom);
    }
}

void messageHandle::registerMsg(QJsonDocument& dom)
{
    QJsonObject domObject = dom.object();
    if(!domObject.contains("params"))
    {
        qDebug() << "json not contains param";
        return;
    }

    if(!domObject.value("params").isObject())
    {
        qDebug() << "params is not an object";
        return;
    }

    QJsonObject paramsObj = domObject.value("params").toObject();
    if(!paramsObj.contains("username"))
    {
        qDebug() << "no username key";
        return;
    }

    if(!paramsObj.contains("password"))
    {
        qDebug() << "no password key";
        return;
    }

    if(!paramsObj.value("username").isString())
    {
        qDebug() << "username key is not string";
        return;
    }

    if(!paramsObj.value("password").isString())
    {
        qDebug() << "password key is not string";
        return;
    }

    QString username = paramsObj.value("username").toString();
    QString password = paramsObj.value("password").toString();

    QJsonObject resultParamsObj;
    QJsonObject json;
    if(0 != Database::getInstance()->setUser(username.toStdString().c_str(),password.toStdString().c_str()))
    {
        qDebug() << "register fail, username = " << username << " , password = " << password;
        resultParamsObj.insert("code","100");
        resultParamsObj.insert("msg","register fail");
    }
    else
    {
        resultParamsObj.insert("code","0");
        resultParamsObj.insert("msg","");
    }
    json.insert("type","register");
    json.insert("msgId",dom.object().value("msgId"));
    json.insert("params",QJsonValue(resultParamsObj));

    QJsonDocument resultDom;
    resultDom.setObject(json);
    m_strResultMessage = resultDom.toJson(QJsonDocument::Compact);
}

void messageHandle::signinMsg(QJsonDocument &dom)
{
    QJsonObject domObject = dom.object();
    if(!domObject.contains("params"))
    {
        qDebug() << "json not contains param";
        return;
    }

    if(!domObject.value("params").isObject())
    {
        qDebug() << "params is not an object";
        return;
    }

    QJsonObject paramsObj = domObject.value("params").toObject();
    if(!paramsObj.contains("username"))
    {
        qDebug() << "no username key";
        return;
    }

    if(!paramsObj.contains("password"))
    {
        qDebug() << "no password key";
        return;
    }

    if(!paramsObj.value("username").isString())
    {
        qDebug() << "username key is not string";
        return;
    }

    if(!paramsObj.value("password").isString())
    {
        qDebug() << "password key is not string";
        return;
    }

    QString username = paramsObj.value("username").toString();
    QString password = paramsObj.value("password").toString();

    QJsonObject resultParamsObj;
    QJsonObject json;
    if(0 != Database::getInstance()->getUser(username.toStdString().c_str(),password.toStdString().c_str()))
    {
        qDebug() << "signin fail, username = " << username << " , password = " << password;
        resultParamsObj.insert("code","101");
        resultParamsObj.insert("msg","signin fail");
    }
    else
    {
        resultParamsObj.insert("code","0");
        resultParamsObj.insert("msg","");
    }
    json.insert("type","signin");
    json.insert("msgId",dom.object().value("msgId"));
    json.insert("params",QJsonValue(resultParamsObj));

    QJsonDocument resultDom;
    resultDom.setObject(json);
    m_strResultMessage = resultDom.toJson(QJsonDocument::Compact);
}
