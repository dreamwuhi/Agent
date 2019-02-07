#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtWebSockets/qwebsocket.h>
#include <QAbstractSocket>

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
Q_SIGNALS:
    void closed();
private Q_SLOTS:
    void onConnected();
    void onTextMessageReceived(QString qStrMessage);
    void onError(QAbstractSocket::SocketError error);
private:
    QWebSocket m_webSocket;
    QUrl m_url;
};

#endif // CLIENT_H
