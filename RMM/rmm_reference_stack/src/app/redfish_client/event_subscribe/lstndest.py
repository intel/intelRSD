import os

G_SUBSCRIBE     = 1
G_UNSUBSCRIBE   = 2

class lstndest:
    def __init__(self):
        self.type = 'RMM1.0.0'
        self.desc = 'For internal test only'
        self.evtType = "Add"
        self.protcol = 'Redfish'
        self.dest = 'http://client:8000/destination/'
        self.target = 'http://subcribe:8000/rack/psu'
        self.reqType = 1
    
    def setReqType(self, type):
        self.reqType = type
    
    def setEvtType(self, type):
        self.evtType = type
        
    def setDest(self, dest):
        self.dest = dest
    
    def getDest(self):
        return self.dest


    def setTarget(self, url):
        self.target = url
    def getTarget(self):
        return self.target
    
    def getJsonMsg(self):
        if self.reqType == G_SUBSCRIBE:
            return ('{"Action":"Subscribe", "Type":"%s","Destination":"%s","EventTypes":[{"Event":"%s"}] ,"Protocol":"%s"}')  \
                % (self.type, self.dest, self.evtType, self.protcol)
        elif self.reqType == G_UNSUBSCRIBE:
            return ('{"Action":"Unsubscribe", "Type":"%s", "Destination":"%s","EventTypes":[{"Event":"%s"}] ,"Protocol":"%s"}')  \
                % (self.type, self.dest, self.evtType, self.protcol)
       
        return ''
            
        
