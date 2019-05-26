#ifndef CHAT_H
#define CHAT_H

#include <QObject>
#include <QMap>
#include <QStringList>

class Chat : public QObject
{
    Q_OBJECT
public:
    explicit Chat(QObject *parent = nullptr);
    bool Output(qint64 id);
    void AddToChat(qint64 id, QString who, QString message);

signals:

public slots:
    void slotAddToChat(qint64 id, QString who, QString message);

private:
    QMap<qint64, QStringList> chats;
    qint64 current = 0;
};

#endif // CHAT_H
