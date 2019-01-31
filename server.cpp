#include "server.h"
#include <QTcpServer>
#include <QTcpSocket>

Server::Server(quint16 portToListen, QObject *parent) : QObject(parent)
{
    port = portToListen;
}

void Server::StartListening()
{
    qDebug() << "==>\tServer: Trying to set server up.";

    QTcpServer *tcpServer = new QTcpServer();
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(slotHandleConnection())); //!!!
    // connect -- отлов ошибок

    if (!tcpServer->listen(QHostAddress::Any, port))
    {
        qDebug() << "==>\tServer: Fail.";
    }
    else
    {
        qDebug() << "==>\tServer: Success.";

        serverIsUp = true;
        while (serverIsUp)
        {
            tcpServer->waitForNewConnection(1000);
        }
    }

    qDebug() << "==>\tServer: Stopping.";

    tcpServer->close();
    delete tcpServer;
}

void Server::slotHandleConnection()
{
    QTcpServer *ServerSender = reinterpret_cast<QTcpServer *>(QObject::sender());
    QTcpSocket *newOne = ServerSender->nextPendingConnection();

    qDebug() << "==>\tServer: Handle new connection: " << newOne->socketDescriptor();

    emit(sigSendSocket(newOne));

    newOne = nullptr;
    delete newOne;
    ServerSender = nullptr;
    delete ServerSender;
}

void Server::slotStop()
{
    serverIsUp = false;
}
