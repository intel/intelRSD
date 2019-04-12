#!/usr/bin/env bash

sudo pip uninstall fpgaof_wheel

cd ./devtools/
sudo bash clean.sh
sudo bash create_fpgaof_wheel_package.sh

cd ..
cd FPGAoF_WHEEL_PACKAGE_0.85
sudo bash INSTALL.sh