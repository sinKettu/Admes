#include "person.h"
#include "server.h"
#include "dialog.h"
#include <QThread>

Person::Person(QObject *parent) : QObject(parent)
{

}

void Person::StartServer(quint16 port)
{
    Server *server = new Server(port);
    QThread *thread = new QThread();
    server->moveToThread(thread);

    // connect -- запуск сервера в потоке
    connect(thread, SIGNAL(started()), server, SLOT(StartListening()));
    // connect -- появляется соединение, ссылку сюда
    connect(server, SIGNAL(sigSendSocket(QTcpSocket *)), this, SLOT(slotReceiveNewConnection(QTcpSocket *)));
    // connect -- сигнал на завершение, сервер выключается, поток заканчивается
    connect(this, SIGNAL(sigStopServer()), server, SLOT(slotStop()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void Person::slotReceiveNewConnection(QTcpSocket *soc)
{
    qDebug() << "==>\tPerson: Got new socket - " << soc->socketDescriptor();

    Dialog *dialog = new Dialog(soc);
    QThread *thread = new QThread();
    dialog->moveToThread(thread);

    // connect -- запуск сокета в потоке
    connect(thread, SIGNAL(started()), dialog, SLOT(WaitForReadable()));
    // connect -- копирование принятых сообщений сюда
    connect(dialog, SIGNAL(sigReturnMessage(qint64, QString)), this, SLOT(slotReceiveMessage(qint64, QString)));
    // connect -- отправка сообщений отсюда
    connect(this, SIGNAL(sigSendMessage(qint64, const char*)), dialog, SLOT(slotWriteMessage(qint64, const char*)));
    // connect -- завершение
    connect(this, SIGNAL(sigCloseDialog(qint64)), dialog, SLOT(CloseDialog(qint64)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();

    //soc = nullptr; ???
}

void Person::slotReceiveMessage(qint64 id, QString message)
{
    messageStorage.push_back(message);
    messageStorage.push_back(QString::number(id));
}

void Person::CreateDialog(QString address, quint16 port)
{
    outcommingSocket = new QTcpSocket();
    connect(outcommingSocket, SIGNAL(connected()), this, SLOT(slotSocketConnected()));
    outcommingSocket->connectToHost(address, port);
    while(!outcommingSocket->waitForConnected(100));
}

void Person::slotSocketConnected()
{
    qDebug() << "==>\tPerson: Outcomming connection!";

    Dialog *dialog = new Dialog(outcommingSocket);
    QThread *thread = new QThread();
    dialog->moveToThread(thread);

    // connect -- запуск сокета в потоке
    connect(thread, SIGNAL(started()), dialog, SLOT(WaitForReadable()));
    // connect -- копирование принятых сообщений сюда
    connect(dialog, SIGNAL(sigReturnMessage(qint64, QString)), this, SLOT(slotReceiveMessage(qint64, QString)));
    // connect -- отправка сообщений отсюда
    connect(this, SIGNAL(sigSendMessage(qint64, const char*)), dialog, SLOT(slotWriteMessage(qint64, const char*)));
    // connect -- завершение
    connect(this, SIGNAL(sigCloseDialog(qint64)), dialog, SLOT(CloseDialog(qint64)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();

    //outcommingSocket = nullptr; ???
}

void Person::SendMessage(qint64 id, QString message)
{
    qDebug() << "==>\tPerson: sending message to" << id;

    emit(sigSendMessage(id, message.toStdString().c_str()));
}

void Person::ReadAllPost()
{
    if (messageStorage.isEmpty())
    {
        qDebug() << "==>\tPerson: nothing to read";
    }
    else
    {
        while (!messageStorage.isEmpty())
        {
            printf("[%s]\n:", messageStorage.last().toStdString().c_str());
            messageStorage.pop_back();
            printf("%s\n\n", messageStorage.last().toStdString().c_str());
            messageStorage.pop_back();
        }
    }
}
