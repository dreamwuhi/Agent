#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QByteArray>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class Server : public QObject
{
    Q_OBJECT
public:
    Server();
    explicit Server(int nPort,QObject* parent = nullptr);
    ~Server();
public:
    int init();
Q_SIGNALS:
    void closed();
private Q_SLOTS:
    void onNewConnection();//对端链接
    void processTextMessage(QString qStrMessage);
    void processBinaryMessgae(QByteArray qStrMessage);
    void socketDisconnected();//对端关闭连接
private:
    QWebSocketServer* m_pWebSocketServer;
    QList<QWebSocket*> m_clients;
    int m_nPort;
};

#endif // SERVER_H

