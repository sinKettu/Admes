#include "control.h"
#include "connection.h"
#include <QThread>

Control::Control(QObject *parent) : QObject(parent)
{
    connection = new Connection();
    QThread    *thread     = new QThread();
    connection->moveToThread(thread);

    connect(thread, SIGNAL(started()),                  connection, SLOT(slotConnectionExec()));

    connect(this, SIGNAL(sigStartServer(quint16)),      connection, SLOT(slotStartServer(quint16)));
    connect(this, SIGNAL(sigStartTorServer(quint16)),   connection, SLOT(slotStartTorServer(quint16)));
    connect(this, SIGNAL(sigConnect(QString, quint16)), connection, SLOT(slotConnect(QString, quint16)));
    connect(this, SIGNAL(sigConnectSOCKS5(QString, quint16)), connection, SLOT(slotConnectSOCKS5(QString, quint16)));
    connect(this, SIGNAL(sigWrite(qint64, QString)),    connection, SLOT(slotWrite(qint64, QString)));
    connect(this, SIGNAL(sigReadAll()),                 connection, SLOT(slotReadIncomming()));
    connect(this, SIGNAL(sigDisconnect(qint64)),        connection, SLOT(slotDisconnect(qint64)));
    connect(this, SIGNAL(sigOutputDialog(qint64)),      connection, SLOT(slotOutputDialog(qint64)));
    connect(this, SIGNAL(sigCloseDialog()),             connection, SLOT(slotCloseDialog()));

    thread->start();
}

void Control::StartServer(quint16 port)
{
    emit sigStartServer(port);
}

void Control::StartTorServer(quint16 port)
{
    emit sigStartTorServer(port);
}

void Control::ConnectTo(QString adr, quint16 port)
{
    emit sigConnect(adr, port);
}

void Control::ConnectThroughSOCKS5(QString addr, quint16 port)
{
    emit sigConnectSOCKS5(addr, port);
}

void Control::Send(qint64 id, QString message)
{
    emit sigWrite(id, message);
}

void Control::ReadAll()
{
    emit sigReadAll();
}

void Control::Disconnect(qint64 id)
{
    emit sigDisconnect(id);
}

void Control::OutputDialog(qint64 id)
{
    emit sigOutputDialog(id);
}

void Control::CloseDialog()
{
    emit sigCloseDialog();
}