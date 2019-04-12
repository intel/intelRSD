#!/usr/bin/env bash
VERSION=1.3.21
WHEEL_FILE_NAME=NVME_Wheel-${VERSION}-py2-none-any.whl

cd ../source

# create a wheel file
python setup.py bdist_wheel

cd ..
BUILD_DIRECTORY=NVME_WHEEL_PACKAGE_${VERSION}
mkdir ${BUILD_DIRECTORY}

# Copy .whl and utils
mv source/dist/${WHEEL_FILE_NAME} ${BUILD_DIRECTORY}
cp INSTALL.sh ${BUILD_DIRECTORY}
cp README.md ${BUILD_DIRECTORY}
cp requirements.txt ${BUILD_DIRECTORY}

# Make tar gz
tar -czvf ${BUILD_DIRECTORY}.tar.gz ${BUILD_DIRECTORY}
