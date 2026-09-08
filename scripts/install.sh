#!/bin/sh

./clean.sh

mkdir build
cmake -S . -B build
cd build
make

sudo cp tuyad /usr/local/bin

cd ..
sudo cp tuyad.service /etc/systemd/system/