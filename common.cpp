#include "common.h"

// const std::string prefix = "[admes] ";

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