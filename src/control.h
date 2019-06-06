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
    void Send(qint64 id, QString message);
    void StopAll();
    void ConnectTo(QString adr, quint16 port);
    void Disconnect(qint64 id);
    void ConnectThroughSOCKS5(QString addr, quint16 port);
    void OutputDialog(qint64 id);
    void CloseDialog();
    void RunTor();
    void SpecifyPortForListening(quint16 port);
    void SpecifyPortForSOCKS5(quint16 port);
    void ShowTorLog();

#if defined(_WIN32) || defined(_WIN64)

    void SpecifyTorPath(QString path);

#endif

signals:
    void sigStartServer();
    void sigStopAll();
    void sigWrite(qint64, QString);
    void sigConnect(QString, quint16);
    void sigConnectSOCKS5(QString, quint16);
    void sigDisconnect(qint64);
    void sigOutputDialog(qint64 id);
    void sigCloseDialog();
    void sigRunTor();
    void sigSpecifyPortForListening(quint16);
    void sigSpecifyPortForSOCKS5(quint16);
    void sigCloseProgram();
    void sigShowTorLog();
    
#if defined(_WIN32) || defined(_WIN64)

    void sigSpecifyTorPath(QString);

#endif

private:
    Connection connection;
    QThread *thread;
};

#endif // CONTROL_H