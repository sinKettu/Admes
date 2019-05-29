#include <iostream>
#include <iomanip>
#include <string>
#include <QString>

#define HEX_BYTE(x) std::setw(2) << std::setfill('0') << std::hex << (int)(x)

const std::string prefix = "[admes] ";

void HighLight(QString str);

void HexOutput(QByteArray buf, char separator, qint32 n);