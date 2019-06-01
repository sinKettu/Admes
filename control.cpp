#include "control.h"
#include "connection.h"

Control::Control(QObject *parent) : QObject(parent)
{
    thread = new QThread();
    connection.moveToThread(thread);

    connect(thread, SIGNAL(started()),                  &connection, SLOT(slotConnectionExec()));
    connect(thread, SIGNAL(finished()),                 thread,      SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()),                 this,        SLOT(deleteLater()));
    connect(&connection, SIGNAL(sigTerminateThread()),  thread,      SLOT(quit()));

    connect(this, SIGNAL(sigStartServer()),             &connection, SLOT(slotStartServer()));
    connect(this, SIGNAL(sigStopAll()),                 &connection, SLOT(slotStopAll()));
    connect(this, SIGNAL(sigConnect(QString, quint16)), &connection, SLOT(slotConnect(QString, quint16)));
    connect(this, SIGNAL(sigConnectSOCKS5(QString, quint16)), &connection, SLOT(slotConnectSOCKS5(QString, quint16)));
    connect(this, SIGNAL(sigWrite(qint64, QString)),    &connection, SLOT(slotWrite(qint64, QString)));
    connect(this, SIGNAL(sigDisconnect(qint64)),        &connection, SLOT(slotDisconnect(qint64)));
    connect(this, SIGNAL(sigOutputDialog(qint64)),      &connection, SLOT(slotOutputDialog(qint64)));
    connect(this, SIGNAL(sigCloseDialog()),             &connection, SLOT(slotCloseDialog()));
    connect(this, SIGNAL(sigRunTor()),                  &connection, SLOT(slotRunTor()));
    connect(this, SIGNAL(sigSpecifyPortForListening(quint16)), &connection, SLOT(slotSpecifyPortForListening(quint16)));
    connect(this, SIGNAL(sigSpecifyPortForSOCKS5(quint16)),  &connection,   SLOT(slotSpecifyPortForSOCKS5(quint16)));
#ifdef _WIN32
    connect(this, SIGNAL(sigSpecifyTorPath(QString)),   &connection, SLOT(slotSpecifyTorPath(QString)));
#elif _WIN64
    connect(this, SIGNAL(sigSpecifyTorPath(QString)),   &connection, SLOT(slotSpecifyTorPath(QString)));
#endif

    thread->start();
}

void Control::StartServer()
{
    emit sigStartServer();
}

void Control::StopAll()
{
    emit sigStopAll();
    emit sigCloseProgram();
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

void Control::RunTor()
{
    emit sigRunTor();
}

void Control::SpecifyPortForListening(quint16 port)
{
    emit sigSpecifyPortForListening(port);
}

void Control::SpecifyPortForSOCKS5(quint16 port)
{
    emit sigSpecifyPortForSOCKS5(port);
}

#ifdef _WIN32

void Control::SpecifyTorPath(QString path)
{
    emit sigSpecifyTorPath(path);
}

#elif _WIN64

void Control::SpecifyTorPath(QString path)
{
    emit sigSpecifyTorPath(path);
}

#endif
