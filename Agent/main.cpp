#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server server(8010);
    server.init();
    QObject::connect(&server,&Server::closed,&a,&QCoreApplication::quit);
    return a.exec();
}
