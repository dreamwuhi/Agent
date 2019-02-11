#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtWebSockets/qwebsocket.h>
#include <QAbstractSocket>
#include <QRunnable>
#include <QMutex>
#include <QVector>

QT_FORWARD_DECLARE_CLASS(Client)

class HeartBeatTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    HeartBeatTask(Client* pClient = nullptr);
    void run();
    void setLastHeart();
    void sendHeartBeat();
Q_SIGNALS:
    void send(QString qStrMessage);
    void heartBeatOvertime();
private:
    Client* m_parent = nullptr;
    QVector<int> m_vecHeart;
    QMutex m_MutexHeart;
};

class RegisterTask : public QObject, public QRunnable
{
   Q_OBJECT
public:
    RegisterTask();
    void run();
    QString getRegisterMsg(const QString& username, const QString& password, const int& msgId);
Q_SIGNALS:
    void exitRegister();
    void send(QString qStrMessage);
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
    void onReconnect();
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
