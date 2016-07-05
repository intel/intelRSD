#!/usr/bin/env python
# -*- coding:utf-8 -*-

import urllib2
import json
import sys

def http_patch(puid):
    url='http://127.0.0.1:8090/redfish/v1/Chassis/Rack'
    values= {
        "Oem": {
            "Intel:RSA": {
                "RackPUID": puid
            }
        }
    }

    jdata = json.dumps(values)
    request = urllib2.Request(url, jdata)
    request.get_method = lambda:'PATCH'
    response = urllib2.urlopen(request)
    return response.getcode()

if len(sys.argv) != 2:
    print "Usage: %s your_rack_puid" % (sys.argv[0])
    sys.exit(0)

resp = http_patch(int(sys.argv[1]))
if resp == 200:
    print "Success to set rack puid"
else:
    print "Fail to set rack puid"
