#include <QCoreApplication>
#include <QString>
#include <QTextStream>
#include <stdio.h>
//#include "test.h"
#include "control.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream qout(stdout);
    QTextStream qin(stdin);
   // char* buffer = new char[256];
    QString input;
    Control *c = new Control();
    //Test *t = new Test();

    while (true)
    {
        printf("(admes) ");
        /*scanf("%s", buffer);
        input = buffer;*/
        input = qin.readLine();
        QStringList commands = input.split(' ');
        if (commands[0].compare("/open") == 0)
        {
            c->StartServer(commands[1].toUShort());
           // t->StartServer(commands[1].toUShort());
            //person->commandOpen(commands[1].toUShort());
            continue;
        }
        if (commands[0].compare("/connect") == 0)
        {
            c->ConnectTo(commands[1], commands[2].toUShort());
            //t->Connect(commands[1], commands[2].toUShort());
            //person->commandConnect(commands[1], commands[2].toUShort());
            continue;
        }
        if (commands[0].compare("/disconnect") == 0)
        {
            //person->commandDisconnectAll();
            break;
        }
        if (commands[0].compare("/send") == 0)
        {
            c->Send(commands[1].toLongLong(), commands[2]);
            //t->Send(commands[1].toLongLong(), commands[2]);
          //  person->commandSend(commands[1]);
            continue;
        }
        if (commands[0].compare("/read") == 0)
        {
            c->ReadAll();
            //t->Read();
            //person->commandRead();
            continue;
        }

        printf("Wrong command!!!\n");
    }

    return a.exec();
}
