#pragma once

#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpSocket>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(quint16 portToListen, QObject *parent = nullptr);

signals:
    void sigSendSocket(QTcpSocket *);

public slots:
    void StartListening();
    void slotStop();
    void slotHandleConnection();

private:
    quint16 port = 0;
    bool serverIsUp = false;
};

#endif // SERVER_H
