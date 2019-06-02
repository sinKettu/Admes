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
    ~Connection();

signals:
    void sigTerminateThread();

public slots:
    void slotConnectionExec();
    void slotStartServer();
    void slotStopAll();
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
    void slotRunTor();
    void slotSpecifyPortForListening(quint16 port);
    void slotSpecifyPortForSOCKS5(quint16 port);
    void slotReadTorOutput();

#if defined(_WIN32) || defined(_WIN64)

    void slotSpecifyTorPath(QString path);

#endif

private:
    void CheckUp();
    void AdmesConnectionRequest(QTcpSocket *);

    QTcpServer *server = nullptr;
    QMap<qint64, QTcpSocket *> socketMap;
    QMap<qint64, QTcpSocket *> WaitingForConfirmation;
    QProcess *tor = nullptr;
    Chat *chat = nullptr;
    quint16 server_port = 4242;
    quint16 socks5_port = 9050;
    // In UNIX will be used just "tor" command
    //      (tor should be installed)
    // In Windows user should specify path to tor.exe
    QString tor_path = "tor";
};

#endif // CONNECTION_H
