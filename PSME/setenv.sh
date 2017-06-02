#!/usr/bin/env bash

# Copyright (c) 2015-2017 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Fail script when occur pipeline or process error

function cleanup {
    set +euo pipefail
}

trap cleanup RETURN EXIT ERR

set -euo pipefail

CURRENT_DIR=$(pwd)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

TOOLS_DIR=${SCRIPT_DIR}/tools
DOWNLOADS_DIR=${TOOLS_DIR}/downloads

LOG_FILE=${TOOLS_DIR}/log_output

TOOLCHAIN_NAME=arm-linux-gnueabihf
TOOLCHAIN_VERSION=2014.11
TOOLCHAIN_ARCH=x86_64
TOOLCHAIN_COMPILER=gcc-linaro-4.9
TOOLCHAIN_URL=http://releases.linaro.org/14.11/components/toolchain/binaries/${TOOLCHAIN_NAME}
TOOLCHAIN_PACKAGE=${TOOLCHAIN_COMPILER}-${TOOLCHAIN_VERSION}-${TOOLCHAIN_ARCH}_${TOOLCHAIN_NAME}
TOOLCHAIN_ARCHIVE=${TOOLCHAIN_PACKAGE}.tar.xz
TOOLCHAIN_ARCHIVE_SIGNATURE=${TOOLCHAIN_ARCHIVE}.asc
TOOLCHAIN_PREFIX=${TOOLCHAIN_NAME}-
TOOLCHAIN_DIR=${TOOLS_DIR}/${TOOLCHAIN_PACKAGE}
TOOLCHAIN_PATH=${TOOLCHAIN_DIR}/bin

UBOOT_NAME=u-boot
UBOOT_GIT_URL=git://git.denx.de/u-boot.git
UBOOT_GIT_BRANCH=v2015.04-rc2
UBOOT_PATCH_URL=https://raw.githubusercontent.com/eewiki/u-boot-patches/master/v2015.04-rc2
UBOOT_PATCH_NAME=0001-sama5d3xek-uEnv.txt-bootz-n-fixes.patch
UBOOT_DIR=${TOOLS_DIR}/${UBOOT_NAME}

LINUX_NAME=linux-armv7-devel
LINUX_GIT_URL=git://github.com/RobertCNelson/armv7_devel.git
LINUX_GIT_BRANCH=v3.14.x-sama5-armv7
LINUX_DIR=${TOOLS_DIR}/${LINUX_NAME}

ROOTFS_PACKAGE=debian-7.8-bare-armhf-2015-01-20
ROOTFS_ARCHIVE=${ROOTFS_PACKAGE}.tar.xz
ROOTFS_ARCHIVE_MD5=cdc78a9caf9ad1e8fae803709d19af27
ROOTFS_URL=https://rcn-ee.net/rootfs/eewiki/barefs

> $LOG_FILE
echo "Creating log file at $(date)" >> $LOG_FILE

if [ ! -d $TOOLS_DIR ]; then
    echo "Creating tools directory: $TOOLS_DIR"
    mkdir -p $TOOLS_DIR
fi

if [ ! -d $DOWNLOADS_DIR ]; then
    echo "Creating downloads directory: $DOWNLOADS_DIR"
    mkdir -p $DOWNLOADS_DIR
fi

cd $DOWNLOADS_DIR

if [ ! -f $TOOLCHAIN_ARCHIVE ]; then
    echo "Downloading toolchain archive..."
    wget $TOOLCHAIN_URL/$TOOLCHAIN_ARCHIVE 1>> $LOG_FILE
fi

if [ ! -f $TOOLCHAIN_ARCHIVE_SIGNATURE ]; then
    echo "Downloading toolchain archive signature..."
    wget $TOOLCHAIN_URL/$TOOLCHAIN_ARCHIVE_SIGNATURE 1>> $LOG_FILE
fi

echo "Checking toolchain archive..."
md5sum --check $TOOLCHAIN_ARCHIVE_SIGNATURE 1>> $LOG_FILE

if [ ! -f $ROOTFS_ARCHIVE ]; then
    echo "Downloading rootfs..."
    wget $ROOTFS_URL/$ROOTFS_ARCHIVE 1>> $LOG_FILE
fi

echo "$ROOTFS_ARCHIVE_MD5 $ROOTFS_ARCHIVE" > ${ROOTFS_ARCHIVE}.md5
echo "Checking root file system..."
md5sum --check ${ROOTFS_ARCHIVE}.md5 1>> $LOG_FILE

cd $TOOLS_DIR

if [ ! -d $TOOLCHAIN_PACKAGE ]; then
    echo "Unpacking downloaded toolchain archive..."
    tar --verbose --extract --file=$DOWNLOADS_DIR/$TOOLCHAIN_ARCHIVE 1>> $LOG_FILE
fi

if [[ $PATH != *$TOOLCHAIN_PATH* ]]; then
    echo "Setting toolchain path..."
    export PATH=$PATH:$TOOLCHAIN_PATH
fi

echo "Testing toolchain compiler..."
${TOOLCHAIN_PREFIX}gcc --version 1>> $LOG_FILE

if [ ! -d $ROOTFS_PACKAGE ]; then
    echo "Unpacking downloaded root file system..."
    tar --verbose --extract --file=$DOWNLOADS_DIR/$ROOTFS_ARCHIVE 1>> $LOG_FILE
fi

if [ ! -d $UBOOT_DIR ]; then
    git clone $UBOOT_GIT_URL $UBOOT_DIR

    cd $UBOOT_DIR

    git reset --hard $UBOOT_GIT_BRANCH
    git checkout -B $UBOOT_GIT_BRANCH-dev

    if [ ! -f  $UBOOT_PATCH_NAME ]; then
        wget $UBOOT_PATCH_URL/$UBOOT_PATCH_NAME
    fi

    patch -p1 < $UBOOT_PATCH_NAME

    if [ ! -f u-boot.bin ]; then
        make ARCH=arm CROSS_COMPILE=$TOOLCHAIN_PREFIX distclean
        make ARCH=arm CROSS_COMPILE=$TOOLCHAIN_PREFIX sama5d3xek_mmc_defconfig
        make ARCH=arm CROSS_COMPILE=$TOOLCHAIN_PREFIX
    fi
fi

if [ ! -d $LINUX_DIR ]; then
    echo "Cloning linux kernel..."
    git clone $LINUX_GIT_URL $LINUX_DIR

    cd $LINUX_DIR

    git reset --hard origin/$LINUX_GIT_BRANCH
    git checkout -B $LINUX_GIT_BRANCH-dev

    ./build_kernel.sh
fi

cd $CURRENT_DIR
