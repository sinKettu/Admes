QT -= gui
QT += network core

CONFIG += c++11 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

win32{
    INCLUDEPATH += $$PWD/libs/mpir
    HEADERS += $$PWD/libs/mpir/mpir.h
    LIBS += $$PWD/libs/mpir/mpir.dll
    LIBS += libws2_32
}else{
    LIBS += -lmpir
}

SOURCES += \
        src/main.cpp \
        src/control.cpp \
        src/connection.cpp \
        src/chat.cpp \
        src/common.cpp \
        src/account.cpp \
            src/crypto/AES.cpp \
            src/crypto/ECC.cpp \
            src/crypto/prng.cpp \
            src/crypto/EC_common.cpp

HEADERS += \
    src/control.h \
    src/connection.h \
    src/chat.h \
    src/common.h \
    src/account.h \
	    src/crypto/AES.h \
	    src/crypto/ECC.h \
	    src/crypto/prng.h \
        src/crypto/EC_common.h
