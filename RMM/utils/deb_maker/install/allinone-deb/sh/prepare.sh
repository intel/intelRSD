#!/bin/bash
CUR_PATH=$(cd "$(dirname "$0")"; pwd)
PROJ_PATH=$CUR_PATH/../../../..

function del_file()
{
    for del in $1*
    do
        if [ -f $del ]
        then
            rm -rf $del
        fi
    done
}

#1
ITEM_PATH=$CUR_PATH/../rmm-allinone

mkdir -p $ITEM_PATH/usr/local/bin
mkdir -p $ITEM_PATH/usr/local/lib
mkdir -p $ITEM_PATH/etc
mkdir -p $ITEM_PATH/etc/rmm/
mkdir -p $ITEM_PATH/etc/udev/rules.d
mkdir -p $ITEM_PATH/var/rmm/
mkdir -p $ITEM_PATH/var/rmm/redfish
del_file $ITEM_PATH/usr/local/bin/
del_file $ITEM_PATH/usr/local/lib/
del_file $ITEM_PATH/etc/rmm/
del_file $ITEM_PATH/etc/udev/rules.d/
del_file $ITEM_PATH/etc/
del_file $ITEM_PATH/var/rmm/redfish
del_file $ITEM_PATH/var/rmm/

#cp $PROJ_PATH/build/lib/*.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_wrap.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_json.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_cfg.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_redfish.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_log.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_utils.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_securec.so* $ITEM_PATH/usr/local/lib
#cp $PROJ_PATH/build/lib/libcurl.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_jsonrpc.so* $ITEM_PATH/usr/local/lib
#cp $PROJ_PATH/build/lib/libssl.so* $ITEM_PATH/usr/local/lib
#cp $PROJ_PATH/build/lib/libcrypto.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/utils/redfish/MR.json $ITEM_PATH/var/rmm/redfish
cp $PROJ_PATH/utils/cfg/rmm.cfg $ITEM_PATH/etc/rmm

cp $PROJ_PATH/build/bin/Memdbd $ITEM_PATH/usr/local/bin/
cp $PROJ_PATH/build/bin/IPMIModule $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/bin/Registerd $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/bin/Assetd $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/bin/AssetModule $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/bin/RMMLogd $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/bin/Upgraded $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/utils/cfg/80-persistent-tty.rules $ITEM_PATH/etc/udev/rules.d/
cp -r $PROJ_PATH/src/core/restd/web $ITEM_PATH/var/rmm/

cp $PROJ_PATH/build/bin/Restd $ITEM_PATH/usr/local/bin/
cp $PROJ_PATH/build/bin/Redfishd $ITEM_PATH/usr/local/bin/
cp $PROJ_PATH/build/bin/cm_reset $ITEM_PATH/usr/local/bin/


cp $PROJ_PATH/build/bin/dumpmemdb $ITEM_PATH/usr/local/bin

$CUR_PATH/modify_ver.sh $ITEM_PATH
