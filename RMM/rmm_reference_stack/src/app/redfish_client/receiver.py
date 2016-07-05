#!/usr/bin/env python
from bottle import get,put,post,run,route,request
import os
import json
import sys

'''
@route('/v1/rack/power',method = 'POST')
def postpower():
	print "--------------"
	print  request.body.getvalue()
	print "------------------"
#	json_data = json.load(request.body)
#	jdata = json.loads(json_data)
#	print "data is {}!!".format(json_data)
#	return "data is {}!!".format(json_data)
	return "psu data is {}!!\n".format(request.body.getvalue())

@route('/v1/rack/thermal',method = 'POST')
def postthermal():
	print "--------------"
	print  request.body.getvalue()
	print "------------------"
	return "fan data is {}!!\n".format(request.body.getvalue())

@route('/v1/rack/mbp',method = 'POST')
def postmbp():
	print "--------------"
	print  request.body.getvalue()
	print "------------------"
	return "mbp data is {}!!\n".format(request.body.getvalue())

@route('/v1/rack/drawer',method = 'POST')
def postdrawer():
	print "--------------"
	print  request.body.getvalue()
	print "------------------"
	return "drawer data is {}!!\n".format(request.body.getvalue())
'''
@route('/v1/rack/rack',method = 'POST')
def postrack():
	print "--------------"
	print  request.body.getvalue()
	print "------------------"
	return "rack data is {}!!\n".format(request.body.getvalue())


@route('/v1/rack',method = 'get')
def getrack():
	return "OK"

##listen the port forever,the host address id "0.0.0.0" means that it can accept any other host request!!!####
run(host='0.0.0.0',port=sys.argv[1])
