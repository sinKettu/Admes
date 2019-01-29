#ifndef PERSON_H
#define PERSON_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QThread>

class Person : public QObject
{
    Q_OBJECT
public:
    QMap<qint64, QTcpSocket *> Sockets;
    explicit Person(QObject *parent = nullptr);
    void StartServer(quint16 port);
    void StopServer();
    //void CloseConnection(qint64 id);

signals:
    void StopServerThreadSignal();

public slots:
    void NewSocketFromServer(qint64 id);

private slots:
    void StartServerSlot();

private:
    quint16 portForServer = 0;
};

#endif // PERSON_H
