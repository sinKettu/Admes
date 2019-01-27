#ifndef GENERALSERVER_H
#define GENERALSERVER_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <ctype.h>

class GeneralServer : public QObject
{
    Q_OBJECT
public:
    explicit GeneralServer(uint16_t port, QObject *parent = nullptr);
    explicit GeneralServer(QObject *parent = nullptr);
    bool Start(uint16_t port);

signals:

public slots:

private slots:
    void handleConnection();
    void readClient();

private:
    QTcpServer *tcpServer;
    QMap<int64_t, QTcpSocket*> socketMap;
};

#endif // GENERALSERVER_H
