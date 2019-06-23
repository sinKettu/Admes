#include "control.h"
#include "connection.h"

Control::Control(QObject *parent) : QObject(parent)
{
    thread = new QThread();
    connection.moveToThread(thread);

    connect(thread, SIGNAL(started()),                  &connection, SLOT(slotConnectionExec()));
    connect(thread, SIGNAL(finished()),                 thread,      SLOT(deleteLater()));
    //connect(thread, SIGNAL(finished()),                 this,        SLOT(deleteLater()));
    connect(&connection, SIGNAL(sigTerminateThread()),  thread,      SLOT(quit()), Qt::DirectConnection);

    connect(this, SIGNAL(sigStartServer()),             &connection, SLOT(slotStartServer()));
    connect(this, SIGNAL(sigStopAll()),                 &connection, SLOT(slotStopAll()));
    connect(this, SIGNAL(sigConnect(QString, quint16)), &connection, SLOT(slotConnect(QString, quint16)));
    connect(this, SIGNAL(sigConnectSOCKS5(QString, quint16)), &connection, SLOT(slotConnectSOCKS5(QString, quint16)));
    connect(this, SIGNAL(sigWrite(QString, QString)),    &connection, SLOT(slotWrite(QString, QString)));
    connect(this, SIGNAL(sigDisconnect(qint64)),        &connection, SLOT(slotDisconnect(qint64)));
    connect(this, SIGNAL(sigOutputDialog(QString)),      &connection, SLOT(slotOutputDialog(QString)));
    connect(this, SIGNAL(sigCloseDialog()),             &connection, SLOT(slotCloseDialog()));
    connect(this, SIGNAL(sigRunTor()),                  &connection, SLOT(slotRunTor()));
    connect(this, SIGNAL(sigSpecifyPortForListening(quint16)), &connection, SLOT(slotSpecifyPortForListening(quint16)));
    connect(this, SIGNAL(sigSpecifyPortForSOCKS5(quint16)),  &connection,   SLOT(slotSpecifyPortForSOCKS5(quint16)));
    connect(this, SIGNAL(sigShowTorLog()),              &connection, SLOT(slotShowTorLog()));
    connect(this, SIGNAL(sigAccept(qint64)),            &connection, SLOT(slotAccept(qint64)));
    connect(this, SIGNAL(sigRefuse(qint64)),            &connection, SLOT(slotRefuse(qint64)));
    connect(this, SIGNAL(sigShowConnected()),           &connection, SLOT(slotShowConnected()));

#if defined(_WIN32) || defined(_WIN64)

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
    while (thread->isRunning());
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

void Control::Send(QString peerName, QString message)
{
    emit sigWrite(peerName, message);
}

void Control::Disconnect(qint64 id)
{
    emit sigDisconnect(id);
}

void Control::OutputDialog(QString peer)
{
    emit sigOutputDialog(peer);
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

void Control::ShowTorLog()
{
    emit sigShowTorLog();
}

void Control::Accept(qint64 id)
{
    emit sigAccept(id);
}

void Control::Refuse(qint64 id)
{
    emit sigRefuse(id);
}

void Control::ShowConnected()
{
    emit sigShowConnected();
}

#if defined(_WIN32) || defined(_WIN64)

void Control::SpecifyTorPath(QString path)
{
    emit sigSpecifyTorPath(path);
}

#endif
