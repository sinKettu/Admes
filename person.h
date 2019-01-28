#ifndef PERSON_H
#define PERSON_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>

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

private slots:
    //void ReadData();
    //void Connected();
    //void Disconnected();
    //void HandleError();
    void NewServerConnection();
    void NewSocketConnection();
    void ReadDataFromSocket();
    void SocketDisconnected();
    void HandleSocketError(QAbstractSocket::SocketError e);

private:
    QTcpServer *Server = nullptr;
    QMap<qint64, QTcpSocket*> Sockets;
    QStringList InputMessages;
};

#endif // PERSON_H
