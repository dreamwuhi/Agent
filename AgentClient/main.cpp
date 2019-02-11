#include <QCoreApplication>
#include "client.h"
#include <QSemaphore>

void test()
{
    QSemaphore sem(0);
    //sem.acquire(1);
    int rest = sem.available();
    sem.release(1);
    bool bRet = sem.tryAcquire(1,1000*10);
    int a = 1;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //test();

    Client client(QUrl("ws://127.0.0.1:8010/ws"));
    client.connectToServer();
    QObject::connect(&client, &Client::closed, &a, &QCoreApplication::quit);
    return a.exec();
}
