#include "chat.h"
#include <iostream>

Chat::Chat(QObject *parent) : QObject(parent)
{

}

void Chat::AddToChat(qint64 id, QString who, QString message)
{
    if (chats.contains(id))
    {
        if (who == "From" && (message == "HelloFromTor" || message == "MirrorHello"))
        {

                chats[id].clear();
                return;
        }

        chats[id] << who << message;

        if (id == current && who == "From")
        {
            std::cout << "\n[" << who.toStdString() << "]\n";
            std::cout << message.toStdString();
            std::cout << "\n\n";
        }
        if (id == current && who == "To")
            std::cout << "\n";
    }
    else
    {
        chats[id] = QStringList();
        if (!(who == "From" && (message == "HelloFromTor" || message == "MirrorHello")))
        {
            chats[id] << who << message;
            std::cout << "[" << who.toStdString() << "]\n";
            std::cout << message.toStdString();
            std::cout << "\n";
        }
    }
}

void Chat::slotAddToChat(qint64 id, QString who, QString message)
{
    AddToChat(id, who, message);
}

bool Chat::Output(qint64 id)
{
    QMap<qint64, QStringList>::iterator currentChat = chats.find(id);
    if (currentChat != chats.end() && currentChat->length())
    {
        current = id;
        for (quint32 index = 0; index < currentChat->length(); index+=2)
        {
            std::cout << "\n[" << currentChat->at(index).toStdString() << "]\n";
            std::cout << currentChat->at(index + 1).toStdString() << "\n\n";
        }
        return true;
    }
    if (currentChat != chats.end() && currentChat->empty())
    {
        current = id;
        std::cout << "Selected chat is empty\n\n";
        return false;
    }
    else
    {
        std::cout << "There is no such chat\n\n";
        return false;
    }
}

void Chat::AddNewOne(qint64 id)
{
    if (!chats.contains(id))
        chats.insert(id, QStringList());
}

void Chat::Close()
{
    current = 0;
}