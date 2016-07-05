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
ITEM_PATH=$CUR_PATH/../rmm-base

mkdir -p $ITEM_PATH/usr/local/bin
mkdir -p $ITEM_PATH/usr/local/sbin
mkdir -p $ITEM_PATH/usr/local/lib
mkdir -p $ITEM_PATH/etc
mkdir -p $ITEM_PATH/etc/rmm/
mkdir -p $ITEM_PATH/etc/udev/rules.d
mkdir -p $ITEM_PATH/etc/stunnel
mkdir -p $ITEM_PATH/var/rmm/
mkdir -p $ITEM_PATH/var/rmm/redfish
del_file $ITEM_PATH/usr/local/bin/
del_file $ITEM_PATH/usr/local/lib/
del_file $ITEM_PATH/etc/rmm
del_file $ITEM_PATH/etc/udev/rules.d/
del_file $ITEM_PATH/etc/
del_file $ITEM_PATH/var/rmm/redfish
del_file $ITEM_PATH/etc/stunnel

#cp $PROJ_PATH/build/lib/*.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/libptassdk.so $ITEM_PATH/usr/local/lib > /dev/null 2>&1
cp $PROJ_PATH/build/lib/librmm_wrap.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_json.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_cfg.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_encrypter.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_redfish.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_utils.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_securec.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_log.so* $ITEM_PATH/usr/local/lib
#cp $PROJ_PATH/build/lib/libcurl.so* $ITEM_PATH/usr/local/lib
#cp $PROJ_PATH/build/lib/libuuid.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_init.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_jsonrpc.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/lib/librmm_jsonrpcapi.so* $ITEM_PATH/usr/local/lib
#cp $PROJ_PATH/build/lib/libssl.so* $ITEM_PATH/usr/local/lib
#cp $PROJ_PATH/build/lib/libcrypto.so* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/utils/redfish/MR.json $ITEM_PATH/var/rmm/redfish
cp $PROJ_PATH/build/utils/cfg/rmm.cfg $ITEM_PATH/etc/rmm
cp $PROJ_PATH/build/utils/cfg/pwm_rpm.cfg $ITEM_PATH/etc/rmm
cp $PROJ_PATH/build/utils/encrypt_text/keyfile $ITEM_PATH/etc/rmm

cp $PROJ_PATH/build/bin/ptasd $ITEM_PATH/usr/local/bin/
cp $PROJ_PATH/build/bin/OEM_coe_file $ITEM_PATH/var/rmm/
cp $PROJ_PATH/build/bin/Memdbd $ITEM_PATH/usr/local/bin/
cp $PROJ_PATH/build/bin/IPMIModule $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/bin/Assetd $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/bin/AssetModule $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/bin/Registerd $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/bin/RMMLogd $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/bin/Upgraded $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/bin/SNMPSubagentd $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/bin/cm_reset $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/bin/snmpd $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/bin/stunnel $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/bin/rmm_post_install $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/bin/rmm_super $ITEM_PATH/usr/local/bin
cp $PROJ_PATH/build/utils/cfg/80-persistent-tty.rules $ITEM_PATH/etc/udev/rules.d/
cp -r $PROJ_PATH/src/core/restd/web $ITEM_PATH/var/rmm/
cp $PROJ_PATH/src/core/restd/stunnel.conf $ITEM_PATH/etc/stunnel/
cp $PROJ_PATH/src/core/restd/stunnel.pem $ITEM_PATH/etc/stunnel/
cp $PROJ_PATH/src/core/restd/key.pub $ITEM_PATH/etc/stunnel/
#cp $PROJ_PATH/src/core/restd/podm.cert $ITEM_PATH/etc/rmm/

#cp $PROJ_PATH/build/bin/in.tftpd $ITEM_PATH/usr/local/sbin/
$CUR_PATH/modify_ver.sh $ITEM_PATH

#2
ITEM_PATH=$CUR_PATH/../rmm-api

mkdir -p $ITEM_PATH/usr/local/bin
del_file $ITEM_PATH/usr/local/bin/
cp $PROJ_PATH/build/bin/Restd $ITEM_PATH/usr/local/bin/
cp $PROJ_PATH/build/bin/Redfishd $ITEM_PATH/usr/local/bin/
$CUR_PATH/modify_ver.sh $ITEM_PATH


#3
ITEM_PATH=$CUR_PATH/../rmm-consolecontrol

mkdir -p $ITEM_PATH/usr/local/bin
del_file $ITEM_PATH/usr/local/bin/
cp $PROJ_PATH/build/utils/tools/set_puid.py $ITEM_PATH/usr/local/bin

cp $PROJ_PATH/build/bin/dumpmemdb $ITEM_PATH/usr/local/bin
$CUR_PATH/modify_ver.sh $ITEM_PATH

#4
ITEM_PATH=$CUR_PATH/../rmm-all
$CUR_PATH/modify_ver.sh $ITEM_PATH

#5
ITEM_PATH=$CUR_PATH/../net-snmp

mkdir -p $ITEM_PATH/usr/local/bin
del_file $ITEM_PATH/usr/local/bin/

mkdir -p $ITEM_PATH/etc/snmp
del_file $ITEM_PATH/etc/snmp/

mkdir -p $ITEM_PATH/usr/local/lib
del_file $ITEM_PATH/usr/local/lib/

#mkdir -p $ITEM_PATH/usr/local/share/snmp/mibs
#del_file $ITEM_PATH/usr/local/share/snmp/mibs

cp $PROJ_PATH/build/lib/libnet* $ITEM_PATH/usr/local/lib
cp $PROJ_PATH/build/bin/snmp* $ITEM_PATH/usr/local/bin/
#copy snmp config file
cp $PROJ_PATH/src/core/snmp_subagentd/config/*.conf $ITEM_PATH/etc/snmp/
#copy mibs file
#cp $PROJ_PATH/src/core/snmp_subagentd/*.txt $ITEM_PATH/usr/local/share/snmp/mibs/
#cp $PROJ_PATH/src/dep/snmp/net-snmp-5.7.3/mibs/*.txt $ITEM_PATH/usr/local/share/snmp/mibs/
#add env 
#export MIBDIRS="/usr/local/share/snmp/mibs"
$CUR_PATH/modify_ver.sh $ITEM_PATH
