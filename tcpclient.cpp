#include "tcpclient.h"
#include <thread>
#include <chrono>
#include <random>

TCPClient::TCPClient(QObject *parent):QObject{parent}
{
    socket = new QTcpSocket(this);
    socket->connectToHost(QHostAddress::LocalHost, 8000);
    if(socket->waitForConnected(10000))
    {
        qDebug() << "Connected to server!";
    }else
    {
        qDebug() << "Failed to connect";
    }
    connect(socket, &QTcpSocket::readyRead, this, &TCPClient::readyRead);
    connect(socket, &QTcpSocket::disconnected, this, &TCPClient::deleteLater);
    socket->waitForReadyRead();
}

void TCPClient::readyRead()
{
    data.clear();
    data = socket->readAll();
    qDebug() << data;
}

//void TCPClient::sendToServer()
//{
//    char elems[7] = {'a','b','c','d','f','h','e'};
//    data.clear();
//    QDataStream charStream(&data, QIODevice::WriteOnly);
//    charStream.setByteOrder(QDataStream::BigEndian);
//    for(int i=0; i<7;++i)
//    {
//        charStream << elems[i];
//    }
//    socket->write(data);
//    socket->waitForBytesWritten(5000);
//    socket->waitForReadyRead();

////    std::this_thread::sleep_for(std::chrono::seconds(5));
//    float felems[3] = {1.5, 3.0, 10.0};
//    QDataStream floatStream(&data, QIODevice::WriteOnly);
//    floatStream.setByteOrder(QDataStream::BigEndian);
//    data.clear();
//    for(int i=0; i<3;++i)
//    {
//        floatStream << felems[i];
//    }
//    socket->write(data);
////    socket->waitForBytesWritten();
//    socket->waitForReadyRead();

//    qDebug() << "Operations completed";
//    socket->disconnect();
//}
