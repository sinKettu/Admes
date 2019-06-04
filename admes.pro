QT -= gui
QT += network core

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# INCLUDEDIR += $$PWD/libs/mpir/include
# LIBS += $$PWD/libs/mpir/lib/libmpir.so
LIBS += -lmpir

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/main.cpp \
    src/control.cpp \
    src/connection.cpp \
    src/chat.cpp \
    src/common.cpp \
	src/crypto/AES.cpp \
	src/crypto/ECC.cpp \
	src/crypto/prng.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/control.h \
    src/connection.h \
    src/chat.h \
    src/common.h \
#	src/crypto/AES.h \
	src/crypto/ECC.h \
	src/crypto/prng.h

win32|win64 {
    LIBS += libws2_32
}
