#include "connection.h"
#include <netinet/in.h>

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
}

void Connection::slotRead()
{
    QTcpSocket *soc = (QTcpSocket *)QObject::sender();
    qint64 id = soc->socketDescriptor();

    qDebug() << "\nREAD:" << id;

    storage.push_back(soc->readAll());
    storage.push_back(QString::number(id));

    soc = nullptr;
}

void Connection::slotConnect(QString adr, quint16 port)
{
    qDebug() << "\nConnecting";

    QTcpSocket *soc = new QTcpSocket();
    soc->connectToHost(adr, port);

    connect(soc, SIGNAL(connected()), this, SLOT(slotConnectSuccess()));

    soc = nullptr;
}

void Connection::slotConnectSOCKS5(QString addr, quint16 port)
{
    qDebug() << "\nConnecting to SOCKS server\n";
    QTcpSocket *soc = new QTcpSocket();
    soc->connectToHost("127.0.0.1", (quint16)9050);

    if (soc->waitForConnected(5000))
    {
        char *request = const_cast<char*>("\05\01\00");
        soc->write(request, 3);
        char response[10];
        soc->read(response, 2);

        if (response[1] != 0x00)
        {
            qDebug() << "SOCKS5 error authentificating\n";
            soc->close();
            delete soc;
            return;
        }

        port = htons(port);
        request = new char[4 + 1 + addr.length() + 2];
        memcpy(request, "\05\01\00\03", 4);
        request[4] = static_cast<unsigned char>(addr.length());
        memcpy(request + 5, addr.toStdString().c_str(), addr.length());
        memcpy(request + 5 + addr.length(), &port, 2);

        soc->write(request, 4 + 1+ addr.length() + 2);
        delete[] request;

        soc->read(response, 10);
        if (response[1] != 0x00)
        {
            qDebug() << "SOCKS5 error: " << (int)response[1] << "\n";
            soc->close();
            delete soc;
            return;
        }

        qint64 id = soc->socketDescriptor();
        qDebug() << "SOCKS5 connected: " << id << "\n";

        socketMap.insert(id, soc);
        connect(socketMap[id], SIGNAL(readyRead()), this, SLOT(slotRead()));
        connect(socketMap[id], SIGNAL(disconnected()), this, SLOT(slotDisconnectWarning()));
        connect(socketMap[id], SIGNAL(disconnected()), socketMap[id], SLOT(deleteLater()));
    }
    else
    {
        qDebug() << "SOCKS5 does not answer\n";
        soc->close();
        delete soc;
   }
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
