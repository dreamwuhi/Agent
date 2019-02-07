#include <QCoreApplication>
#include "client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Client client(QUrl("ws://127.0.0.1:8010/ws"));
    client.connectToServer();
    return a.exec();
}
