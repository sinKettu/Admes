#include "connection.h"

Connection::Connection(QObject *parent) : QObject(parent)
{

}

void Connection::slotConnectionExec()
{
    loop = new QEventLoop();
    loop->exec();
}

void Connection::slotStartServer(quint16 port)
{
    server = new QTcpServer();

    if (!server->listen(QHostAddress::Any, port))
    {
        qDebug() << "Server is not started";
        return;
    }

    qDebug() << "Server is started";

    connect(server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

void Connection::slotNewConnection()
{
    QTcpSocket *tmp = server->nextPendingConnection();
    qint64 id = tmp->socketDescriptor();
    socketMap.insert(id, tmp);

    qDebug() << "New Conncetion: " << id;

    connect(socketMap[id], SIGNAL(readyRead()), this, SLOT(slotRead()));
    connect(socketMap[id], SIGNAL(disconnected()), socketMap[id], SLOT(deleteLater()));
}

void Connection::slotRead()
{
    QTcpSocket *soc = (QTcpSocket *)QObject::sender();
    qint64 id = soc->socketDescriptor();

    qDebug() << "READ:" << id;

    storage.push_back(soc->readAll());
    storage.push_back(QString::number(id));
}

void Connection::slotConnect(QString adr, quint16 port)
{
    qDebug() << "Connecting";

    QTcpSocket *soc = new QTcpSocket();
    soc->connectToHost(adr, port);

    connect(soc, SIGNAL(connected()), this, SLOT(slotConnectSuccess()));
    connect(soc, SIGNAL(disconnected()), soc, SLOT(deleteLater()));
}

void Connection::slotConnectSuccess()
{
    QTcpSocket *soc = (QTcpSocket *)QObject::sender();
    qint64 id = soc->socketDescriptor();

    qDebug() << "Connected: " << id;

    socketMap.insert(id, soc);
    connect(socketMap[id], SIGNAL(readyRead()), this, SLOT(slotRead()));
}

void Connection::slotWrite(qint64 id, QString message)
{
    socketMap[id]->write(message.toStdString().c_str());
}

void Connection::slotReadIncomming()
{
    if (storage.isEmpty())
    {
        qDebug() << "Nothing to read";
    }
    else
    {
        while (!storage.isEmpty())
        {
            printf("[%s]:\n", storage.last().toStdString().c_str());
            storage.pop_back();
            printf("%s\n", storage.last().toStdString().c_str());
            storage.pop_back();
        }
    }
}



