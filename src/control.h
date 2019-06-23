#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <QThread>
#include "connection.h"

class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = nullptr);
    void StartServer();
    void Send(QString peerName, QString message);
    void StopAll();
    void ConnectTo(QString adr, quint16 port);
    void Disconnect(QString peer);
    void ConnectThroughSOCKS5(QString addr, quint16 port);
    void OutputDialog(QString peer);
    void CloseDialog();
    void RunTor();
    void SpecifyPortForListening(quint16 port);
    void SpecifyPortForSOCKS5(quint16 port);
    void ShowTorLog();
    void Accept(qint64 id);
    void Refuse(qint64 id);
    void ShowConnected();

#if defined(_WIN32) || defined(_WIN64)

    void SpecifyTorPath(QString path);

#endif

signals:
    void sigStartServer();
    void sigStopAll();
    void sigWrite(QString, QString);
    void sigConnect(QString, quint16);
    void sigConnectSOCKS5(QString, quint16);
    void sigDisconnect(QString);
    void sigOutputDialog(QString);
    void sigCloseDialog();
    void sigRunTor();
    void sigSpecifyPortForListening(quint16);
    void sigSpecifyPortForSOCKS5(quint16);
    void sigCloseProgram();
    void sigShowTorLog();
    void sigAccept(qint64);
    void sigRefuse(qint64);
    void sigShowConnected();
    
#if defined(_WIN32) || defined(_WIN64)

    void sigSpecifyTorPath(QString);

#endif

private:
    Connection connection;
    QThread *thread;
};

#endif // CONTROL_H
