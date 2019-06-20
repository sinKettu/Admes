#include "crypto/AES.h"
#include "crypto/ECC.h"
#include <QString>

bool LogIn(QString login, QString password);

bool CreateAccount(QString login, QString password);

bool IsLoggedIn();

bool IsUserKnown(QString login);

bool NewUser(QString login, QByteArray puk);

QByteArray GetBytePuk();

Point GetPuk();

mpz_t GetPk();