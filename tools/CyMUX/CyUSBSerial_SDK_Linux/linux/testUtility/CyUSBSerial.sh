#!/bin/bash
pid=`pidof CyUSBSerialTestUtility`

if [ "$pid" ]; then
    kill -s SIGUSR1 $pid
fi

