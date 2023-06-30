#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

class TCPClient : public QObject
{
    Q_OBJECT
public:
    TCPClient(QObject* parent = nullptr);
public slots:
    void readyRead();
public:
    void sendToServer();
public:
    QTcpSocket *socket;
    QByteArray data;
    QDataStream dataStream;
};

#endif // TCPCLIENT_H
