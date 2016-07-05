#!/usr/bin/python

import os, sys
from http_info import http_post
http_post("http://10.239.56.74:8090/v1/rack/snmp_listener", '{"action":"unsubscribe","destination":"10.239.196.224","port":162}')
  