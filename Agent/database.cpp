#include "database.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDir>

Database *Database::getInstance()
{
    static Database inst;
    return &inst;
}

int Database::init()
{
    if(QSqlDatabase::contains("server_sqlite"))
    {
        m_sqlDb = QSqlDatabase::database("server_sqlite");
    }
    else
    {
        m_sqlDb = QSqlDatabase::addDatabase("QSQLITE","server_sqlite");
        m_sqlDb.setDatabaseName("server.db");
    }

    if(!m_sqlDb.open())
    {
        qDebug()<<"open database fail, error = " << m_sqlDb.lastError();
        return -1;
    }
    qDebug()<<"open database succ";

//    QFile dbFile("server.db");
//    if(dbFile.exists())
//    {
//        qDebug() << "server.db is existed";
//        return 0;
//    }

    QFile file("./debug/core.sql");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"open file fail , file path core.sql , error = "<<file.errorString();
        return -1;
    }

    QTextStream data(&file);
    if(!m_sqlDb.transaction())
    {
        qDebug()<<"db transaction fail, error = " <<m_sqlDb.lastError();
        return -1;
    }

    QString dataAll = data.readAll();
    file.close();
    QStringList dataList = dataAll.split(";");
    for(int i=0; i<dataList.size(); ++i)
    {
        QString sqlText = dataList[i];
        sqlText = sqlText.trimmed();
        if(sqlText.isEmpty())
        {
            continue;
        }
        QSqlQuery query(m_sqlDb);
        if(false == query.exec(sqlText))
        {
            qDebug()<<"db exec fail, error = "<<query.lastError();
            m_sqlDb.rollback();
            return -1;
        }
    }

    if(false == m_sqlDb.commit())
    {
        qDebug()<<"db commit fail, error = "<<m_sqlDb.lastError();
        return -1;
    }

    return 0;
}

int Database::setUser(const char *szUsername, const char *szPassword)
{
    if(szUsername == nullptr)
    {
        qDebug() << "username is empty";
        return -1;
    }

    if(szPassword == nullptr)
    {
        qDebug() << "password is empty";
        return -1;
    }

    QSqlQuery query(m_sqlDb);
    query.prepare("insert into user (username,password) VALUES(?,?)");
    query.bindValue(0,szUsername);
    query.bindValue(1,szPassword);
    if(false == query.exec())
    {
        qDebug() << "db exec fail , error = " << query.lastError();
        return -1;
    }
    return 0;
}
