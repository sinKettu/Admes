Admes - Advanced Messenger.
Asynchronous P2P connector based on Qt5.

Current program features:
- Sending and receiving data (messages);
- Dialog (chat) for each connection;
- Working using real IPs (can be useful in LAN);
- Working using TOR network (to connect over NAT);
- Cross-platform;

Comming program features:
- End-to-end encryption;
- Unique pair of public and private ECC keys to verify user identity;
- Working using STUNT (to connect over NAT);
- UI (ncurses);
- GUI (QT Widget);
- ...

Build and compile:
You need Qt5 Framework and TOR (Expert Bundle) installed on your PC.
- Use qmake + make to build and compile.
    Build script example for Linux:

    mkdir build || cd build
    qmake -o Makefile ../admes.pro
    make

- Use QtCreator to build and compile (recomended for Windows).

To get help enter "/help" in the program.

Copyright © Andrey Ivanov, 2019.