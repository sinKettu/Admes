#ifndef CONNECTION_H
#define CONNECTION_H

#include "chat.h"
#include "account.h"
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
    void slotWrite(QString peerName, QString message);
    void slotDisconnect(qint64);
    void slotDisconnectWarning();
    void slotOutputDialog(QString peer);
    void slotCloseDialog();
    void slotRunTor();
    void slotSpecifyPortForListening(quint16 port);
    void slotSpecifyPortForSOCKS5(quint16 port);
    void slotReadTorOutput();
    void slotShowTorLog();
    void slotAccept(qint64);
    void slotRefuse(qint64);
    void slotShowConnected();

#if defined(_WIN32) || defined(_WIN64)

    void slotSpecifyTorPath(QString path);

#endif

private:
    void CheckUp();
    void AdmesConnectionRequest(QTcpSocket *);
    void BadConnection(qint64 id);
    bool SendPuk(QTcpSocket *soc);
    void SendLogin(QTcpSocket *soc, qint64 id);
    int CheckPeer(qint64 id, QByteArray data, QString &strLogin);
    bool SendNumber(qint64 id);
    void OpenSession(qint64 id, QTcpSocket *soc, QByteArray message);
    QByteArray MessagePackaging(QString message, qint64 id);
    QString MessageExtract(QByteArray data, qint64 id, bool &signature);

    QMap<qint64, QTcpSocket *> socketMap;
    QMap<qint64, QTcpSocket *> WaitingForConfirmation;
    QMap<qint64, qint32> connectionStage;
    QMap<qint64, Point> pukMap;
    QMap<QString, qint64> peerID;
    QMap<qint64, QByteArray> peersSessionKeys;
    QMap<qint64, QString> IDPeer;

    QTcpServer *server;
    QProcess *tor;
    Chat *chat = nullptr;
    quint16 serverPort = 4242;
    quint16 socks5Port = 9050;
    bool firstWriting = true;

    // In UNIX will be used just "tor" command
    //      (tor should be installed)
    // In Windows user should specify path to tor.exe
    QString tor_path = "tor";
};

#endif // CONNECTION_H
