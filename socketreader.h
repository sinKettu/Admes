#ifndef SOCKETREADER_H
#define SOCKETREADER_H

#include <QObject>
#include <QTcpSocket>

class SocketReader : public QObject
{
    Q_OBJECT
public:
    explicit SocketReader(QTcpSocket ** sckt, QObject *parent = nullptr);

signals:
    void ReceiveTheMessage(QString message);    // for Person

public slots:
    void Reader();
    void ErrorHandler();
};

#endif // SOCKETREADER_H
