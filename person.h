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
    explicit Person(QObject *parent = nullptr);
    int commandOpen(quint16 port);
    int commandConnect(QString address, quint16 port);
    int commandDisconnectAll();
    int commandSend(QString message);
    int commandRead();

signals:
    void InitWaitingForReadData();
    void StopServer();

private slots:
    void NewServerConnection();
    void NewSocketConnection();
    void ReadDataFromSocket();
    void SocketDisconnected();
    void HandleSocketError(QAbstractSocket::SocketError e);
    void WaitForReadDataFromSocket();
    void WaitForConnection();
    void ServerListening();

private:
    QTcpServer *Server = nullptr;
    QMap<qint64, QTcpSocket*> Sockets;
    QStringList InputMessages;
    QList<QThread *> threadPool;
    QList<QTcpSocket *> socketStack;
    bool socketStackInUse = false;
};

#endif // PERSON_H
