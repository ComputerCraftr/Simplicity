# Simplicity
Build Daemon and GUI Wallets with the information provided below.

Copyright (c) 2018 The Simplicity Developers
Distributed under the MIT/X11 software license, see the accompanying
file license.txt or http://www.opensource.org/licenses/mit-license.php.
This product includes software developed by the OpenSSL Project for use in
the OpenSSL Toolkit (http://www.openssl.org/).  This product includes
cryptographic software written by Eric Young (eay@cryptsoft.com) and UPnP
software written by Thomas Bernard.

## Unix Build Notes
### Prerequisites
```bash
sudo apt install build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils software-properties-common libboost-dev libboost-system-dev libboost-filesystem-dev libboost-program-options-dev libboost-thread-dev libgmp-dev libminiupnpc-dev libqrencode-dev

sudo add-apt-repository ppa:bitcoin/bitcoin && sudo apt update

sudo apt install libdb4.8-dev libdb4.8++-dev

git clone https://github.com/ComputerCraftr/Simplicity.git
```

### Build Qt wallet
```bash
sudo apt install qt5-default qt5-qmake qtbase5-dev-tools qttools5-dev-tools libqt5webkit5

cd Simplicity

qmake SPL-Qt.pro

make

strip Simplicity-Qt
```

### Build headless wallet

```bash
cd Simplicity/src

make -f makefile.unix

strip simplicityd
```

### Build headless wallet with script
```bash
 cd ~/
 
 wget https://raw.githubusercontent.com/ComputerCraftr/Simplicity/master/install_linux_wallet
 
 bash install_linux_wallet
 ```