#include "person.h"

Person::Person(QObject *parent) : QObject(parent)
{
    Server = new QTcpServer();
    connect(Server, SIGNAL(newConnection()), this, SLOT(NewServerConnection()));
}

int Person::commandOpen(quint16 port)
{
    if (Server == nullptr)
        return 2;

    if (!Server->listen(QHostAddress::Any, port))
    {
        qDebug() << "Unable to start the server: " << Server->errorString();
        return 1;
    }
    else
    {
        qDebug() << "Server is ready!";
        while (!Server->waitForNewConnection(1000));
        //Server->waitForNewConnection(); //! !!
        return 0;
    }
}

int Person::commandConnect(QString address, quint16 port)
{
    QTcpSocket *tmpSocket = new QTcpSocket();
    tmpSocket->connectToHost(address, port);

    connect(tmpSocket, SIGNAL(connected()), this, SLOT(NewSocketConnection()));
    connect(tmpSocket, SIGNAL(readyRead()), this, SLOT(ReadDataFromSocket()));
    connect(tmpSocket, SIGNAL(disconnected()), this, SLOT(SocketDisconnected()));
    connect(tmpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(HandleSocketError(QAbstractSocket::SocketError)));

    Sockets[tmpSocket->socketDescriptor()] = tmpSocket;
    while (!tmpSocket->waitForConnected(1000));

    tmpSocket = nullptr;
    delete tmpSocket;


    return 0;
}

void Person::NewSocketConnection()
{
    qDebug() << "Connected";
    QTcpSocket *s = static_cast<QTcpSocket *>(QObject::sender());
    s->write("Hello\n", 6);
    //s->disconnectFromHost();
    qDebug() << "ok";
    while (!s->waitForReadyRead(1000));

    s = nullptr;
    delete s;
}

void Person::HandleSocketError(QAbstractSocket::SocketError e)
{
    qDebug() << "Error" << e;

    QTcpSocket *tmp = static_cast<QTcpSocket*>(QObject::sender());
   // qint64 descriptor = tmp->socketDescriptor();
  //  Sockets.remove(descriptor);
}

void Person::NewServerConnection()
{
    QTcpSocket *client = Server->nextPendingConnection();
    int64_t id = client->socketDescriptor();
    Sockets.insert(id, client);
    connect(Sockets[id], SIGNAL(readyRead()), this, SLOT(ReadDataFromSocket()));
    connect(Sockets[id], SIGNAL(disconnected()), this, SLOT(SocketDisconnected()));
    connect(Sockets[id], SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(HandleSocketError(QAbstractSocket::SocketError)));
    while (!Sockets[id]->waitForReadyRead(1000));

    client = nullptr;
    delete client;
}

void Person::ReadDataFromSocket()
{
    QTcpSocket *client = static_cast<QTcpSocket *>(QObject::sender());
    qDebug() << "Message from socket " << client->socketDescriptor();
    printf("%s\n", client->readAll().toStdString().c_str());

    client = nullptr;
    delete client;
}

void Person::SocketDisconnected()
{
    QTcpSocket *client = static_cast<QTcpSocket *>(QObject::sender());
    qDebug() << "Socket " << client->socketDescriptor() << " disconnected";
    Sockets.remove(client->socketDescriptor());

    client = nullptr;
    delete client;
}
