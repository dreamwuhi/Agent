#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtWebSockets/qwebsocket.h>
#include <QAbstractSocket>
#include <QRunnable>
#include <QMutex>

QT_FORWARD_DECLARE_CLASS(Client)

class HeartBeatTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    HeartBeatTask(Client* pClient = nullptr);
    void run();
Q_SIGNALS:
    void send(QString qStrMessage);
private:
    Client* m_parent = nullptr;
};

class Client : public QObject
{
    Q_OBJECT
public:
    Client();
    explicit Client(const QUrl& url,QObject* parent=nullptr);
    ~Client();
public:
    void connectToServer();
    void reconnect();
    bool isExit();
    bool isConnect();
    int sendMessage(const QString& qStrMessage);
Q_SIGNALS:
    void closed();
private Q_SLOTS:
    void onConnected();
    void onTextMessageReceived(QString qStrMessage);
    void onError(QAbstractSocket::SocketError error);
    void onSend(QString qStrMessage);
private:
    QWebSocket m_webSocket;
    QUrl m_url;
    HeartBeatTask* m_pHeartbeatTask = nullptr;
    bool m_bExit = false;
    bool m_bConnect = false;
    QMutex m_MutexExit;
    QMutex m_MutexConnect;
};

#endif // CLIENT_H
