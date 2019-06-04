#include <iostream>
#include <iomanip>
#include <string>

#include <QString>
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QThread>

#define HEX_BYTE(x) std::setw(2) << std::setfill('0') << std::hex << (int)(x)

const std::string prefix = "[admes] ";

void HighLight(QString str);

void HexOutput(QByteArray buf, char separator, qint32 n);

bool TorIsRunning();

bool KillTor();