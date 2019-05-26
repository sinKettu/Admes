#include "chat.h"
#include <QTextStream>

Chat::Chat(QObject *parent) : QObject(parent)
{

}

void Chat::AddToChat(qint64 id, QString who, QString message)
{
    QTextStream qout(stdout);
    if (chats.contains(id))
    {
        chats[id] << who << message;

        if (id == current)
        {
            qout << "[" << who << "]\n";
            qout << message;
            qout << "\n";
        }
    }
    else
    {
        chats[id] = QStringList();
        chats[id] << who << message;
        qout << "[" << who << "]\n";
        qout << message;
        qout << "\n";
    }
}

void Chat::slotAddToChat(qint64 id, QString who, QString message)
{
    AddToChat(id, who, message);
}

bool Chat::Output(qint64 id)
{
    QTextStream qout(stdout);
    QMap<qint64, QStringList>::iterator currentChat = chats.find(id);
    if (currentChat != chats.end() && currentChat->length())
    {
        for (quint32 index = 0; index < currentChat->length(); index+=2)
        {
            qout << "[" << currentChat->at(index) << "]\n";
            qout << currentChat->at(index) << "\n";
        }
        return true;
    }
    if (currentChat != chats.end() && currentChat->empty())
    {
        qout << "Selected chat is empty\n";
        return false;
    }
    else
    {
        qout << "There is no such chat\n";
        return false;
    }
}