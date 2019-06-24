## Admes - Advanced Messenger.
Asynchronous P2P connector (messenger) based on Qt5.

## Current program features:
- Sending and receiving data (messages);
- Dialog (chat) for each connection (peer);
- Working using real IPs (can be useful in LAN);
- Working using TOR network (to connect over NAT);
- End-to-end encryption (see 'CRYPTO' for details);
- Unique pair of public and private ECC keys to verify user identity (see 'CRYPTO' for details);
- Cross-platform;

## Comming program features:
- Working using STUNT (to connect over NAT);
- UI (ncurses);
- GUI (QT Widget);
- Secure exporting of dialogs;
- ...

## Build and compile:
You need:
- Qt5 Framework (>= 10.0);
- Tor (Tor Expert Bundle);
- GMP (>=2) or MPIR (=3.0.0).

Use qmake + make to build and compile.
Build script example for Linux:

    mkdir build || cd build
    qmake -o Makefile ../admes.pro
    make

Also, you can use QtCreator to build and compile (recomended for Windows).
Please, see 'INSTALL' for details.

To get help enter "/help" in the program.

## Copyright © Andrey Ivanov, 2019.