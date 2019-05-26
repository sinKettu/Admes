#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QEventLoop>
#include <QTcpServer>
#include <QTcpSocket>
#include <QProcess>

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = nullptr);

signals:

public slots:
    void slotConnectionExec();
    void slotStartServer(quint16);
    void slotStartTorServer(quint16);
    void slotNewConnection();
    void slotRead();
    void slotConnect(QString adr, quint16 port);
    void slotConnectSOCKS5(QString addr, quint16 port);
    void slotConnectSuccess();
    void slotWrite(qint64 id, QString message);
    void slotReadIncomming();
    void slotDisconnect(qint64);
    void slotDisconnectWarning();

private:
    QEventLoop *loop = nullptr;
    QTcpServer *server;
    QMap<qint64, QTcpSocket *> socketMap;
    QStringList storage;
    QProcess *tor;
};

#endif // CONNECTION_H
