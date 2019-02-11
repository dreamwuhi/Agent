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
    RegisterTask(Client* parent=nullptr):m_parent(parent)
    {}
    void run();
    QString getRegisterMsg(const QString& username, const QString& password, const int& msgId);
Q_SIGNALS:
    void exitRegister();
    void send(QString qStrMessage);
private:
    Client* m_parent;
};

class SigninTask : public QObject , public QRunnable
{
    Q_OBJECT
public:
    SigninTask(Client* parent = nullptr) : m_parent(parent)
    {}
    void run();
    QString getSigninMsg(const QString& username, const QString& password, const int& msgId);
Q_SIGNALS:
    void send(QString qStrMessage);
private:
    Client* m_parent;
};

class MainTask : public QObject , public QRunnable
{
  Q_OBJECT
public:
    MainTask(Client* parent=nullptr):m_parent(parent)
    {}
    void run();
Q_SIGNALS:
    void exitMain();
private Q_SLOTS:
    //void onExitRegister();
private:
    Client* m_parent;
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
    void setExit();
    bool isExit();
    bool isConnect();
    int sendMessage(const QString& qStrMessage);
Q_SIGNALS:
    void closed();
public Q_SLOTS:
    void onSend(QString qStrMessage);
private Q_SLOTS:
    void onConnected();
    void onTextMessageReceived(QString qStrMessage);
    void onError(QAbstractSocket::SocketError error);
    void onReconnect();
    void onClose();
private:
    QWebSocket m_webSocket;
    QUrl m_url;
    HeartBeatTask* m_pHeartbeatTask = nullptr;
    bool m_bExit = false;
    bool m_bConnect = false;
    QMutex m_MutexExit;
    QMutex m_MutexConnect;
    MainTask* m_pMainTask = nullptr;
};

#endif // CLIENT_H
