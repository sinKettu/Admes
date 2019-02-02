#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QEventLoop>
#include <QTcpServer>
#include <QTcpSocket>

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = nullptr);

signals:

public slots:
    void slotConnectionExec();
    void slotStartServer(quint16);
    void slotNewConnection();
    void slotRead();
    void slotConnect(QString adr, quint16 port);
    void slotConnectSuccess();
    void slotWrite(qint64 id, QString message);
    void slotReadIncomming();

private:
    QEventLoop *loop = nullptr;
    QTcpServer *server;
    QMap<qint64, QTcpSocket *> socketMap;
    QStringList storage;
};

#endif // CONNECTION_H
