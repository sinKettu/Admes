#include <QCoreApplication>
#include <QString>
#include <QTextStream>
#include <QFile>
#include "control.h"
#include "common.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream qout(stdout);
    QTextStream qin(stdin);
    QString input;
    Control *c = new Control();
    qint64 chatID = 0;

    HighLight(QString("admes is ready to interpret your commands!"));
    while (true)
    {
        input = qin.readLine();
        if (input.length() == 0)
            continue;

        QStringList commands = input.split(' ');
        if (commands[0].compare("/open") == 0 && commands.length() >= 2)
        {
            if (commands[1].compare("tor") == 0 && commands.length() == 3)
            {
            #ifdef _WIN32

                std::cout << prefix << "It seems like you use windows\n";
                std::cout << prefix << "Please, specify path to tor.exe\n";
                std::cout << prefix << "Type '/back' to return to interpreter\n";

                QString tor_path = qin.readLine();
                bool valid = false;
                while (true)        // Упростить?
                {
                    valid = QFile(tor_path).exists();
                    if (valid)
                        break;
                    else if (tor_path.compare("/back") == 0)
                        break;
                    else
                    {
                        std::cout << prefix << "No such file exists\n";
                        std::cout << prefix << "Try again\n";
                        tor_path = qin.readLine();
                    }
                }

                if (!valid)
                {
                    HighLight(QString("admes is ready to interpret your commands!"));
                    continue;
                }

                c->SpecifyTorPath(tor_path);

            #elif _WIN64

                std::cout << prefix << "It seems like you use windows\n";
                std::cout << prefix << "Please, specify path to tor.exe\n";
                std::cout << prefix << "Type '/back' to return to interpreter\n";

                QString tor_path = qin.readLine();
                bool valid = false;
                while (true)        // Упростить?
                {
                    valid = QFile(tor_path).exists();
                    if (valid)
                        break;
                    else if (tor_path.compare("/back") == 0)
                        break;
                    else
                    {
                        std::cout << prefix << "No such file exists\n";
                        std::cout << prefix << "Try again\n";
                        tor_path = qin.readLine();
                    }
                }

                if (!valid)
                {
                    HighLight(QString("admes is ready to interpret your commands!"));
                    continue;
                }

                c->SpecifyTorPath(tor_path);

            #endif

                c->StartTorServer(commands[2].toUShort());
                continue;
            }
            else if (commands[1].compare("tor") && commands.length() == 2)
            {
                c->StartServer(commands[1].toUShort());
                continue;
            }
        }
        if (commands[0].compare("/connect") == 0 && commands.length() >= 3)
        {
            if (commands[1].compare("tor") == 0 && commands.length() == 4)
            {
                c->ConnectThroughSOCKS5(commands[2], commands[3].toUShort());
                continue;
            }
            else if (commands[1].compare("tor") && commands.length() == 3)
            {
                c->ConnectTo(commands[1], commands[2].toUShort());
                continue;
            }
        }
        if (commands[0].compare("/disconnect") == 0 && commands.length() == 2)
        {
            c->Disconnect(commands[1].toLongLong());
            continue;
        }
        if (commands[0].compare("/chat") == 0 && commands.length() == 2)
        {
            chatID = commands[1].toLongLong();
            c->OutputDialog(chatID);
            continue;
        }
        if (chatID && input[0] != '/')
        {
            c->Send(chatID, input);
            continue;
        }
        if (commands[0].compare("/close") == 0 && commands.length() == 1)
        {
            chatID = 0;
            c->CloseDialog();
            continue;
        }
        if (commands[0].compare("/exit") == 0)
        {
            exit(0);
        }

        std::cout <<  prefix << "Wrong command!\n";
    }

    return a.exec();
}
