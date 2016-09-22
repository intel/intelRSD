#!/usr/bin/python
from MdbDef import CmdId as CmdId
from MdbDef import MdbJKey as MdbJKey
from remote import send_request as send_request
from MdbDef import McType as McType
from common import get_type_str as get_type_str

def get_req_info(db_name, parent, mctype, node_id, snapshot_flag, lock_id):
	params = dict()
	params[MdbJKey.db_name] = db_name
	params[MdbJKey.lock_id] = lock_id
	params[MdbJKey.node_id] = node_id
	params[MdbJKey.parent] = parent
	params[MdbJKey.mtype] = get_type_str(mctype)
	params[MdbJKey.snapshot] = snapshot_flag
	return params

def list_node(db_name, node_id, typeMatch, typeValue, lock_id, cmdId):
	params[MdbJKey.db_name] = db_name
	params[MdbJKey.lock_id] = lock_id
	params[MdbJKey.node_id] = node_id
	params[MdbJKey.type_match] = typeMatch
	params[MdbJKey.type_value] = get_type_str(typeValue)
	send_request(params, cmdId)


def libdb_dump_nodes(db_name, lock_id):
	'''
	params = dict()
	params[MdbJKey.db_name] = db_name
	params[MdbJKey.lock_id] = lock_id
	params[MdbJKey.node_id] = 0
	'''
	req = get_req_info(db_name, McType.MC_TYPE_RMC, McType.MC_TYPE_RMC, 0, 0, lock_id)
	send_request(req, CmdId.CMD_DUMP_NODES)



def libdb_create_node_with_node_id(db_name, parent, mctype, node_id, snapshot_flag, lock_id):
	req = get_req_info(db_name, parent, mctype, node_id, snapshot_flag, lock_id)
	send_request(req, CmdId.CMD_NODE_CREATE)

def libdb_create_node(db_name, parent, mctype, snapshot_flag, lock_id):
	req = get_req_info(db_name, parent, mctype, node_id, snapshot_flag, lock_id)
	send_request(req, CmdId.CMD_NODE_CREATE_WITH_NODE_ID)

def libdb_get_node_by_node_id(db_name, node_id, lock_id):
	req = get_req_info(db_name, McType.MC_TYPE_RMC, McType.MC_TYPE_RMC, node_id, 0, lock_id)
	send_request(req, CmdId.CMD_NODE_GET_BY_NODE_ID)

def libdb_destroy_node(db_name, node_id, lock_id):
	req = get_req_info(db_name, McType.MC_TYPE_RMC, McType.MC_TYPE_RMC, node_id, 0, lock_id)
	send_request(req, CmdId.CMD_NODE_DESTROY)

def libdb_list_subnode(db_name, node, lock_id):
	list_node(db_name, node, 0, 0, lock_id,	CmdId.CMD_LIST_SUBNODE)
	return

def libdb_list_subnode_by_type(db_name, node, type, lock_id):
	list_node(db_name, node, 1, type, lock_id,	CmdId.CMD_LIST_SUBNODE)
	return

def libdb_list_node(db_name, lock_id):
	params[MdbJKey.db_name] = db_name
	params[MdbJKey.lock_id] = lock_id
	params[MdbJKey.type_match] = typeMatch
	params[MdbJKey.type_value] = get_type_str(typeValue)	
	return

def libdb_list_node_by_type(db_name, type_min, type_max, nodenum, lock_id):
	return


def libdb_attr_remove(db_name, node, name, lock_id):
	return

def libdb_lock(timeout):
	return
def libdb_unlock(lock_id):
	return

def libdb_attr_set(db_name, node_id, name, cookie, value, snapshot_flag, lock_id):
	params = dict()
	params[MdbJKey.db_name] = db_name
	params[MdbJKey.lock_id] = lock_id
	params[MdbJKey.node_id] = node_id
	params[MdbJKey.cookie] = cookie
	if isinstance(value, str) == False:
		value = str(value)
	params[MdbJKey.name] = name
	params[MdbJKey.data] = value
	params[MdbJKey.snapshot] = snapshot_flag
	send_request(params, CmdId.CMD_ATTRBUTE_SET)


def libdb_attr_get(db_name, node_id, name, lock_id):
	params = dict()
	params[MdbJKey.db_name] = db_name
	params[MdbJKey.lock_id] = lock_id
	params[MdbJKey.node_id] = node_id
	params[MdbJKey.name] = name
	send_request(params, CmdId.CMD_ATTRBUTE_GET)
	return


def libdb_list_attrs_by_node(db_name, node_id, name, lock_id):
	return

def libdb_list_attrs_by_cookie(db_name, node_id, name, lock_id):
	return

libdb_dump_nodes("RMM", 0)
#libdb_create_node("RMM", McType.MC_TYPE_RMC, McType.MC_TYPE_CM, 1, 0)
#libdb_attr_get("RMM", 10000073, "MsgID", 0)
#libdb_attr_set("RMM", 10000073, "MsgID",  0, "1111", 0, 0)
libdb_destroy_node("RMM", 10000073, 0)