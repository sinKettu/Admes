#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QTcpSocket>

class Socket : public QObject
{
    Q_OBJECT
public:
    explicit Socket(QTcpSocket ** sckt, QObject *parent = nullptr);
    void Wait(quint32 ms);

signals:
    void TakeTheMessage(qint64, QString);

public slots:
    void SendMessageSlot(qint64 id, QString message);

private slots:
    void ReadFromSocketSlot();

private:
    QTcpSocket *ConnectionListener = nullptr;
    bool ContinueWaiting = true;
};

#endif // SOCKET_H
