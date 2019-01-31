#include "dialog.h"

Dialog::Dialog(QTcpSocket *incommingSocket, QObject *parent) : QObject(parent)
{
    soc = incommingSocket;

    connect(soc, SIGNAL(readyRead()), this, SLOT(slotReceiver()));
    connect(soc, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotHandleError(QAbstractSocket::SocketError)));
}

void Dialog::WaitForReadable()
{
    qDebug() << "==>\tDialog " << soc->socketDescriptor() << " is waiting for messages.";

    keepDialog = true;
    while(keepDialog)
    {
        soc->waitForReadyRead(1000);
    }

    qDebug() << "==>\tDialog " << soc->socketDescriptor() << " is closing.";

    soc->close();
    delete soc;
}

void Dialog::slotReceiver()
{
    qDebug() << "==>\tDialog " << soc->socketDescriptor() << ": new message.";

    QString incommingMessage = soc->readAll();
    emit(sigReturnMessage(soc->socketDescriptor(), incommingMessage));
}

void Dialog::slotHandleError(QAbstractSocket::SocketError se)
{
    if (se != QAbstractSocket::SocketTimeoutError  )
        qDebug() << "==>\tDialog " << soc->socketDescriptor() << ": Error: " << se;
}

void Dialog::CloseDialog(qint64 id)
{
    if (id == soc->socketDescriptor())
        keepDialog = false;
}

void Dialog::slotWriteMessage(qint64 id, const char* message)   // Добавить длину передачи
{
    qDebug() << "==>Dialog: sending message to" << id;

    if (id == soc->socketDescriptor())
    {
        soc->write(message, strlen(message));
    }
}
