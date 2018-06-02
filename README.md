# Simplicity

DO NOT BUILD WITH autogen.sh IT IS NOT FULLY IMPLEMENTED YET
==============================================================

Build Daemon and GUI Wallets with the information provided below.


UNIX BUILD NOTES
====================
Some notes on how to build Simplicity in Unix.

Copyright (c) 2018 The Simplicity Developers
Distributed under the MIT/X11 software license, see the accompanying
file license.txt or http://www.opensource.org/licenses/mit-license.php.
This product includes software developed by the OpenSSL Project for use in
the OpenSSL Toolkit (http://www.openssl.org/).  This product includes
cryptographic software written by Eric Young (eay@cryptsoft.com) and UPnP
software written by Thomas Bernard.


To Build Headless
-----------------
with script:
------------

 cd ~/
 
 wget https://raw.githubusercontent.com/ComputerCraftr/Simplicity/master/install_linux_wallet
 
 bash install_linux_wallet
 
 
manual install :
----------------

sudo apt install build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils

sudo apt install qt5-default qt5-qmake qtbase5-dev-tools qttools5-dev-tools libqt5webkit5 build-essential libboost-dev libboost-system-dev libboost-filesystem-dev libboost-program-options-dev libboost-thread-dev libssl-dev libgmp-dev libminiupnpc-dev

sudo apt install software-properties-common

sudo add-apt-repository ppa:bitcoin/bitcoin

sudo apt update

sudo apt install libdb4.8-dev libdb4.8++-dev

sudo apt install libqrencode-dev

git clone https://github.com/ComputerCraftr/Simplicity.git

cd Simplicity/src/secp256k1

chmod +x autogen.sh

./autogen.sh

./configure --enable-module-recovery

make && sudo make install

cd ../leveldb/

chmod +x build_detect_platform

cd ..

make -f makefile.unix -jnumofcoreshere

strip simplicityd

LD_LIBRARY_PATH=/usr/local/lib

export LD_LIBRARY_PATH


To Build Qt Wallet
------------------

sudo apt install build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils

sudo apt install qt5-default qt5-qmake qtbase5-dev-tools qttools5-dev-tools libqt5webkit5 build-essential libboost-dev libboost-system-dev libboost-filesystem-dev libboost-program-options-dev libboost-thread-dev libssl-dev libminiupnpc-dev 

sudo apt install software-properties-common

sudo add-apt-repository ppa:bitcoin/bitcoin

sudo apt update

sudo apt install libdb4.8-dev libdb4.8++-dev

sudo apt-get install libqrencode-dev

git clone https://github.com/ComputerCraftr/Simplicity.git

cd Simplicity/src/secp256k1

chmod +x autogen.sh

./autogen.sh

./configure --enable-module-recovery

sudo make && sudo make install

cd ../leveldb/

chmod +x build_detect_platform

cd ../..

qmake SPL-Qt.pro

make -jnumofcoreshere
