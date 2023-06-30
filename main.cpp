#include <QTcpSocket>
#include <QHostAddress>
#include <QCoreApplication>
//#include <QTextStream>
#include <QDebug>
#include <iostream>
#include <random>
#include <vector>
#include <chrono>
//#include <QTextCodec>
#include <windows.h>
//#include "tcpclient.h"

void fillMatrixRandom(std::vector<std::vector<qint32>>& matrix, int size)
{
    auto time = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(time);

    for (int i = 0; i < size; ++i)
    {
        std::vector<int> row;
        for (int j = 0; j < size; ++j)
        {
            row.push_back(gen()%100 + 1);
        }
        matrix.push_back(row);
    }
}

int main(int argc, char *argv[])
{
    SetConsoleOutputCP(1251);
    QCoreApplication app(argc, argv);
    QTcpSocket* socket = new QTcpSocket(&app);
    socket->connectToHost(QHostAddress::LocalHost, 8000);
    if(socket->waitForConnected(10000))
    {
        qDebug() << "Connected to server!";
    }else
    {
        qDebug() << "Failed to connect";
    }
//    TCPClient client;
//    client.sendToServer();

    // Зчитування розмірності матриці
    QTextStream consoleReader(stdin);
    qDebug() << "Введіть розмірність матриці (N x N):";
    qint32 size = consoleReader.readLine().toInt();

    // Зчитування кількості потоків для обчислення
    qDebug() << "Введіть кількість потоків для обчислення: ";
    qint32 k = consoleReader.readLine().toInt();

    // Створення та заповнення матриці випадковими числами
    std::vector<std::vector<qint32>> matrix;
    fillMatrixRandom(matrix, size);

    // Виведення матриці на екран
    qDebug() << "Матриця:";
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }

    qint32 way;
    qDebug() << "Виберіть вид паралелізації:1 - через future, інше - openMP";
    way = consoleReader.readLine().toInt();

    while(true)
    {
        qDebug() << "Уведіть команду(start/result/exit):";
        QString command = consoleReader.readLine();
        if(command == "start")
        {
            QByteArray data;
            QDataStream stream(&data, QIODevice::WriteOnly);
            stream << command;
            socket->write(data);
            socket->waitForBytesWritten(500);

            data.clear();
            QDataStream sizeStream(&data, QIODevice::WriteOnly);
            sizeStream.setByteOrder(QDataStream::BigEndian);
            sizeStream << size << k;
            socket->write(data);
            socket->waitForBytesWritten();

            data.clear();
            QDataStream matrixStream(&data, QIODevice::WriteOnly);
            matrixStream.setByteOrder(QDataStream::BigEndian);
            for(int i=0; i<matrix.size();++i)
            {
                for(int j=0; j<matrix.size();++j)
                {
                    matrixStream << matrix[i][j];
                }
            }
            socket->write(data);
            socket->waitForBytesWritten();

            data.clear();
            QDataStream wayStream(&data, QIODevice::WriteOnly);
            wayStream.setByteOrder(QDataStream::BigEndian);
            wayStream << way;
            socket->write(data);
            socket->waitForBytesWritten();

        }
        else if(command == "result")
        {
            QByteArray data;
            QDataStream stream(&data, QIODevice::WriteOnly);
            stream << command;
            socket->write(data);
            socket->waitForBytesWritten(500);
            socket->waitForReadyRead();

            data.clear();
            QDataStream inStream(socket);
            inStream.setVersion(QDataStream::Qt_5_15);
            inStream.setByteOrder(QDataStream::BigEndian);
            QString resultText;
            inStream >> resultText;
            if(resultText == "Calculation in progress")
            {
                qDebug() << "Still working";
                continue;
            }
            socket->waitForReadyRead();
            for(int i=0; i<size;++i)
            {
                for(int j=0; j<size;++j)
                {
                    inStream >> matrix[i][j];
                }
            }
            qDebug() << "Отримано результат";
            for(auto row: matrix)
            {
                for(auto it : row)
                {
                    std::cout << it << " ";
                }
                std::cout << std::endl;
            }
        }
        else if(command == "exit")
        {
            socket->write("exit");
            socket->waitForBytesWritten(500);
            socket->disconnect();
            socket->close();
            qDebug() << "З'єднання закрито";
            break;
        }
    }
    return app.exec();
}
