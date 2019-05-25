#include <QCoreApplication>
#include <QString>
#include <QTextStream>
#include <stdio.h>
#include "control.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream qout(stdout);
    QTextStream qin(stdin);
    QString input;
    Control *c = new Control();

    while (true)
    {
        printf("(admes) ");
        input = qin.readLine();
        if (input.length() == 0)
            continue;

        QStringList commands = input.split(' ');
        if (commands[0].compare("/open") == 0)
        {
            c->StartServer(commands[1].toUShort());
            continue;
        }
        if (commands[0].compare("/connect") == 0)
        {
            if (commands[1].compare("tor") == 0)
            {
                c->ConnectThroughSOCKS5(commands[2], commands[3].toUShort());
            }
            else
            {
                c->ConnectTo(commands[1], commands[2].toUShort());
                continue;
            }
        }
        if (commands[0].compare("/disconnect") == 0)
        {
            c->Disconnect(commands[1].toLongLong());
            continue;
        }
        if (commands[0].compare("/send") == 0)
        {
            c->Send(commands[1].toLongLong(), commands[2]);
            continue;
        }
        if (commands[0].compare("/read") == 0)
        {
            c->ReadAll();
            continue;
        }
        if (commands[0].compare("/exit") == 0)
        {
            exit(0);
        }

        printf("Wrong command!!!\n");
    }

    return a.exec();
}
