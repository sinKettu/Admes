#ifndef PERSON_H
#define PERSON_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QThread>

class Person : public QObject
{
    Q_OBJECT
public:
    QMap<qint64, QTcpSocket *> Sockets;
    explicit Person(QObject *parent = nullptr);
    void StartServer(quint16 port);
    void StopServer();
    //void CloseConnection(qint64 id);

signals:
    void StopServerThreadSignal();
    void SendMessageSignal(qint64, QString);

public slots:
    void NewSocketFromServer(qint64 id);
    void MessageReceiver(QString message);

private slots:
    void StartServerSlot();
    void StartListenToSocketSlot();

private:
    quint16 portForServer = 0;
    qint64 nextSocket = 0;
};

#endif // PERSON_H
