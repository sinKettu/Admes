#include "generalserver.h"

GeneralServer::GeneralServer(uint16_t port, QObject *parent) : QObject(parent)
{
    tcpServer = new QTcpServer();
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(handleConnection));
    if (!tcpServer->listen(QHostAddress::Any, port))
    {
        qDebug() << "Unable to start the server: " << tcpServer->errorString();
    }
    else
    {
        qDebug() << "Server is ready!";
    }
}

GeneralServer::GeneralServer(QObject *parent) : QObject(parent)
{
    tcpServer = new QTcpServer();
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(handleConnection));
    qDebug() << "Don't forget to start the server!";
}

bool GeneralServer::Start(uint16_t port)
{
    return tcpServer->listen(QHostAddress::Any, port);
}

void GeneralServer::handleConnection()
{
    QTcpSocket *client = tcpServer->nextPendingConnection();
    int64_t id = client->socketDescriptor();
    socketMap[id] = client;
    //socketMap.insert(id, client);     ???
    connect(socketMap[id], SIGNAL(readyRead()), this, SLOT(readClient));
}

void GeneralServer:: readClient()
{
    QTcpSocket *sender = static_cast<QTcpSocket*>(QObject::sender());
    QByteArray message = sender->readAll();
    printf("%s\n", message.toStdString().c_str());
}
