#ifndef CONNECTION_H
#define CONNECTION_H

#include "chat.h"
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
    void slotDisconnect(qint64);
    void slotDisconnectWarning();
    void slotOutputDialog(qint64);
    void slotCloseDialog();

private:
    void CheckUp();

    QEventLoop *loop = nullptr;
    QTcpServer *server = nullptr;
    QMap<qint64, QTcpSocket *> socketMap;
    QProcess *tor = nullptr;
    Chat *chat = nullptr;
};

#endif // CONNECTION_H
