/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * */

#pragma once
#include <string>

static std::string FabricManager1 =
    R"({
        "collections": [
            {
                "name": "Fabrics",
                "slotMask": "SlotMask",
                "type": "Fabrics"
            }
        ],
        "commandShell": {
            "enabled": null,
            "maxSessions": null,
            "typesSupported": []
        },
        "allowedActions": [],
        "dateTime": null,
        "dateTimeLocalOffset": null,
        "firmwareVersion": "2.58",
        "graphicalConsole": {
            "enabled": null,
            "maxSessions": null,
            "typesSupported": []
        },
        "guid": "123e4567-e89b-ffff-a456-426655440000",
        "ipv4Address": "1.1.2.1",
        "location": null,
        "model": "DD43",
        "networkServices": [
            {
                "enabled": false,
                "name": "IPMI",
                "port": 0
            },
            {
                "enabled": false,
                "name": "SSH",
                "port": 0
            },
            {
                "enabled": false,
                "name": "Telnet",
                "port": 0
            }
        ],
        "oem": {},
        "parentId": "",
        "serialConsole": {
            "bitrate": 115200,
            "dataBits": 8,
            "enabled": true,
            "flowControl": "None",
            "maxSessions": null,
            "parity": "None",
            "pinOut": "Cisco",
            "signalType": "Rs232",
            "stopBits": 1,
            "typesSupported": []
        },
        "status": {
            "health": "OK",
            "state": "Enabled"
        },
        "type": "ManagementController",
        "parentId": "parent"
    })";

static std::string Fabric1 =
    R"({
        "collections": [
            {
                "name": "Zones",
                "slotMask": "SlotMask",
                "type": "Zones"
            },
            {
                "name": "Endpoints",
                "slotMask": "SlotMask",
                "type": "Endpoints"
            },
            {
                "name": "Switches",
                "slotMask": "SlotMask",
                "type": "Switches"
            }
        ],
        "oem": {},
        "protocol": "PCIe",
        "status": {
            "health": "OK",
            "state": "Enabled"
        },
        "oemProtocol": null
    })";

static std::string FabricZone1 =
    R"({
        "collections": [
            {
                "name": "Endpoints",
                "slotMask": "SlotMask",
                "type": "Endpoints"
            }
        ],
        "oem": {},
        "status": {
            "health": "OK",
            "state": "Enabled"
        }
    })";

static std::string FabricZone2 =
    R"({
        "collections": [
            {
                "name": "Endpoints",
                "slotMask": "SlotMask",
                "type": "Endpoints"
            }
        ],
        "oem": {},
        "status": {
            "health": "OK",
            "state": "Enabled"
        }
    })";

static std::string Endpoint1InZone1 =
    R"({
        "name": null,
        "description": null,
        "collections": [
            {
                "name": "Ports",
                "slotMask": "SlotMask",
                "type": "Ports"
            }
        ],
        "connectedEntities": [
            {
                "entity": null,
                "role": "Initiator",
                "lun": null,
                "identifiers" : []
            }
        ],
        "fabric": "9e8aa67c-a8f9-11e7-9371-b3c84ca2076c",
        "identifiers": [],
        "ipTransportDetails": [],
        "username": "user",
        "password": "pass",
        "oem": {},
        "protocol": "PCIe",
        "status": {
            "health": "OK",
            "state": "Enabled"
        },
        "oemProtocol": null
    })";

static std::string Endpoint1InZone2 =
    R"({
        "name": null,
        "description": null,
        "collections": [
            {
                "name": "Ports",
                "slotMask": "SlotMask",
                "type": "Ports"
            }
        ],
        "connectedEntities": [
            {
                "entity": null,
                "role": "Initiator",
                "lun": null,
                "identifiers" : []
            }
        ],
        "fabric": "9e8aa67c-a8f9-11e7-9371-b3c84ca2076c",
        "identifiers": [],
        "ipTransportDetails": [],
        "username": "user",
        "password": "pass",
        "oem": {},
        "protocol": "PCIe",
        "status": {
            "health": "OK",
            "state": "Enabled"
        },
        "oemProtocol": null
    })";

static std::string Endpoint2InZone2 =
    R"({
        "name": null,
        "description": null,
        "collections": [
            {
                "name": "Ports",
                "slotMask": "SlotMask",
                "type": "Ports"
            }
        ],
        "connectedEntities": [
            {
                "entity": null,
                "role": "Initiator",
                "lun": null,
                "identifiers" : []
            }
        ],
        "fabric": "9e8aa67c-a8f9-11e7-9371-b3c84ca2076c",
        "identifiers": [],
        "ipTransportDetails": [],
        "username": "user",
        "password": "pass",
        "oem": {},
        "protocol": "PCIe",
        "status": {
            "health": "OK",
            "state": "Enabled"
        },
        "oemProtocol": null
    })";
