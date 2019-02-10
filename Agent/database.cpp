#include "database.h"
#include <QSqlError>
#include <QDebug>

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
    return 0;
}
