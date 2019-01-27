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

}
