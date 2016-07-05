#!/bin/bash
CUR_PATH=$(cd "$(dirname "$0")"; pwd)
function changemod()
{
    chmod +x $1/DEBIAN/post*
}
declare -a arr=(
    "rmm-allinone")
    
for i in "${arr[@]}"
do
   changemod $CUR_PATH/../$i
done
chmod 777 $CUR_PATH/*.sh
