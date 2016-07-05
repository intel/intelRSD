#! /bin/bash

CUR_DIR=$PWD
TOP_DIR="$PWD/../"
BUILD_DIR="$PWD/../build/"
TOOLCHAIN_DIR="/opt/cc/bin"
BUILD_FLAG="$PWD/../build/build_arm"

/bin/mkdir -p ${BUILD_DIR}

if [ "$1" = "clean" ];then
	rm -rf ${BUILD_DIR}/* && exit -1
fi

if [ "$1" = "arm" ];then
	if [ -e $TOOLCHAIN_DIR ]; then
		echo "toolchain found in opt"
	else
		sudo tar -xvf tools/cc.tar.gz -C /opt || exit -1
	fi

	echo $PATH|grep -q -w "/opt/cc/bin" || export PATH=$PATH:/opt/cc/bin

	if [ -e $BUILD_FLAG ]; then
		cd ${BUILD_DIR} && export CC=/opt/cc/bin/arm-linux-gcc && cmake .. -DARM=yes && make && make install || exit -1
	else
		cd ${BUILD_DIR} && rm -rf * && touch $BUILD_FLAG && export CC=/opt/cc/bin/arm-linux-gcc && cmake .. -DARM=yes && make && make install || exit -1
	fi
else
	if [ -e $BUILD_FLAG ]; then
		cd ${BUILD_DIR} && rm -rf * && export CC= && cmake ..  && make && make install || exit -1
	else
		cd ${BUILD_DIR} && cmake ..  && export CC= && make && make install || exit -1
	fi
	rm -f $BUILD_FLAG
fi

SRC_DIR="$TOP_DIR/src"
DEB_TOOL_DIR="$TOP_DIR/utils/deb_maker"
DEB_PACKAGE_DIR="$TOP_DIR/build/install/multi-deb/bin"

RMM_RELEASE_DIR="$TOP_DIR/build/release"
if [ -e ${RMM_RELEASE_DIR} ]; then
	/bin/rm -rf ${RMM_RELEASE_DIR}
fi
/bin/mkdir -p ${RMM_RELEASE_DIR}

DEB_TOOL="$DEB_TOOL_DIR/gener_deb.sh"
VERSION=$(cat $SRC_DIR/VERSION)


# generate deb packages
$DEB_TOOL

cd $DEB_PACKAGE_DIR
#time_stamp=`date "+%Y%m%d%H%M%S"`
#deb_tar_name="rmm-$time_stamp.tar.gz"

deb_tar_name="rmm-${VERSION}.tar.gz"
deb_bin_name="rmm-${VERSION}.bin"
deb_signed_bin_name="rmm-${VERSION}.signed.bin"

tar -zcf $RMM_RELEASE_DIR/$deb_bin_name rmm*.deb
(cd $TOP_DIR/utils/tools/sign_image/ && make && ./img_sign $RMM_RELEASE_DIR/$deb_bin_name $RMM_RELEASE_DIR/$deb_signed_bin_name)
RELEASE_PACKAGE_DIR="rmm-${VERSION}"
if [ -e $RELEASE_PACKAGE_DIR ]; then
	/bin/rm -rf ${RELEASE_PACKAGE_DIR}/*
else
	/bin/mkdir -p ${RELEASE_PACKAGE_DIR}
fi

cp -a rmm*.deb ${RELEASE_PACKAGE_DIR}
tar -zcf $RMM_RELEASE_DIR/$deb_tar_name ${RELEASE_PACKAGE_DIR}
