#include "person.h"
#include "server.h"
#include <QThread>

Person::Person(QObject *parent) : QObject(parent)
{

}

void Person::NewSocketFromServer(qint64 id)
{
    qDebug() << "[ DEBUG ] Person knows about socket " << id;
}

void Person::StartServer(quint16 port)
{
    QThread *serverThread = new QThread();
    portForServer = port;
    // Нужно куда-то записать порт
    connect(serverThread, SIGNAL(started()), this, SLOT(StartServerSlot()));
    connect(this, SIGNAL(StopServerThreadSignal()), serverThread, SLOT(quit()));

    serverThread->start();
}

void Person::StartServerSlot()
{
    qDebug() << "[ DEBUG ] Server's thread is started.";

    Server *server = new Server(this);
    // проверить значение portForServer
    // не уверен, что такой connect корректен
    connect(QThread::currentThread(), SIGNAL(finished()), server, SLOT(ShutDownSLot()));
    server->Start(portForServer);
    portForServer = 0;
}

void Person::StopServer()
{
    qDebug() << "[ DEBUG ] Stopping server.";

    emit(StopServerThreadSignal());
}

