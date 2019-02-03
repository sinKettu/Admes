#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>

class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = nullptr);
    void StartServer(quint16 port);
    void ReadAll();
    void Send(qint64 id, QString message);
    void ConnectTo(QString adr, quint16 port);
    void Disconnect(qint64 id);

signals:
    void sigStartServer(quint16);
    void sigReadAll();
    void sigWrite(qint64, QString);
    void sigConnect(QString, quint16);
    void sigDisconnect(qint64);

public slots:
};

#endif // CONTROL_H
