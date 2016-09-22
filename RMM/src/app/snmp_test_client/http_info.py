import urllib
import urllib2, urlparse
import json
import sys
import socket
import time
import datetime

def http_post(url, values):
    try:
        #jdata = json.dumps(values)
        req = urllib2.Request(url, values)
        response = urllib2.urlopen(req)
        return response.read()
    except:
        return ''

def http_get(url):
    try:
        response = urllib2.urlopen(url)
        addr = socket.gethostbyname(urlparse.urlparse(response.geturl()).hostname)
        return [addr, response.read()]
    except:
        print 'exception.............'
        return ['','']
