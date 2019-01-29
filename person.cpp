#include "person.h"

Person::Person(QObject *parent) : QObject(parent)
{
    Server = new QTcpServer();
    connect(Server, SIGNAL(newConnection()), this, SLOT(NewServerConnection()));
    connect(this, SIGNAL(InitWaitingForReadData()), this, SLOT(WaitForReadDataFromSocket()));
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
        QThread *thr = new QThread(this);
        threadPool.push_back(thr);
        connect(this, SIGNAL(StopServer()), threadPool.last(), SLOT(quit()));
        connect(threadPool.last(), SIGNAL(started()), this, SLOT(ServerListening()));
        threadPool.last()->start();

        thr = nullptr;
        delete thr;
        return 0;
    }
}

void Person::ServerListening()
{
    qDebug() << "[ DEBUG ] Server is listening.";
    while (!Server->waitForNewConnection(1000));
}

int Person::commandConnect(QString address, quint16 port)
{
    QTcpSocket *tmpSocket = new QTcpSocket();
    while (socketStackInUse);
    socketStackInUse = true;
    socketStack.push_back(tmpSocket);

    connect(socketStack.last(), SIGNAL(connected()), this, SLOT(NewSocketConnection()));
    connect(socketStack.last(), SIGNAL(readyRead()), this, SLOT(ReadDataFromSocket()));
    connect(socketStack.last(), SIGNAL(disconnected()), this, SLOT(SocketDisconnected()));
    connect(socketStack.last(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(HandleSocketError(QAbstractSocket::SocketError)));

    tmpSocket = nullptr;
    delete tmpSocket;

    socketStack.last()->connectToHost(address, port);
    qDebug() << "[ DEBUG ] Try to connect.";
    while (!socketStack.last()->waitForConnected(100));

    socketStackInUse = false;
    return 0;
}

void Person::WaitForConnection()
{
    //QTcpSocket
}

void Person::NewSocketConnection()
{
    qDebug() << "[ DEBUG ] " << "Connected";

    QTcpSocket *s = static_cast<QTcpSocket *>(QObject::sender());
    while (socketStackInUse);
    socketStackInUse = true;
    socketStack.push_back(s);
    s = nullptr;
    delete s;

    QThread *thr = new QThread(this);
    threadPool.push_back(thr);
    thr = nullptr;
    delete thr;

    connect(socketStack.last(), SIGNAL(disconnected()), threadPool.last(), SLOT(quit()));
    connect(threadPool.last(), SIGNAL(started()), this, SLOT(WaitForReadDataFromSocket()));
    socketStackInUse = false;
}

void Person::HandleSocketError(QAbstractSocket::SocketError e)
{
    qDebug() << "[ DEBUG ] " << "Error:" << e;
}

void Person::NewServerConnection()
{
    while (socketStackInUse);
    socketStackInUse = true;
    socketStack.push_back(Server->nextPendingConnection());
    QThread *newThread = new QThread(this);
    threadPool.push_back(newThread);
    newThread = nullptr;
    delete newThread;
    connect(threadPool.last(), SIGNAL(started()), this, SLOT(WaitForReadDataFromSocket()));
    connect(socketStack.last(), SIGNAL(disconnected()), threadPool.last(), SLOT(quit()));
    threadPool.last()->start();
    socketStackInUse = false;
    ServerListening();
}

void Person::WaitForReadDataFromSocket()
{
    while (socketStackInUse);
    socketStackInUse = true;
    QTcpSocket *socket = socketStack.last();
    qint64 id = socket->socketDescriptor();
    socketStack.last() = nullptr;
    socketStack.pop_back();
    socketStackInUse = false;

    if (!Sockets.contains(id))
    {
        connect(socket, SIGNAL(readyRead()), this, SLOT(ReadDataFromSocket()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(SocketDisconnected()));
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(HandleSocketError(QAbstractSocket::SocketError)));
        Sockets.insert(id, socket);
    }

    socket = nullptr;
    delete socket;

    while (!Sockets[id]->waitForReadyRead(100));
}

void Person::ReadDataFromSocket()
{
    QTcpSocket *client = static_cast<QTcpSocket *>(QObject::sender());
    qDebug() << "[ DEBUG ] " << "Message from socket " << client->socketDescriptor();

    InputMessages.push_back(QString::number(client->socketDescriptor()));
    InputMessages.push_back(client->readAll());

    while (socketStackInUse);
    socketStackInUse = true;
    socketStack.push_back(client);
    socketStackInUse = false;
    client = nullptr;
    delete client;

    emit(InitWaitingForReadData());
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
