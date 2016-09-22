
class CmdId:
	CMD_DUMP_NODES 		= 0
	CMD_NODE_CREATE  	= 1
	CMD_NODE_DESTROY	= 2
	CMD_LIST_SUBNODE 	= 3
	CMD_LIST_NODE 		= 4

	CMD_ATTRBUTE_SET 	= 5
	CMD_ATTRBUTE_GET 	= 6
	CMD_ATTRBUTE_REMOVE = 7 
	CMD_LIST_ATTRS_BY_NODE 			= 8
	CMD_LIST_ATTRS_BY_COOKIE 		= 9 

	CMD_ADD_SUBSCRIPTION  			= 10
	CMD_REMOVE_SUBSCRIPTION 		= 11
	CMD_NODE_GET_BY_NODE_ID 		= 12
	CMD_NODE_CREATE_WITH_NODE_ID	= 13

	CMD_LOCK 						= 14
	CMD_UNLOCK 						= 15

	CMD_MAX							= 16


memdb_cmds = [(CmdId.CMD_DUMP_NODES, "dump_nodes"), 
			(CmdId.CMD_NODE_CREATE, "create_node"),
			(CmdId.CMD_NODE_DESTROY, "node_destroy"),
			(CmdId.CMD_LIST_SUBNODE, "list_subnode"),
			(CmdId.CMD_LIST_NODE, "list_node"),
			(CmdId.CMD_ATTRBUTE_SET, "attrbute_set"),
			(CmdId.CMD_ATTRBUTE_GET, "attrbute_get"),
			(CmdId.CMD_ATTRBUTE_REMOVE, "attrbute_remove"),
			(CmdId.CMD_LIST_ATTRS_BY_NODE, "list_attrs_by_node"),
			(CmdId.CMD_LIST_ATTRS_BY_COOKIE, "list_attrs_by_cookie"),
			(CmdId.CMD_ADD_SUBSCRIPTION, "add_subscription"),
			(CmdId.CMD_REMOVE_SUBSCRIPTION, "remove_subscription"),
			(CmdId.CMD_NODE_GET_BY_NODE_ID, "node_get_by_node_id"),
			(CmdId.CMD_NODE_CREATE_WITH_NODE_ID, "node_create_with_node_id"),
			(CmdId.CMD_LOCK, "lock"),
			(CmdId.CMD_UNLOCK, "unlock")]


class MdbJKey(object):
	"""docstring for MdbJKey"""
	jrpc_ver 	= "json_rpc"
	method   	= "method"
	msg_id	 	= "id"			 

	db_name 	= "db_name"
	node_id 	= "node_id"
	lock_id 	= "lock_id"
	params		= "params"

	cookie 		= "p_cookie"
	name   		= "p_name"
	data   		= "p_data"

	parent 		= "p_parent"
	mtype  		= "p_type"
	snapshot 	= "p_snapshot_flag"

	result   	= "result"
	err_id 		= "error"
	err_code 	= "code"
	err_msg 	= "message"

	#list subnode
	type_match = "p_type_match"
	type_value = "p_type_value"

	type_max   = "p_type_max"
	type_min   = "p_type_min"


class MdbJDefVale(object):
	jrpc_def_ver = "2.0"

class McType(object):
	MC_TYPE_RMC = 0
	MC_TYPE_CM 	= 1
	MC_TYPE_DZONE 	= 2
	MC_TYPE_DRAWER 	= 3
	MC_TYPE_BMC    	= 4
	MC_TYPE_PZONE	= 5
	MC_TYPE_PSU 	= 6
	MC_TYPE_TZONE	= 7
	MC_TYPE_FAN		= 8
	MC_TYPE_SENSOR 	= 9
	MC_TYPE_REG		= 10
	'''
	MC_REDFISH
	MC_REDFISH_EVENT
	MC_REDFISH_LOG
	MC_REDFISH_RACK 
	MC_REDFISH_PZONE
	MC_REDFISH_PSU_COLL

	MC_REDFISH_TZONE
	MC_REDFISH_FAN_COLL
	MC_REDFISH_DZONE
	MC_REDFISH_DRAWER_COLL
	MC_REDFISH_MZONE
	MC_REDFISH_MBP_COLL
	MC_REDFISH_CHANGE
	MC_REDFISH_UPDATE
	MC_REDFISH_ADD
	MC_REDFISH_REMOVE
	MC_REDFISH_ALERT
	MC_REDFISH_LISTENER

	MC_REDFISH_PSU
	MC_REDFISH_FAN
	MC_REDFISH_SYS
	MC_REDFISH_MBP
	'''
	MC_TYPE_END


mc_types = [(McType.MC_TYPE_RMC, 	"RMC"),
			(McType.MC_TYPE_CM, 	"CM"),
			(McType.MC_TYPE_DZONE, 	"DZONE"),
			(McType.MC_TYPE_DRAWER, "DRAWER"),
			(McType.MC_TYPE_PZONE, 	"PZONE"),
			(McType.MC_TYPE_PSU, 	"PSU"),
			(McType.MC_TYPE_TZONE, 	"TZONE"),
			(McType.MC_TYPE_FAN, 	"FAN"),
			(McType.MC_TYPE_SENSOR, "SENSOR"),
			(McType.MC_TYPE_REG, 	"REG"),
			(McType.MC_INVALID, 	"INVAILD")]


class JRpcRc(object):
	#JSON RPC ERROR CODE
	JSONRPC_PARSE_ERROR = -32700
	JSONRPC_INVALID_REQ = -32600
	JSONRPC_METHOD_NOTFOUND = -32601
	JSONRPC_INVALID_PARAMS = -32602
	JSONRPC_INTERNAL_ERR = -32603
	JSONRPC_SERVER_ERR = -32000

	JSONRPC_32001 = -32001
	JSONRPC_32002 = -32002
	JSONRPC_32003 = -32003
	JSONRPC_32004 = -32004
	JSONRPC_FILL_PARAM_ERR = -32005
	
	ERR_JSONRPC_RSP_INVALID = -100