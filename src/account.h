#include "crypto/AES.h"
#include "crypto/ECC.h"
#include <QString>

bool LogIn(QString login, QString password);

bool CreateAccount(QString login, QString password);

bool IsLoggedIn();

bool IsUserKnown(QString login);

bool CheckKey(QString login, Point puk);

void RemovePeer(QString login);

bool NewPeer(QString login, Point pk);

void ShowKnown();

QByteArray GetBytePuk();

QString GetLogin();

EllipticCurve *GetEC();

Keychain *GetKeys();