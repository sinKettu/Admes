#include "control.h"
#include "connection.h"
#include <QThread>

Control::Control(QObject *parent) : QObject(parent)
{
    Connection *connection = new Connection();
    QThread *thread = new QThread();
    connection->moveToThread(thread);

    connect(thread, SIGNAL(started()), connection, SLOT(slotConnectionExec()));

    connect(this, SIGNAL(sigStartServer(quint16)),      connection, SLOT(slotStartServer(quint16)));
    connect(this, SIGNAL(sigConnect(QString, quint16)), connection, SLOT(slotConnect(QString, quint16)));
    connect(this, SIGNAL(sigWrite(qint64, QString)),    connection, SLOT(slotWrite(qint64, QString)));
    connect(this, SIGNAL(sigReadAll()),                 connection, SLOT(slotReadIncomming()));

    thread->start();
}

void Control::StartServer(quint16 port)
{
    emit sigStartServer(port);
}

void Control::ConnectTo(QString adr, quint16 port)
{
    emit sigConnect(adr, port);
}

void Control::Send(qint64 id, QString message)
{
    emit sigWrite(id, message);
}

void Control::ReadAll()
{
    emit sigReadAll();
}

