#pragma once

#ifndef PERSON_H
#define PERSON_H

#include <QObject>
#include <QTcpSocket>

class Person : public QObject
{
    Q_OBJECT
public:
    explicit Person(QObject *parent = nullptr);
    void StartServer(quint16 port);
    void CreateDialog(QString address, quint16 port);
    void SendMessage(qint64 id, QString message);
    void ReadAllPost();

signals:
    void sigStopServer();
    void sigSendMessage(qint64, const char*);
    void sigCloseDialog(qint64);

public slots:
    void slotReceiveNewConnection(QTcpSocket *);
    void slotReceiveMessage(qint64 id, QString message);
    void slotSocketConnected();

private:
    QStringList messageStorage;
    QTcpSocket *outcommingSocket = nullptr;
};

#endif // PERSON_H
