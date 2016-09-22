import socket,select
from  MdbDef import MdbJKey as MdbJKey
from  MdbDef import MdbJDefVale as MdbJDefVale
from  MdbDef import memdb_cmds as  memdb_cmds
from common import parse_rsp_to_json as parse_rsp_to_json
from common import get_jrpc_method as get_jrpc_method
import json

msgID = 0
def send_msg(req):
	host = "127.0.0.1"
	port = 50000
	ADDR = (host, port) 
	s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
	s.sendto(req, ADDR)
	s.settimeout(1)
	return s;

def recv_msg(s):
	buf = s.recv(8196)
	return buf

def socket_err_rsp():
	return ""

def get_req(func_id, params):
	req = dict()
	global msgID 
	msgID = msgID + 1
	req[MdbJKey.jrpc_ver] 	= MdbJDefVale.jrpc_def_ver
	req[MdbJKey.method]		= get_jrpc_method(func_id)
	req[MdbJKey.msg_id]		= msgID
	req[MdbJKey.params] 	= params
	return (msgID,req)

def send_request(params, func_id):
	(msgID, req) = get_req(func_id, params)
	strReq = json.dumps(req)
	print "strReq" + strReq

	s = send_msg(strReq)
	rcvID = 0
	while rcvID < msgID:
		rsp = recv_msg(s)
		print "rsp :" + rsp
		if req != "":
			(rcvID, result) = parse_rsp_to_json(rsp)
		else:
			s.close()
			return
		pass
	return



