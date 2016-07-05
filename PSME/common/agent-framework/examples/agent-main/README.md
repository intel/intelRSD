Testing
--------

COMPUTE

    curl -d '{"jsonrpc": "2.0", "method": "getComponentCollection", "params": {}, "id":1}' IP:PORT

    curl -d '{"jsonrpc": "2.0", "method": "getModuleInfo", "params": {"module" : "uuid"}, "id":1}' IP:PORT

    curl -d '{"jsonrpc": "2.0", "method": "getCollection", "params": {"component" : "uuid", "name" : "collection_name"}, "id":1}' IP:PORT

    curl -d '{"jsonrpc": "2.0", "method": "getBladeInfo", "params": {"blade" : "uuid"}, "id":1}' IP:PORT

    curl -d '{"jsonrpc": "2.0", "method": "getManagerInfo", "params": {"component": "uuid"}, "id":1}' IP:PORT

    curl -d '{"jsonrpc": "2.0", "method": "getMemoryInfo", "params": {"memory": "uuid"}, "id":1}' IP:PORT

    curl -d '{"jsonrpc": "2.0", "method": "getProcessorInfo", "params": {"processor": "uuid"}, "id":1}' IP:PORT

    curl -d '{"jsonrpc": "2.0", "method": "getNetworkInterfaceInfo", "params": {"interface": "uuid"}, "id":1}' IP:PORT

    curl -d '{"jsonrpc": "2.0", "method": "getStorageControllerInfo", "params": {"controller": "uuid"}, "id":1}' IP:PORT

    curl -d '{"jsonrpc": "2.0", "method": "getDriveInfo", "params": {"drive": "uuid"}, "id":1}' IP:PORT

NETWORK

    curl -d '{"jsonrpc": "2.0", "method": "addVlan", "params": {"component" : "0", "vlanId": 1, "vlanName" : "vlan1"}, "id":1}' IP:PORT

    curl -d '{"jsonrpc": "2.0", "method": "addVlanPort", "params": {"component" : "0", "portIdentifier": "1", "vlanId" : 1, "tagged" : false}, "id":1}' IP:PORT

    curl -d '{"jsonrpc": "2.0", "method": "getComponents", "params": {"component" : ""}, "id":1}' IP:PORT

    curl -d '{"jsonrpc": "2.0", "method": "getSwitchInfo", "params": {"component" : ""}, "id":1}' IP:PORT

    curl -d '{"jsonrpc": "2.0", "method": "getSwitchPortInfo", "params": {"component" : "1", "port" : 1}, "id":1}' IP:PORT

    curl -d '{"jsonrpc": "2.0", "method": "getVlanInfo", "params": {"component" : "1", "vlan" : 1}, "id":1}' IP:PORT

    curl -d '{"jsonrpc": "2.0", "method": "getVlanPortInfo", "params": {"component" : "1", "port" : 1, "vlan" : 1}, "id":1}' IP:PORT

STORAGE

	curl -d '{"jsonrpc": "2.0", "method": "getiSCSITargetInfo", "params": {"target" : "f3b47280-08b6-11e5-a11d-0800272cd2ad"}, "id":1}' localhost:7777

	curl -d '{"jsonrpc": "2.0", "method": "getComponentCollection", "params": {}, "id":1}' localhost:7777

	curl -d '{"jsonrpc": "2.0", "method": "getStorageServicesInfo", "params": {"services" : "eda31572-08b6-11e5-94e7-0800272cd2ad"}, "id":1}' localhost:7777

	curl -d '{"jsonrpc": "2.0", "method": "getStorageServicesInfo", "params": {"services" : "eda31572-08b6-11e5-94e7-0800272cd2ad"}, "id":1}' localhost:7777 | python -m json.tool7

	curl -d '{"jsonrpc": "2.0", "method": "getCollection", "params": {"component" : "eda31572-08b6-11e5-94e7-0800272cd2ad", "name" : "PhysicalDrives"}, "id":1}' localhost:7777 | python -m json.tool

	curl -d '{"jsonrpc": "2.0", "method": "getCollection", "params": {"component" : "eda31572-08b6-11e5-94e7-0800272cd2ad", "name" : "LogicalDrives"}, "id":1}' localhost:7777 | python -m json.tool

	curl -d '{"jsonrpc": "2.0", "method": "getCollection", "params": {"component" : "eda31572-08b6-11e5-94e7-0800272cd2ad", "name" : "iSCSITargets"}, "id":1}' localhost:7777 | python -m json.tool

	curl -d '{"jsonrpc": "2.0", "method": "getPhysicalDriveInfo", "params": {"drive" : "f3b261de-08b6-11e5-a6b0-0800272cd2ad"}, "id":1}' localhost:7777 | python -m json.tool

	curl -d '{"jsonrpc": "2.0", "method": "getLogicalDriveInfo", "params": {"drive" : "f3b3c60a-08b6-11e5-b2bc-0800272cd2ad"}, "id":1}' localhost:7777 | python -m json.tool

	curl -d '{"jsonrpc": "2.0", "method": "getiSCSITargetInfo", "params": {"target" : "f3b47280-08b6-11e5-a11d-0800272cd2ad"}, "id":1}' localhost:7777 | python -m json.tool

	curl -d '{"jsonrpc": "2.0", "method": "addiSCSITarget", "params": {"initiatorIQN" : "iqn.2015-1.com.pod-1-initiator", "targetIQN": "iqn.2015-01.com.ceph-ubuntu", "targetLUN": [{"LUN": 1, "logicalDrive": "f3b47280-08b6-11e5-a11d-0800272cd2ad"}], "oem": {}}, "id":1}' localhost:7777 | python -m json.tool

	curl -d '{"jsonrpc": "2.0", "method": "deleteiSCSITarget", "params": {"target" : "f3b47280-08b6-11e5-a11d-0800272cd2ad"}, "id":1}' localhost:7777 | python -m json.tool

