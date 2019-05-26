#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include "connection.h"

class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = nullptr);
    void StartServer(quint16 port);
    void StartTorServer(quint16 port);
    void Send(qint64 id, QString message);
    void ConnectTo(QString adr, quint16 port);
    void Disconnect(qint64 id);
    void ConnectThroughSOCKS5(QString addr, quint16 port);
    void OutputDialog(qint64 id);
    void CloseDialog();

signals:
    void sigStartServer(quint16);
    void sigStartTorServer(quint16 port);
    void sigWrite(qint64, QString);
    void sigConnect(QString, quint16);
    void sigConnectSOCKS5(QString, quint16);
    void sigDisconnect(qint64);
    void sigOutputDialog(qint64 id);
    void sigCloseDialog();

private:
    Connection *connection = nullptr;

public slots:
};

#endif // CONTROL_H
