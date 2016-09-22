import json
from MdbDef import mc_types as mc_types
from MdbDef import memdb_cmds as memdb_cmds
from MdbDef import MdbJKey as MdbJKey


def get_type_str(tcType):
	for (tid, name) in mc_types:
		if tid == tcType:
			return name
	return ""

def parse_rsp_to_json(rsp):
	strJson = "".join([ rsp.strip().rsplit("}" , 1)[0] ,  "}"] )
	jrsp = json.loads(strJson)
	mid = jrsp[MdbJKey.msg_id]
	result = jrsp[MdbJKey.result]
	#error = jrsp["fdf"]
	return (mid, result)
	

def get_jrpc_method(func_id):
	for (fid, name) in memdb_cmds:
		if fid == func_id:
			return name
	return ""