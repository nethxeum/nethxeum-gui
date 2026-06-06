# Nethxeum GUI

Copyright (c) 2026, Nethxeum

## Table of Contents
  * [Development resources](#development-resources)
  * [Vulnerability response](#vulnerability-response)
  * [Introduction](#introduction)
  * [About this project](#about-this-project)
  * [License](#license)
  * [Compiling the Nethxeum GUI from source](#compiling-the-nethxeum-gui-from-source)
    + [Building Reproducible Windows static binaries with Docker (any OS)](#building-reproducible-windows-static-binaries-with-docker-any-os)
    + [Building on Windows](#building-on-windows)

## Development resources

- Github GUI: [https://github.com/nethxeum/nethxeum-gui](https://github.com/nethxeum/nethxeum-gui)
- Github Daemon: [https://github.com/nethxeum/nethxeum](https://github.com/nethxeum/nethxeum)
- Web: [https://nethxeum.com](https://nethxeum.com)
- Mail: [contact@nethxeum.com](contact@nethxeum.com)

## Vulnerability response

Please open a GitHub issue at [https://github.com/nethxeum/nethxeum-gui/issues](https://github.com/nethxeum/nethxeum-gui/issues) to report any vulnerability or security concern.

## Introduction

Nethxeum (NTU) is a private, secure, untraceable, decentralised digital currency built as a fork of Monero with Bitcoin-like block maturity (100 blocks).

**Privacy:** Nethxeum uses a cryptographically sound system to allow you to send and receive funds without your transactions being easily revealed on the blockchain. This ensures that your purchases, receipts, and all transfers remain absolutely private by default.

**Security:** Using the power of a distributed peer-to-peer consensus network, every transaction on the network is cryptographically secured. Individual wallets have a 25 word mnemonic seed that is only displayed once, and can be written down to backup the wallet. Wallet files are encrypted with a passphrase to ensure they are useless if stolen.

**Untraceability:** By taking advantage of ring signatures, Nethxeum ensures that transactions are not only untraceable, but have an optional measure of ambiguity that ensures that transactions cannot easily be tied back to an individual user or computer.

**Block maturity:** Mined outputs require 100 block confirmations before they become spendable, consistent with Bitcoin's approach to coinbase maturity.

## About this project

This is the GUI for the [core Nethxeum implementation](https://github.com/nethxeum/nethxeum). It is open source and completely free to use without restrictions, except for those specified in the license agreement below.

The `master` branch is the active development branch. For stability, use a tagged release when available.

## License

See [LICENSE](LICENSE).

---

## Compiling the Nethxeum GUI from source

*Note*: Qt 5.12 is the minimum version required to build the GUI.

*Note*: Official GUI releases use `nethxeum-wallet-gui` from this process alongside the supporting binaries (`nethxeumd`, etc.) built from the [Nethxeum daemon repository](https://github.com/nethxeum/nethxeum).

### Building Reproducible Windows static binaries with Docker (any OS)

1. Install Docker [https://docs.docker.com/engine/install/](https://docs.docker.com/engine/install/)
2. Clone the repository
   ```
   git clone --branch master --recursive https://github.com/nethxeum/nethxeum-gui.git
   ```
3. Prepare build environment
   ```
   cd nethxeum-gui
   docker build --tag nethxeum:build-env-windows --build-arg THREADS=4 --file Dockerfile.windows .
   ```
   \* `4` - number of CPU threads to use

4. Build
   ```
   docker run --rm -it -v <NETHXEUM_GUI_DIR_FULL_PATH>:/nethxeum-gui -w /nethxeum-gui nethxeum:build-env-windows sh -c 'make depends root=/depends target=x86_64-w64-mingw32 tag=win-x64 -j4'
   ```
   \* `<NETHXEUM_GUI_DIR_FULL_PATH>` - absolute path to `nethxeum-gui` directory  
   \* `4` - number of CPU threads to use

5. The Nethxeum GUI Windows static binaries will be placed in `nethxeum-gui/build/x86_64-w64-mingw32/release/bin`

### Building Reproducible Linux static binaries with Docker (any OS)

1. Install Docker [https://docs.docker.com/engine/install/](https://docs.docker.com/engine/install/)
2. Clone the repository
   ```
   git clone --branch master --recursive https://github.com/nethxeum/nethxeum-gui.git
   ```
3. Prepare build environment
   ```
   cd nethxeum-gui
   docker build --tag nethxeum:build-env-linux --build-arg THREADS=4 --file Dockerfile.linux .
   ```
4. Build
   ```
   docker run --rm -it -v <NETHXEUM_GUI_DIR_FULL_PATH>:/nethxeum-gui -w /nethxeum-gui nethxeum:build-env-linux sh -c 'make release-static -j4'
   ```
5. The binary will be placed in `nethxeum-gui/build/release/bin`

*Note*: `nethxeumd` must be built separately from the [daemon repository](https://github.com/nethxeum/nethxeum).

### Building on Linux

(Tested on Ubuntu 18.04 x64 and above)

1. Install Nethxeum dependencies

   For Debian/Ubuntu:
   ```
   sudo apt install build-essential cmake miniupnpc libunbound-dev graphviz doxygen \
   libunwind8-dev pkg-config libssl-dev libzmq3-dev libsodium-dev libhidapi-dev \
   libnorm-dev libusb-1.0-0-dev libpgm-dev libprotobuf-dev protobuf-compiler \
   libgcrypt20-dev libboost-chrono-dev libboost-date-time-dev libboost-filesystem-dev \
   libboost-locale-dev libboost-program-options-dev libboost-regex-dev \
   libboost-serialization-dev libboost-system-dev libboost-thread-dev
   ```

2. Install Qt:
   ```
   sudo apt install qtbase5-dev qtdeclarative5-dev qml-module-qtqml-models2 \
   qml-module-qtquick-controls qml-module-qtquick-controls2 qml-module-qtquick-dialogs \
   qml-module-qtquick-xmllistmodel qml-module-qt-labs-settings qml-module-qt-labs-platform \
   qml-module-qt-labs-folderlistmodel qttools5-dev-tools qml-module-qtquick-templates2 \
   libqt5svg5-dev
   ```

3. Clone repository
   ```
   git clone --recursive https://github.com/nethxeum/nethxeum-gui.git
   cd nethxeum-gui
   ```

4. Build
   ```
   make release -j4
   ```

The executable can be found in `build/release/bin`.

### Building on OS X

1. Install Xcode from the App Store

2. Install [homebrew](http://brew.sh/)

3. Install dependencies:
   ```
   brew install cmake pkg-config openssl boost unbound hidapi zmq libpgm libsodium miniupnpc expat libunwind-headers protobuf libgcrypt
   ```

4. Install Qt:
   ```
   brew install qt5
   ```

5. Clone repository
   ```
   git clone --recursive https://github.com/nethxeum/nethxeum-gui.git
   cd nethxeum-gui
   ```

6. Build
   ```
   make release -j4
   ```

The executable can be found in `build/release/bin`.

### Building on Windows

The Nethxeum GUI on Windows is 64-bit only.

1. Install [MSYS2](https://www.msys2.org/) and update all packages

2. Open the *MSYS2 MinGW 64-bit* shell

3. Install dependencies:
   ```
   pacman -S mingw-w64-x86_64-toolchain make mingw-w64-x86_64-cmake \
   mingw-w64-x86_64-boost mingw-w64-x86_64-openssl mingw-w64-x86_64-zeromq \
   mingw-w64-x86_64-libsodium mingw-w64-x86_64-hidapi mingw-w64-x86_64-protobuf-c \
   mingw-w64-x86_64-libusb mingw-w64-x86_64-libgcrypt mingw-w64-x86_64-unbound \
   mingw-w64-x86_64-pcre mingw-w64-x86_64-angleproject
   ```

4. Install Qt5:
   ```
   pacman -S mingw-w64-x86_64-qt5
   ```

5. Install git:
   ```
   pacman -S git
   ```

6. Clone repository:
   ```
   git clone --recursive https://github.com/nethxeum/nethxeum-gui.git
   cd nethxeum-gui
   ```

7. Build:
   ```
   make release-win64 -j4
   cd build/release
   make deploy
   ```

The executable can be found in the `.\bin` directory.
