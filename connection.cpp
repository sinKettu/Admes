#ifdef _WIN32
    #include <winsock.h>
    #include <string>
#elif _WIN64
    #include <winsock.h>
    #include <string>
#else
    #include <netinet/in.h>
#endif

#include "connection.h"
#include <QDir>
#include <iostream>

Connection::Connection(QObject *parent) : QObject(parent)
{

}

void Connection::slotConnectionExec()
{
    chat = new Chat();
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

void Connection::slotStartTorServer(quint16 port)
{
    tor = new QProcess();
    tor->setProgram(tor_path);
    QDir tor_conf = QDir::current().absolutePath() + "/tor_config";
    if (!tor_conf.exists() && !QDir::current().mkdir("tor_config"))
    {
        qDebug() << "Can't create " << tor_conf.absolutePath() << "\n";
        delete tor;
        return;
    }

    QStringList args;
    args << "-f" << tor_conf.absolutePath() + "/torrc";
    tor->setArguments(args);
    QFile fout(tor_conf.absolutePath() + "/torrc");
    
    if (!fout.open(QIODevice::WriteOnly))
    {
        qDebug() << "Can't create torrc file\n";
        delete tor;
        return;
    }
    
    QString tmp = "SOCKSPort 9050\n";
    fout.write(tmp.toLatin1());
    qDebug() << "\nSOCKS port is 9050\n";

    tmp = "HiddenServiceDir " + tor_conf.absolutePath() + "/service\n";
    fout.write(tmp.toLatin1());
    qDebug() << "Hidden service directory is " << tor_conf.absolutePath() + "/service\n";

    QString strPort = QString::number(port);
    tmp = "HiddenServicePort " + strPort + " 127.0.0.1:" + strPort + "\n";
    fout.write(tmp.toLatin1());
    qDebug() << "Service will listen to port " << strPort;

    fout.close();

    if (!tor->open(QIODevice::ReadOnly))
    {
        qDebug() << "Can't start tor\n";
        tor->close();
        delete tor;
        return;
    }

    tor->waitForFinished(5500);
    if (tor->state() == QProcess::NotRunning)
    {
        qDebug() << "Tor ran with error\n";
        QStringList torOutput = QString().fromLocal8Bit(tor->readAll()).split('\n');
        for (uint32_t i = 0; i < torOutput.length(); i++)
            qDebug() << torOutput[i] << "\n";
            
        tor->close();
        delete tor;
        return;
    }

    server = new QTcpServer();

    if (!server->listen(QHostAddress::Any, port))
    {
        qDebug() << "\nServer is not started\n";

        server->close();
        tor->close();
        delete tor;
        delete server;
        return;
    }

    QFile fin(tor_conf.absolutePath() + "/service/hostname", this);
    qDebug() << "\nServer is started\n";
    if (!fin.open(QIODevice::ReadOnly))
    {
        qDebug() << "Couldn't read tor service hostname\n";
        server->close();
        tor->close();
        delete tor;
        delete server;
        return;
    }
    else
    {
        tmp = QString().fromLocal8Bit(fin.readLine());
        qDebug() << "Tor service hostname: " << tmp.mid(0, tmp.length() - 1) << "\n\n";
        fin.close();
    }

    connect(server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

void Connection::slotNewConnection()
{
    QTcpSocket *tmp = server->nextPendingConnection();
    qint64 id = tmp->socketDescriptor();
    socketMap.insert(id, tmp);
    chat->AddNewOne(id);

    qDebug() << "\nNew Conncetion: " << id << "\n";

    connect(socketMap[id], SIGNAL(readyRead()), this, SLOT(slotRead()));
    connect(socketMap[id], SIGNAL(disconnected()), this, SLOT(slotDisconnectWarning()));
    connect(socketMap[id], SIGNAL(disconnected()), socketMap[id], SLOT(deleteLater()));

    tmp = nullptr;
}

void Connection::slotRead()
{
    QTcpSocket *soc = (QTcpSocket *)QObject::sender();
    qint64 id = soc->socketDescriptor();

    QString message = QString().fromLocal8Bit(soc->readAll());

    chat->AddToChat(id, "From", message);

    if (message == "HelloFromTor")
        soc->write("MirrorHello");
}

void Connection::slotConnect(QString adr, quint16 port)
{
    QTcpSocket *soc = new QTcpSocket();
    soc->connectToHost(adr, port);

    connect(soc, SIGNAL(connected()), this, SLOT(slotConnectSuccess()));
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

        port = htons(static_cast<unsigned short>(port));
        request = new char[4 + 1 + addr.length() + 2];
        memcpy(request, "\05\01\00\03", 4);
        request[4] = static_cast<char>(addr.length());
        memcpy(request + 5, addr.toStdString().c_str(), static_cast<size_t>(addr.length()));
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
        chat->AddNewOne(id);
        soc->write("HelloFromTor");
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

    qDebug() << "\nConnected: " << id << "\n";

    socketMap.insert(id, soc);
    chat->AddNewOne(id);
    connect(socketMap[id], SIGNAL(readyRead()), this, SLOT(slotRead()));
    connect(socketMap[id], SIGNAL(disconnected()), this, SLOT(slotDisconnectWarning()));
    connect(socketMap[id], SIGNAL(disconnected()), socketMap[id], SLOT(deleteLater()));

    soc = nullptr;
}

void Connection::slotWrite(qint64 id, QString message)
{
    if (socketMap.contains(id))
    {
        socketMap[id]->write(message.toStdString().c_str());
        chat->AddToChat(id, "To", message);
    }
    else
        qDebug() << "No socket" << id << "exists";
}

void Connection::slotDisconnect(qint64 id)
{
    socketMap[id]->disconnectFromHost();
}

void Connection::slotDisconnectWarning()
{
    CheckUp();
}

void Connection::slotOutputDialog(qint64 id)
{
    chat->Output(id);
}

void Connection::slotCloseDialog()
{
    chat->Close();
}

void Connection::CheckUp()
{
    QMap<qint64, QTcpSocket *>::iterator soc;
    QVector<qint64> disconnectedSockets;
    for (soc = socketMap.begin(); soc != socketMap.end(); soc++)
    {
        if (soc.value()->state() == QAbstractSocket::UnconnectedState)
        {
            qDebug() << "Socket " << soc.key() << " disconnected\n";
            disconnectedSockets.push_back(soc.key());
        }
    }
    for (quint32 index = 0; index < disconnectedSockets.length(); index++)
    {
        chat->Remove(disconnectedSockets.at(index));
        socketMap.remove(disconnectedSockets.at(index));
    }
}

#ifdef _WIN32

void Connection::slotSpecifyTorPath(QString path)
{
    tor_path = path;
}

#elif _WIN64

void Connection::slotSpecifyTorPath(QString path)
{
    tor_path = path;
}

#endif
