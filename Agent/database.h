#ifndef DATABASE_H
#define DATABASE_H
#include <QtSql\QSqlDatabase>

class Database
{
public:
    static Database* getInstance();
    int init();
    int setUser(const char* szUsername,const char* szPassword);
private:
    Database(){}
    ~Database()
    {
        if(m_sqlDb.isOpen())
        {
            m_sqlDb.close();
        }
    }
private:
    QSqlDatabase m_sqlDb;
};

#endif // DATABASE_H
