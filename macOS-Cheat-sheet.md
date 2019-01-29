eDystem Mac cheat sheet - tested macOS Sierra 10.12.6
=====================================

// DEPENDANCIES

/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

brew update && brew upgrade `brew outdated`

brew install openssl

brew install autoconf

brew install pkg-config

brew install boost

brew install curl

brew install jsoncpp

brew install g++

export LDFLAGS="-L/usr/local/opt/openssl/lib -L/usr/local/opt/jsoncpp/lib"
export CPPFLAGS="-I/usr/local/opt/openssl/include -I/usr/local/opt/jsoncpp/include"
export PKG_CONFIG_PATH="/usr/local/opt/openssl/lib/pkgconfig /usr/local/opt/jsoncpp/lib/pkgconfig"

// WALLET GUIDE

brew install automake berkeley-db4 libtool boost miniupnpc openssl pkg-config protobuf python qt libevent qrencode

brew install librsvg

Install / run / debug QT:

1. Make sure you installed everything through homebrew mentioned above
2. Do a proper ./configure --with-gui=qt5 --enable-debug
3. In Qt Creator do "New Project" -> Import Project -> Import Existing Project
4. Enter "dystem-qt" as project name, enter src/qt as location
5. Leave the file selection as it is
6. Confirm the "summary page"
7. In the "Projects" tab select "Manage Kits..."
8. Select the default "Desktop" kit and select "Clang (x86 64bit in /usr/bin)" as compiler
9. Select LLDB as debugger (you might need to set the path to your installtion)
10. Go to the Projects build section and remove the clean and make all steps
11. Go to the Run target and set the Executable to the path of the executable "<path to project>/Dystem/src/qt/./dystem-qt"
12. Start debugging with Qt Creator by Debug -> Start Debugging
