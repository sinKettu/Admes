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
        qDebug() << "\nServer is not started";

        server->close();
        delete server;
        return;
    }

    qDebug() << "\nServer is started";

    connect(server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

void Connection::slotNewConnection()
{
    QTcpSocket *tmp = server->nextPendingConnection();
    qint64 id = tmp->socketDescriptor();
    socketMap.insert(id, tmp);

    qDebug() << "\nNew Conncetion: " << id;

    connect(socketMap[id], SIGNAL(readyRead()), this, SLOT(slotRead()));
    connect(socketMap[id], SIGNAL(disconnected()), this, SLOT(slotDisconnectWarning()));
    connect(socketMap[id], SIGNAL(disconnected()), socketMap[id], SLOT(deleteLater()));

    tmp = nullptr;
    delete tmp;
}

void Connection::slotRead()
{
    QTcpSocket *soc = (QTcpSocket *)QObject::sender();
    qint64 id = soc->socketDescriptor();

    qDebug() << "\nREAD:" << id;

    storage.push_back(soc->readAll());
    storage.push_back(QString::number(id));

    soc = nullptr;
    delete soc;
}

void Connection::slotConnect(QString adr, quint16 port)
{
    qDebug() << "\nConnecting";

    QTcpSocket *soc = new QTcpSocket();
    soc->connectToHost(adr, port);

    connect(soc, SIGNAL(connected()), this, SLOT(slotConnectSuccess()));

    soc = nullptr;
    delete soc;
}

void Connection::slotConnectSuccess()
{
    QTcpSocket *soc = (QTcpSocket *)QObject::sender();
    qint64 id = soc->socketDescriptor();

    qDebug() << "\nConnected: " << id;

    socketMap.insert(id, soc);
    connect(socketMap[id], SIGNAL(readyRead()), this, SLOT(slotRead()));
    connect(socketMap[id], SIGNAL(disconnected()), this, SLOT(slotDisconnectWarning()));
    connect(socketMap[id], SIGNAL(disconnected()), socketMap[id], SLOT(deleteLater()));

    soc = nullptr;
    delete soc;
}

void Connection::slotWrite(qint64 id, QString message)
{
    if (socketMap.contains(id))
        socketMap[id]->write(message.toStdString().c_str());
    else
        qDebug() << "No socket" << id << "exists";
}

void Connection::slotReadIncomming()
{
    if (storage.isEmpty())
    {
        qDebug() << "\nNothing to read";
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

void Connection::slotDisconnect(qint64 id)
{
    socketMap[id]->disconnectFromHost();
    socketMap.remove(id);
}


void Connection::slotDisconnectWarning()
{
    qDebug() << "Disconnected: " << ((QTcpSocket *)QObject::sender())->socketDescriptor();
}
