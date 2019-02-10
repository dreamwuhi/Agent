#ifndef REGISTER_H
#define REGISTER_H
#include <QObject>

class cregister : public QObject
{
    Q_OBJECT
public:
    static cregister* getInstance()
    {
        static cregister inst;
        return &inst;
    }
    void run();
Q_SIGNALS:
    void exit();
private:
    cregister(){}
    ~cregister(){}
};

#endif // REGISTER_H
