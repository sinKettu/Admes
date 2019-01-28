#include <QCoreApplication>
#include <QString>
#include <QTextStream>
#include <stdio.h>
#include "person.h"

int main(/*int argc, char *argv[]*/)
{
    QTextStream qout(stdout);
    QTextStream qin(stdin);
    char* buffer = new char[256];
    QString input;

    while (true)
    {
        printf("(admes) ");
        scanf("%s", buffer);
        input = buffer;
        QStringList commands = input.split(' ');
        if (commands[0].compare("/open") == 0)
        {
            Person *person = new Person();
            person->commandOpen(4242);
            break;
        }
        if (commands[0].compare("/connect") == 0)
        {
            Person *person = new Person();
            person->commandConnect("127.0.0.1", 4242);
            break;
        }
        qout << "Wrong command!!!\n";
    }

    return 0;
}
