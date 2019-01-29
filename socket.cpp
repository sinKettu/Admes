#include "socket.h"
#include "person.h"

Socket::Socket(QTcpSocket ** sckt, QObject *parent) : QObject(parent)
{
    ConnectionListener = *sckt;

    connect(ConnectionListener, SIGNAL(readyRead()), this, SLOT(ReadFromSocketSlot()));
    connect(this, SIGNAL(TakeTheMessage(qint64, QString)), (Person*)this->parent(), SLOT(MessageReceiver(qint64, QString)));
    connect((Person*)this->parent(), SIGNAL(SendMessageSignal(qint64, QString)), this, SLOT(SendMessageSlot(qint64, QString)));
}

void Socket::Wait(quint32 ms)
{
    qDebug() << "[ DEBUG ] Socker " << ConnectionListener->socketDescriptor() << " started to listen.";
    while (ContinueWaiting)
    {
       ConnectionListener->waitForReadyRead(ms);
    }
}

void Socket::ReadFromSocketSlot()
{
    qDebug() << "[ DEBUG ] Message from socket " << ConnectionListener->socketDescriptor();
    QString message;
    message = ConnectionListener->readAll();

    emit(TakeTheMessage(ConnectionListener->socketDescriptor(), message));
}

void Socket::SendMessageSlot(qint64 id, QString message)
{
    if (id == ConnectionListener->socketDescriptor())
    {
        ConnectionListener->write(message.toStdString().c_str());
    }
}
