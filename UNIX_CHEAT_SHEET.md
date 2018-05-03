Dystem MN cheat sheet
=====================================

// DEPENDANCIES

sudo su

add-apt-repository ppa:bitcoin/bitcoin

apt update && sudo apt upgrade -y

apt install -y build-essential libtool autotools-dev automake pkg-config libssl-dev autoconf

apt install -y pkg-config libssl-dev libgmp3-dev libevent-dev bsdmainutils

apt install -y libevent-dev bsdmainutils libboost-all-dev libdb4.8-dev libdb4.8++-dev nano git

apt install automake

apt install libtool

apt install g++

//SWAP FILE

dd if=/dev/zero of=/swapfile1 bs=1024 count=8388608

chown root:root /swapfile1

chmod 0600 /swapfile1

mkswap /swapfile1

swapon /swapfile1

vi /etc/fstab

Append the following line:
/swapfile1 none swap sw 0 0

reboot

free -m

SHOULD LOOK LIKE:

total        used        free      shared  buff/cache   available
Mem:            992          43         755           3         193         801
Swap:          8191           0        8191

//COMPILE

git clone https://github.com/Dystem/dystem-core.git

cd Dystem

./autogen.sh
./configure
make
cd /src
make install
