#! /usr/bin/env python
#coding=utf-8
import os
import sys
import string
import json

'''
global configuration MACRO
'''
STR_USAGE = "Usage:\
\npython json_format file_name\
\n\
\nFor example:\
\npython /var/log/AssetModule/logfile\
\npython /var/log/Assetd/logfile"

'''
global variable
'''
def json_format_line(str):
	try:
		data_string = json.loads(str)
		data_string = json.dumps(data_string, indent=4)
		print data_string
	except Exception, e:
		print "json_format_line except: %s!" %(e)
		exit(1)

def find_json_str(str):
	try:
		js_start = str.find('{"jsonrpc"')
		if  js_start < 0:
			return ""
		else:
			js_end = str.rfind('}', js_start, len(str))
			if js_end == -1 or js_end <= js_start:
				return ""
			else:
				return str[js_start:(js_end + 1)]
	except Exception, e:
		print "find_json_str except: %s!" %(e)
		exit(1)
		
def format_file(file_name):
	try:
		if not os.path.exists(file_name):
			print "Error: %s not exist!" %file_name
			exit(1)
            
		file_object = open(file_name,'r')
		for eachline in file_object.readlines():
			data_string = eachline.replace("","").replace("\t","").strip()
			js_str = find_json_str(data_string)
			if js_str != "":
				json_format_line(js_str)
	
		file_object.close()
	except Exception, e:
		print "json_format_line except: %s!" %(e)
		exit(1)


if __name__ == "__main__":
	try:
		if len(sys.argv) != 2:
			print "no input file"
			print STR_USAGE
			exit(1)
		
		format_file(str(sys.argv[1]))
	except Exception, e:
		print "main except: %s!" %(e)
		exit(1)
