#ifdef _WIN32
    // code here
#elif _WIN64
    // code here
#else
    #include <sys/types.h>
    #include <signal.h>
#endif

#include "common.h"

pid_t torPid = 0;

void HighLight(QString str)
{
    QString separator = "+";
    for (qint32 index = 0; index < str.length(); index++)
        separator += "-";
    separator += "+";

    std::cout << std::endl << separator.toStdString() << std::endl;
    std::cout << " " << str.toStdString() << std::endl;
    std::cout << separator.toStdString() << std::endl << std::endl;
}

void HexOutput(QByteArray buf, char separator='\0', qint32 n=INT32_MAX)
{
    n = qMin(buf.length(), n);
    for (qint32 index = 0; index < n; index++)
    {
        std::cout << HEX_BYTE(buf[index]);
        if (separator)
            std::cout << separator;
    }
}

#ifdef _WIN32
    // code here
#elif _WIN64
    // code here
#else

bool TorIsRunning()
{
    QString cmdline = "/stat";
    QDirIterator iter("/proc", QDirIterator::Subdirectories);
    bool result = false;
    
    while (iter.hasNext())
    {
        QString tmp = iter.next();
        if (!tmp.split('/').at(2).toUShort())
            continue;
        tmp = iter.next() + cmdline;
        QFile fin(tmp);
        if (fin.open(QIODevice::ReadOnly))
        {
            QString line = QString::fromLocal8Bit(fin.readLine());
            qint32 left = line.indexOf('(');
            qint32 right = line.indexOf(')');
            fin.close();
            if (left >= 0 && right > 0 && right > left)
            {
                tmp = line.mid(left + 1, right - left - 1);
                if (tmp.compare("tor"))
                    continue;
                
                torPid = line.split(' ')[0].toUInt();
                result = true;
                break;
            }
        }
    }

    return result;
}

bool KillTor()
{
    if (torPid == 0)
        if (!TorIsRunning())
            return false;

    kill(torPid, SIGKILL);
    QThread::sleep(1);
    QString str = "/proc/" + QString::number(static_cast<quint32>(torPid)) + "/stat";
    QFile tmp(str);
    bool result = tmp.open(QIODevice::ReadOnly);
    tmp.close();
    return !result;
}

#endif