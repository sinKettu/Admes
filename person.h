#ifndef PERSON_H
#define PERSON_H

#include <QObject>
#include <QtNetwork/QTcpSocket>

class Person : public QObject
{
    Q_OBJECT
public:
    explicit Person(QObject *parent = nullptr);
    int commandConnect(QString address, qint16 port);
    int commandDisconnect();
    int commandSend();

signals:

private slots:
    void ReadData();
    void Connected();
    void Disconnected();
    void HandleError();

private:
    QTcpSocket *socket = nullptr;
};

#endif // PERSON_H
