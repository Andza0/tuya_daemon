#!/bin/sh

./clean.sh

mkdir build
cmake -S . -B build
cd build
make

sudo cp cdaemon /usr/local/bin

cd ..
sudo cp cdaemon.service /etc/systemd/system/