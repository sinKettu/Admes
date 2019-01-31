#pragma once

#ifndef DIALOG_H
#define DIALOG_H

#include <QObject>
#include <QTcpSocket>

class Dialog : public QObject
{
    Q_OBJECT
public:
    explicit Dialog(QTcpSocket *incommingSocket, QObject *parent = nullptr);

signals:
    void sigReturnMessage(qint64, QString);

public slots:
    void slotReceiver();
    void slotHandleError(QAbstractSocket::SocketError se);
    void WaitForReadable();
    void slotWriteMessage(qint64 id, const char* message);
    void CloseDialog(qint64);

private:
    QTcpSocket *soc = nullptr;
    bool keepDialog = false;
};

#endif // DIALOG_H
