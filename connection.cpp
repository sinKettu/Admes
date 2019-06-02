#if defined(_WIN32) || defined(_WIN64)
    #include <winsock.h>
#else
    #include <netinet/in.h>
#endif

#include "connection.h"
#include "common.h"

QString connectReq = "admesconnectrequest";
QString connectResp = "admesconnectresponse";

Connection::Connection(QObject *parent) : QObject(parent) {}

void Connection::slotConnectionExec()
{
    chat = new Chat();
}

Connection::~Connection()
{
    if (server != nullptr)
    {
        server->close();
        delete server;
    }
    if (tor != nullptr)
    {
        tor->close();
    }
    QMap<qint64, QTcpSocket*>::iterator iter;
    for (iter = WaitingForConfirmation.begin(); iter != WaitingForConfirmation.end(); iter++)
        iter.value()->disconnect();
    for (iter = socketMap.begin(); iter != socketMap.end(); iter++)
        iter.value()->disconnect();
    WaitingForConfirmation.clear();
    socketMap.clear();
    delete chat;
}

void Connection::slotStopAll()
{
    this->~Connection();
    emit sigTerminateThread();
}

void Connection::slotStartServer()
{
    server = new QTcpServer();

    if (!server->listen(QHostAddress::Any, server_port))
    {
        std::cout << prefix << "Server is not started\n";

        server->close();
        delete server;
        return;
    }

    std::cout << prefix << "Server is started at port " << server_port << "\n";

    connect(server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

void Connection::slotRunTor()
{
    tor = new QProcess();
    connect(tor, SIGNAL(finished(int, QProcess::ExitStatus)), tor, SLOT(deleteLater()));
    tor->setProgram(tor_path);
    QDir tor_conf = QDir::current().absolutePath() + "/tor_config";
    if (!tor_conf.exists() && !QDir::current().mkdir("tor_config"))
    {
        std::cout << prefix << "Can't create " << tor_conf.absolutePath().toStdString() << "\n";
        return;
    }

    QStringList args;
    args << "-f" << tor_conf.absolutePath() + "/torrc";
    tor->setArguments(args);
    QFile fout(tor_conf.absolutePath() + "/torrc");
    
    if (!fout.open(QIODevice::WriteOnly))
    {
        std::cout << prefix << "Can't create torrc file\n";
        return;
    }
    
    QString tmp = "SOCKSPort " + QString::number(socks5_port) + "\n";
    fout.write(tmp.toLatin1());

    tmp = "HiddenServiceDir " + tor_conf.absolutePath() + "/service\n";
    fout.write(tmp.toLatin1());

    QString strPort = QString::number(server_port);
    tmp = "HiddenServicePort " + strPort + " 127.0.0.1:" + strPort + "\n";
    fout.write(tmp.toLatin1());

    fout.close();
    if (!tor->open(QIODevice::ReadOnly))
    {
        std::cout << prefix << "Can't start tor\n";
        tor->close();
        return;
    }

    tor->waitForFinished(3000);
    if (tor->state() == QProcess::NotRunning)
    {
        std::cout << prefix << "Tor ran with error\n";
        QStringList torOutput = QString().fromLocal8Bit(tor->readAll()).split('\n');
        for (uint32_t i = 0; i < torOutput.length(); i++)
            std::cout << "[TOR] " << torOutput[i].toStdString() << "\n";
            
        tor->close();
        return;
    }

    QFile fin(tor_conf.absolutePath() + "/service/hostname", this);
    if (!fin.open(QIODevice::ReadOnly))
    {
        std::cout << prefix << "Couldn't read tor service hostname\n";
        tor->close();
        return;
    }
    else
    {
        tmp = QString().fromLocal8Bit(fin.readLine());
        fin.close();
        connect(tor, SIGNAL(readyReadStandardOutput()), this, SLOT(slotReadTorOutput()));
        connect(tor, SIGNAL(readyReadStandardError()), this, SLOT(slotReadTorOutput()));
        std::cout << prefix << "Service will listen to port " << strPort.toStdString() << "\n";
        std::cout << prefix << "Hidden service directory is " << tor_conf.absolutePath().toStdString() + "/service\n";
        std::cout << prefix << "SOCKS5 port is " + QString::number(socks5_port).toStdString() + "\n";
        std::cout << prefix << "Tor service hostname: " << tmp.mid(0, tmp.length() - 1).toStdString() << "\n";
    }
}

void Connection::slotNewConnection()
{
    QTcpSocket *tmp = server->nextPendingConnection();
    qint64 id = tmp->socketDescriptor();
    WaitingForConfirmation.insert(id, tmp);

    std::cout << prefix << "New conncetion at socket " << id << "\n";
    connect(tmp, SIGNAL(readyRead()), this, SLOT(slotRead()));
    connect(tmp, SIGNAL(disconnected()), this, SLOT(slotDisconnectWarning()));
    connect(tmp, SIGNAL(disconnected()), tmp, SLOT(deleteLater()));
}

void Connection::slotRead()
{
    QTcpSocket *soc = (QTcpSocket *)QObject::sender();
    qint64 id = soc->socketDescriptor();
    QString message = QString().fromLocal8Bit(soc->readAll());

    // Connection establishing
    // Need to make this a part of a protocol
    // and move to another scoupe
    if (!message.compare(connectReq))
    {
        if (soc->write(connectResp.toLocal8Bit()) == connectResp.length() ||
            soc->write(connectResp.toLocal8Bit()) == connectResp.length() ||
            soc->write(connectResp.toLocal8Bit()) == connectResp.length() )
        {
            std::cout << prefix << "The connection (" << id << ") is established\n";
            socketMap.insert(id, WaitingForConfirmation[id]);
            WaitingForConfirmation.remove(id);
            chat->AddNewOne(id);
        }
        else
        {
            std::cout << prefix << "Connection failure\n";
            WaitingForConfirmation.remove(id);
            soc->disconnect();
            return;
        }
    }
    else if (!message.compare(connectResp))
    {
        std::cout << prefix << "The connection (" << id << ") is established\n";
        socketMap.insert(id, WaitingForConfirmation[id]);
        WaitingForConfirmation.remove(id);
        chat->AddNewOne(id);
    }
    else if (socketMap.contains(id))
        chat->AddToChat(id, "From", message);
}

void Connection::slotConnect(QString adr, quint16 port)
{
    QTcpSocket *soc = new QTcpSocket();
    soc->connectToHost(adr, port);

    connect(soc, SIGNAL(connected()), this, SLOT(slotConnectSuccess()));
}

void Connection::AdmesConnectionRequest(QTcpSocket *soc)
{
    qint64 id = soc->socketDescriptor();
    // Three attempts to send
    if (soc->write(connectReq.toLocal8Bit()) != connectReq.length() &&
        soc->write(connectReq.toLocal8Bit()) != connectReq.length() &&
        soc->write(connectReq.toLocal8Bit()) != connectReq.length())
    {
        std::cout << "Connection failure\n";
    }
    else
    {
        std::cout << prefix << "New conncetion at socket " << id << "\n";

        //socketMap.insert(id, soc);
        WaitingForConfirmation.insert(id, soc);
        connect(soc, SIGNAL(readyRead()), this, SLOT(slotRead()));
        connect(soc, SIGNAL(disconnected()), this, SLOT(slotDisconnectWarning()));
        connect(soc, SIGNAL(disconnected()), soc, SLOT(deleteLater()));
    }
}

void Connection::slotConnectSOCKS5(QString addr, quint16 port)
{
    std::cout << prefix << "Connecting to SOCKS server\n";
    QTcpSocket *soc = new QTcpSocket();
    soc->connectToHost("127.0.0.1", socks5_port);

    if (soc->waitForConnected(5000))
    {
        char *request = const_cast<char*>("\05\01\00");
        soc->write(request, 3);
        char response[10];
        soc->read(response, 2);

        if (response[1] != 0x00)
        {
            std::cout << prefix << "SOCKS5 error authentificating\n";
            std::cout << prefix << "SOCKS5 response: ";
            HexOutput(QByteArray(response), ' ', 2);
            std::cout << '\n';

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
            std::cout << prefix << "SOCKS5 error: " << (int)response[1] << "\n";
            soc->close();
            delete soc;
            return;
        }

        qint64 id = soc->socketDescriptor();
        std::cout << prefix << "SOCKS5 connected at socket " << id << "\n";

        AdmesConnectionRequest(soc);
    }
    else
    {
        std::cout << prefix << "SOCKS5 server does not answer\n";
        soc->close();
        delete soc;
   }
}

void Connection::slotConnectSuccess()
{
    QTcpSocket *soc = (QTcpSocket *)QObject::sender();
    AdmesConnectionRequest(soc);
}

void Connection::slotWrite(qint64 id, QString message)
{
    if (socketMap.contains(id))
    {
        // Three attempts to send
        if (socketMap[id]->write(message.toLocal8Bit()) == message.length() ||
            socketMap[id]->write(message.toLocal8Bit()) == message.length() ||
            socketMap[id]->write(message.toLocal8Bit()) == message.length() )
        {
            chat->AddToChat(id, "To", message);
        }
        else
            std::cout << prefix << "Sending failure\n";
    }
    else
        std::cout << prefix << "No socket" << id << "exists";
}

void Connection::slotDisconnect(qint64 id)
{
    if (socketMap.contains(id))
        socketMap[id]->disconnectFromHost();
    else if (WaitingForConfirmation.contains(id))
        WaitingForConfirmation[id]->disconnectFromHost();
    else
        std::cout << prefix << "No such socket exists\n";
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
            std::cout << prefix << "Socket " << soc.key() << " disconnected\n";
            disconnectedSockets.push_back(soc.key());
        }
    }
    for (quint32 index = 0; index < disconnectedSockets.length(); index++)
    {
        chat->Remove(disconnectedSockets.at(index));
        socketMap.remove(disconnectedSockets.at(index));
    }
}

void Connection::slotSpecifyPortForListening(quint16 port)
{
    server_port = port;   
    std::cout << prefix << "Admes will listen to port " << port << "\n";
}

void Connection::slotSpecifyPortForSOCKS5(quint16 port)
{
    socks5_port = port;
    std::cout << prefix << "SOCKS5 service will listen to port " << port << "\n";
}

void Connection::slotReadTorOutput()
{
    QByteArray output = tor->readAllStandardOutput();
    output.append(tor->readAllStandardError());
    QFile fout(QDir::current().absolutePath() + "/tor_config/last_session.log");
    if (fout.open(QIODevice::WriteOnly))
    {
        fout.write(output);
    }
}

#if defined(_WIN32) || defined(_WIN64)

void Connection::slotSpecifyTorPath(QString path)
{
    tor_path = path;
}

#endif
