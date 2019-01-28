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
        qDebug() <<  "[ DEBUG ] " << "Unable to start the server: " << Server->errorString();
        return 1;
    }
    else
    {
        qDebug() <<  "[ DEBUG ] " << "Server is ready!";
        while (!Server->waitForNewConnection(1000));
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
    qDebug() << "[ DEBUG ] " << "Connected";
    QTcpSocket *s = static_cast<QTcpSocket *>(QObject::sender());
    //while (!s->waitForDisconnected(1000));

    s = nullptr;
    delete s;
}

void Person::HandleSocketError(QAbstractSocket::SocketError e)
{
    qDebug() << "[ DEBUG ] " << "Error:" << e;

    QTcpSocket *tmp = static_cast<QTcpSocket*>(QObject::sender());
}

void Person::NewServerConnection()
{
    QTcpSocket *client = Server->nextPendingConnection();
    int64_t id = client->socketDescriptor();
    Sockets.insert(id, client);
    connect(Sockets[id], SIGNAL(readyRead()), this, SLOT(ReadDataFromSocket()));
    connect(Sockets[id], SIGNAL(disconnected()), this, SLOT(SocketDisconnected()));
    connect(Sockets[id], SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(HandleSocketError(QAbstractSocket::SocketError)));
    //while (!Sockets[id]->waitForDisconnected(1000));

    client = nullptr;
    delete client;
}

void Person::ReadDataFromSocket()
{
    QTcpSocket *client = static_cast<QTcpSocket *>(QObject::sender());
    qDebug() << "[ DEBUG ] " << "Message from socket " << client->socketDescriptor();
    //printf("%s\n", client->readAll().toStdString().c_str());
    InputMessages.push_back(QString::number(client->socketDescriptor()));
    InputMessages.push_back(client->readAll());

    client = nullptr;
    delete client;
}

void Person::SocketDisconnected()
{
    QTcpSocket *client = static_cast<QTcpSocket *>(QObject::sender());

    qDebug() << "[ DEBUG ] " << "Socket " << client->socketDescriptor() << " disconnected";
    Sockets.remove(client->socketDescriptor());

    client = nullptr;
    delete client;
}

int Person::commandDisconnectAll()
{
    for (int i = 0; i < Sockets.keys().size(); i++)
    {
        Sockets[Sockets.keys()[i]]->disconnect();
        Sockets.remove(Sockets.keys()[i]);
    }

    if (Server != nullptr)
    {
        Server->close();
        delete Server;
    }
}

int Person::commandSend(QString message)
{
    qDebug() << "[ DEBUG ]" << "Sending message";
    Sockets[Sockets.keys()[0]]->write(message.toStdString().c_str());
}

int Person::commandRead()
{
    if (InputMessages.empty())
    {
        printf("No messages!\n");
    }
    else
    {
        while (!InputMessages.empty())
        {
            printf("Message from %s\n", InputMessages[0].toStdString().c_str());
            printf("%s\n", InputMessages[1].toStdString().c_str());
            InputMessages.removeFirst();
            InputMessages.removeFirst();
        }
    }
}
