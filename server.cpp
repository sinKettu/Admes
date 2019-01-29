#include "server.h"
#include "person.h"

Server::Server(QObject *parent) : QObject(parent)
{
    ConnectionReceiver = new QTcpServer();
    connect(ConnectionReceiver, SIGNAL(newConnection()), this, SLOT(HandleConnection()));
    connect(this, SIGNAL(NewSocketForParrent(qint64)), this->parent(), SLOT(NewSocketFromServer(qint64)));
}

void Server::Start(quint16 port)
{
    if (!ConnectionReceiver->listen(QHostAddress::Any, port))
    {
        qDebug() << "[ DEBUG ] Server was not started.";
    }
    else
    {
        while (continueListening)    // Заменить true на что-то поудачнее
        {
            qDebug() << "[ DEBUG ] Server is listening for connections.";
            while (!ConnectionReceiver->waitForNewConnection(1000));
            qDebug() << "[ DEBUG ] New Connection";
        }
    }
}

void Server::HandleConnection()
{
    QTcpSocket *socket = ConnectionReceiver->nextPendingConnection();
    qint64 id = socket->socketDescriptor();

    qDebug() << "[ DEBUG ] socket " << id << " was handled.";

    ((Person *)this->parent())->Sockets.insert(id, socket);

    socket = nullptr;
    delete socket;

    emit(NewSocketForParrent(id));
}

void Server::ShutDownSlot()
{
    qDebug() << "[ DEBUG ] Server received signal to stop.";

    continueListening = false;
    ConnectionReceiver->close();
}
