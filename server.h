#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    void Start(quint16 port);

signals:
    void NewSocketForParrent(qint64);

private slots:
    void HandleConnection();

public slots:
    void ShutDownSlot();

private:
    QTcpServer *ConnectionReceiver = nullptr;
    bool continueListening = true;
};

#endif // SERVER_H
