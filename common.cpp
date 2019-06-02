#include "common.h"

#if defined(_WIN32) || defined(_WIN64)

    #include <windows.h>
    #include <tlhelp32.h>
    DWORD torPid = 0;

#else

    #include <sys/types.h>
    #include <signal.h>
    pid_t torPid = 0;

#endif

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

#if defined(_WIN32) || defined(_WIN64)

bool TorIsRunning()
{
    PROCESSENTRY32 peProcessEntry;
    HANDLE CONST hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot)
    {
        return false;
    }
    peProcessEntry.dwSize = sizeof(PROCESSENTRY32);
    Process32First(hSnapshot, &peProcessEntry);
    do
    {
        QString tmp;
        for (int i = 0; i < 260; i+=2)
            tmp.push_back(*((char *)peProcessEntry.szExeFile + i));

        if (tmp.indexOf("tor.exe") == 0)
        {
            torPid = peProcessEntry.th32ProcessID;
            CloseHandle(hSnapshot);
            return true;
        }
    } while(Process32Next(hSnapshot, &peProcessEntry));

    CloseHandle(hSnapshot);
    return false;
}

bool KillTor()
{
    if (torPid == 0)
        if (!TorIsRunning())
            return false;

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, torPid);
    return TerminateProcess(hProcess, 0);
}

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

    return !kill(torPid, SIGKILL);
}

#endif
