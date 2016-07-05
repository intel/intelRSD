#!/usr/bin/python

import os, sys

'''
from lstndest import lstndest
from color import color
from http_info import http_post
from http_info import http_get
'''

from event_subscribe.lstndest import lstndest
from event_subscribe.color import color
from event_subscribe.http_info import http_post
from event_subscribe.http_info import http_get

lstnDestData = lstndest()
#Error Message
paramsErr               = "input params error, just RMM ip address require."
selectErr               = "Select ID error, Please select again."

targetHeaderMsg         = "Please select the subscribe url:"
eventTypesHeaderMsg     = "Please the event type:"
subscribeTypeHeaderMsg  = "Please select the subscribe type:"

selectPromptMsg         = "default is 0 and type 'quit' to exit: "

g_item_disc = [ ['{pzone_id}',              'Please select power zone id:',         1, 3],
                ['{tzone_id}',              'Please select thermal zone id:',       1, 3],
                ['{psu_id}',                'Please select PSU id:',                1, 7], 
                ['{fan_id}',                'Please select fan id:',                1, 7],
                ['{mbp_id}',                'Please select mbp id:',                1, 3]]

g_remoteip_key = "remoteip"
g_subcribe_ip = "subcribe_ip"
g_check_rmm_url = "http://%s/v1/rack"

g_rfEvents = ["StatusChange","ResourceUpdated",'ResourceAdded', 'ResourceRemoved', "Alert"]

#pzone=      ['http://remoteip/v1/rack/powerzones/{pzone_id}/EventService',                    g_rfEvents,  'http://subcribe_ip/v1/rack/power' ]
#psu =       ['http://remoteip/v1/rack/powerzones/{pzone_id}/psus/{psu_id}/EventService',      g_rfEvents,  'http://subcribe_ip/v1/rack/power' ]
#tzone=      ['http://remoteip/v1/rack/thermalzones/{tzone_id}/EventService',                  g_rfEvents,  'http://subcribe_ip/v1/rack/thermal' ]
#fan =       ['http://remoteip/v1/rack/thermalzones/{tzone_id}/fans/{fan_id}/EventService',    g_rfEvents,  'http://subcribe_ip/v1/rack/thermal' ]
#drawer=     ['http://remoteip/v1/rack/drawers/{sys_id}/EventService',                         g_rfEvents,  'http://subcribe_ip/v1/rack/drawer' ]
#mbp=        ['http://remoteip/v1/rack/mbps/{mbp_id}/EventService',                            g_rfEvents,  'http://subcribe_ip/v1/rack/mbp' ]
rack=       ['http://remoteip/v1/rack/EventService',                                          g_rfEvents,  'http://subcribe_ip/v1/rack/rack']

g_subcribe_items = [rack]#, mbp, drawer, pzone, psu, tzone, fan]

g_check_subscribe_items = ['Subscribe', 'Unsubscribe']

def printErrMsg(msg):
    print color.RED + color.BOLD + msg

def getSelectInfo(items, msg):
    id = 0
    print color.BLUE + color.BOLD  + msg
    while True:
        for item in items:  
            print color.BOLD + color.GREEN +('%d') % (id) + color.END + (' -  %s')%(item)
            id = id + 1
        try:
            content = raw_input(color.CYAN + selectPromptMsg + color.END)
            if content == 'quit':
                exit(0)

            if content == '':
                selectid = 0
            else:
                selectid = int(content)
            if selectid < id :
                return items[selectid]
            else:
                printErrMsg(selectErr)
        except KeyboardInterrupt:
            exit(0)
        except ValueError:
            printErrMsg(selectErr)

def getNumSelectInfo(msg, beg, end):
    print color.BLUE + color.BOLD  + msg + color.BOLD + color.GREEN +('  [%d - %d)')%(beg, end)
    while True:
        try:
            content = raw_input(color.CYAN + selectPromptMsg + color.END)
            if content == 'quit':
                exit(0)
    
            if content == '':
                selectid = 0
            else:
                selectid = int(content)
            if selectid >= beg and selectid < end :
                return selectid
            else:
                printErrMsg(selectErr)
        except KeyboardInterrupt:
            exit(0)
        except ValueError:
            printErrMsg(selectErr)

def setEvtType(types):
    evttype = getSelectInfo(types, eventTypesHeaderMsg)
    lstnDestData.setEvtType(evttype)

def setTarget(data):
    targetList = []
    for item in data:  
        targetList.append(item[0])
    target = getSelectInfo(targetList, targetHeaderMsg)
    return targetList.index(target)
    
def setSubscribeType(types):
    type = getSelectInfo(types, subscribeTypeHeaderMsg)
    lstnDestData.setReqType(types.index(type) + 1)
    
def setDest(dest):
    lstnDestData.setDest(dest)
    
def checkRMMAvailable(ipaddress):
    #requesturl = ('http://%s/seventservice') % (ipaddress)
    print "------------------Check RMM available----------------------------------"
    requesturl = (g_check_rmm_url) % (ipaddress)
    print "test url:     " + requesturl
    ret = http_get(requesturl)
    print "local ip:     " + ret[0]
    print "return str:   " + ret[1]
    print "------------------Check RMM available end ----------------------------"
    if len(ret[1]) > 0:
        return ret[0]
    exit(0)

def changeIP(url, rstr,ip):
    return url.replace(rstr, ip)

def changeDefData(lip, rip):
    id = 0
    for item in g_subcribe_items:
        g_subcribe_items[id][0] = changeIP(g_subcribe_items[id][0], g_remoteip_key, rip)
        g_subcribe_items[id][2] = changeIP(g_subcribe_items[id][2], g_subcribe_ip, lip)
        id = id + 1
    return


def showSelectInfoByUrl(url):
    for item in g_item_disc:
        if item[0] in url:
            id = getNumSelectInfo(item[1], item[2], item[3])
            url = url.replace(item[0], ('%d')%(id));
    lstnDestData.setTarget(url)
    
def run():
    id = setTarget(g_subcribe_items)
    showSelectInfoByUrl(g_subcribe_items[id][0])
    setSubscribeType(g_check_subscribe_items)
    ret = setEvtType(g_subcribe_items[id][1])
    setDest(g_subcribe_items[id][2])
    msg = lstnDestData.getJsonMsg()
    print color.PURPLE + "Dest url:       " + lstnDestData.getDest() + color.END
    print color.PURPLE + "Target url:     " + lstnDestData.getTarget() + color.END
    print color.BLUE  +  "MSG:            " + msg + color.END
    ret = http_post(lstnDestData.getTarget(), msg)
    print "response: " + ret

if __name__ == "__main__":
    cnt  = len(sys.argv)
    if cnt != 3:
        print paramsErr 
        exit(0)
    rmmip  = sys.argv[1]
    checkRMMAvailable(rmmip)
    listenerip = sys.argv[2]
    changeDefData(listenerip, rmmip)
    run()
