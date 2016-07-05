/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

static std::string Manager1 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "type": "ManagementController",
            "model": null,
            "location": null,
            "firmwareVersion": "2112",
            "ipv4Address": "2.1.1.2",
            "networkServices": [
                {
                    "name": "IPMI",
                    "port": 21,
                    "enabled": true
                }
            ],
            "graphicalConsole": {
                "enabled": true,
                "maxSessions": null,
                "types": []
            },
            "serialConsole": {
                "signalType": "Rs232",
                "bitrate": 1200,
                "parity": "None",
                "dataBits": 8,
                "stopBits": 1,
                "flowControl": "None",
                "pinOut": "Cisco",
                "enabled": false,
                "types": []
            },
            "commandShell": {
                "enabled": null,
                "maxSessions": null,
                "types": []
            },
            "collections": [
                {
                    "name": "Systems",
                    "type": "Systems"
                }
            ],
            "oem": {}
        })";

static std::string System1 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "biosVersion": "A20F21_A0",
            "bootOverride": "Once",
            "bootOverrideTarget": "Hdd",
            "bootOverrideSupported": [ "Hdd", "Pxe" ],
            "uefiTarget": "system_1_uefi_target",
            "powerState": "On",
            "pciDevices": [
                {
                    "vendorId": "0x2112",
                    "deviceId": "0x1000"
                }
            ],
            "indicatorLED": "Off",
            "fruInfo": {
                "serialNumber": "System1",
                "manufacturer": "Intel Corporation",
                "modelNumber": "E5150",
                "partNumber": "29ee2220939"
            },
            "collections": [],
            "chassis": "none",
            "oem": {}
        })";


static std::string System2 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "biosVersion": "A20F21_A0",
            "bootOverride": "Once",
            "bootOverrideTarget": "Hdd",
            "bootOverrideSupported": [ "Hdd", "Pxe" ],
            "uefiTarget": "system_2_uefi_target",
            "powerState": "On",
            "pciDevices": [
                {
                    "vendorId": "0x2112",
                    "deviceId": "0x1000"
                }
            ],
            "indicatorLED": "Off",
            "fruInfo": {
                "serialNumber": "System1",
                "manufacturer": "Intel Corporation",
                "modelNumber": "E5150",
                "partNumber": "29ee2220939"
            },
            "collections": [
                {
                    "name": "Processors",
                    "type": "Processors",
                    "slotMask": 11
                }
            ],
            "chassis": "none",
            "oem": {}
        })";

static std::string System2Modified =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "biosVersion": "B20F21_A0",
            "bootOverride": "Once",
            "bootOverrideTarget": "Hdd",
            "bootOverrideSupported": [ "Hdd", "Pxe" ],
            "uefiTarget": "system_2_uefi_target",
            "powerState": "On",
            "pciDevices": [
                {
                    "vendorId": "0x2112",
                    "deviceId": "0x1000"
                }
            ],
            "indicatorLED": "Off",
            "fruInfo": {
                "serialNumber": "System1",
                "manufacturer": "Intel Corporation",
                "modelNumber": "E5150",
                "partNumber": "29ee2220939"
            },
            "collections": [
                {
                    "name": "Processors",
                    "type": "Processors",
                    "slotMask": 11
                }
            ],
            "chassis": "none",
            "oem": {}
        })";

static std::string Processor1 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "socket": "1151",
            "processorType": "CPU",
            "processorArchitecture": "x86_64",
            "instructionSet": "x64",
            "manufacturer": "Intel Corporation",
            "model": "I7",
            "modelName": "Cool Model Name",
            "cpuid": {
                "vendorId": "GenuineIntel",
                "numericId": "1001001",
                "family": 6,
                "model": 5,
                "step": 4,
                "microcodeInfo": 3
            },
            "maxSpeedMHz": 2400,
            "totalCores": 16,
            "enabledCores": 16,
            "totalThreads": 32,
            "enabledThreads": 32,
            "oem": {}
        })";

static std::string Processor2 =
        R"({
            "status": {
               "health": "OK",
               "state": "Disabled"
            },
            "socket": "1151",
            "processorType": "CPU",
            "processorArchitecture": "x86_64",
            "instructionSet": "x64",
            "manufacturer": "Intel Corporation",
            "model": "I5",
            "modelName": "Cool Model Name",
            "cpuid": {
                "vendorId": "GenuineIntel",
                "numericId": "1001001",
                "family": 6,
                "model": 5,
                "step": 4,
                "microcodeInfo": 3
            },
            "maxSpeedMHz": 1200,
            "totalCores": 4,
            "enabledCores": 3,
            "totalThreads": 8,
            "enabledThreads": 2,
            "oem": {}
        })";

static std::string Processor2Modified =
        R"({
            "status": {
               "health": "OK",
               "state": "Disabled"
            },
            "socket": "666",
            "processorType": "CPU",
            "processorArchitecture": "x86_64",
            "instructionSet": "x64",
            "manufacturer": "Intel Corporation",
            "model": "I5",
            "modelName": "Cool Model Name",
            "cpuid": {
                "vendorId": "GenuineIntel",
                "numericId": "1001001",
                "family": 6,
                "model": 5,
                "step": 4,
                "microcodeInfo": 3
            },
            "maxSpeedMHz": 1200,
            "totalCores": 4,
            "enabledCores": 3,
            "totalThreads": 8,
            "enabledThreads": 2,
            "oem": {}
        })";

static std::string Manager2 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "type": "ManagementController",
            "firmwareVersion": "21122",
            "ipv4Address": "2.1.1.2",
            "networkServices": [
                {
                    "name": "IPMI",
                    "port": 21,
                    "enabled": true
                }
            ],
            "graphicalConsole": {
                "enabled": true,
                "maxSessions": null,
                "types": []
            },
            "serialConsole": {
                "signalType": "Rs232",
                "bitrate": 1200,
                "parity": "None",
                "dataBits": 8,
                "stopBits": 1,
                "flowControl": "None",
                "pinOut": "Cisco",
                "enabled": false,
                "types": []
            },
            "commandShell": {
                "enabled": null,
                "maxSessions": null,
                "types": []
            },
            "collections": [
                {
                    "name": "Systems",
                    "type": "Systems"
                }
            ],
            "oem": {}
        })";

static std::string System3 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "biosVersion": "A20F21_A0",
            "bootOverride": "Once",
            "bootOverrideTarget": "Hdd",
            "bootOverrideSupported": [ "Hdd", "Pxe" ],
            "uefiTarget": "system_3_uefi_target",
            "powerState": "On",
            "pciDevices": [
                {
                    "vendorId": "0x2112",
                    "deviceId": "0x1000"
                }
            ],
            "indicatorLED": "Off",
            "fruInfo": {
                "serialNumber": "System1",
                "manufacturer": "Intel Corporation",
                "modelNumber": "E5150",
                "partNumber": "29ee2220939"
            },
            "collections": [
                {
                    "name": "Processors",
                    "type": "Processors",
                    "slotMask": 11
                }
            ],
            "chassis": "none",
            "oem": {}
        })";

static std::string Processor1OnSystem3 =
        R"({
            "status": {
               "health": "OK",
               "state": "Disabled"
            },
            "socket": "1151",
            "processorType": "CPU",
            "processorArchitecture": "x86_64",
            "instructionSet": "x64",
            "manufacturer": "Intel Corporation",
            "model": "I3",
            "modelName": "Cool Model Name",
            "cpuid": {
                "vendorId": "GenuineIntel",
                "numericId": "1001001",
                "family": 6,
                "model": 5,
                "step": 4,
                "microcodeInfo": 3
            },
            "maxSpeedMHz": 1200,
            "totalCores": 2,
            "enabledCores": 1,
            "totalThreads": 2,
            "enabledThreads": 2,
            "oem": {}
        })";

static std::string Processor1OnSystem3Modified =
        R"({
            "status": {
               "health": "OK",
               "state": "Disabled"
            },
            "socket": "1151",
            "processorType": "CPU",
            "processorArchitecture": "x86_64",
            "instructionSet": "x64",
            "manufacturer": "Intel Corporation",
            "model": "I3",
            "modelName": "Cool Model Name",
            "cpuid": {
                "vendorId": "GenuineIntel",
                "numericId": "1001001",
                "family": 6,
                "model": 5,
                "step": 4,
                "microcodeInfo": 3
            },
            "maxSpeedMHz": 666,
            "totalCores": 2,
            "enabledCores": 1,
            "totalThreads": 2,
            "enabledThreads": 2,
            "oem": {}
        })";

static std::string System4 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "biosVersion": "A20F21_A0",
            "bootOverride": "Once",
            "bootOverrideTarget": "Hdd",
            "bootOverrideSupported": [ "Hdd", "Pxe" ],
            "uefiTarget": "system_4_uefi_target",
            "powerState": "On",
            "pciDevices": [
                {
                    "vendorId": "0x2112",
                    "deviceId": "0x1000"
                }
            ],
            "indicatorLED": "Off",
            "fruInfo": {
                "serialNumber": "System1",
                "manufacturer": "Intel Corporation",
                "modelNumber": "E5150",
                "partNumber": "29ee2220939"
            },
            "collections": [
                {
                    "name": "Processors",
                    "type": "Processors",
                    "slotMask": 11
                }
            ],
            "chassis": "none",
            "oem": {}
        })";

static std::string Processor1OnSystem4 =
        R"({
            "status": {
               "health": "OK",
               "state": "Disabled"
            },
            "socket": "1158",
            "processorType": "CPU",
            "processorArchitecture": "x86_64",
            "instructionSet": "x64",
            "manufacturer": "Intel Corporation",
            "model": "I3",
            "modelName": "Cool Model Name",
            "cpuid": {
                "vendorId": "GenuineIntel",
                "numericId": "1001001",
                "family": 6,
                "model": 5,
                "step": 4,
                "microcodeInfo": 3
            },
            "maxSpeedMHz": 1200,
            "totalCores": 2,
            "enabledCores": 1,
            "totalThreads": 2,
            "enabledThreads": 2,
            "oem": {}
        })";

static std::string Manager3 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "type": "ManagementController",
            "model": null,
            "location": null,
            "firmwareVersion": "2112",
            "ipv4Address": "2.1.1.2",
            "networkServices": [
                {
                    "name": "IPMI",
                    "port": 21,
                    "enabled": true
                }
            ],
            "graphicalConsole": {
                "enabled": true,
                "maxSessions": null,
                "types": []
            },
            "serialConsole": {
                "signalType": "Rs232",
                "bitrate": 1200,
                "parity": "None",
                "dataBits": 8,
                "stopBits": 1,
                "flowControl": "None",
                "pinOut": "Cisco",
                "enabled": false,
                "types": []
            },
            "commandShell": {
                "enabled": null,
                "maxSessions": null,
                "types": []
            },
            "collections": [
                {
                    "name": "FastSystems",
                    "type": "Systems"
                },
                {
                    "name": "SlowSystems",
                    "type": "Systems"
                }
            ],
            "oem": {}
        })";

static std::string System5 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "biosVersion": "A20F21_A0",
            "bootOverride": "Once",
            "bootOverrideTarget": "Hdd",
            "bootOverrideSupported": [ "Hdd", "Pxe" ],
            "uefiTarget": "system_4_uefi_target",
            "powerState": "On",
            "pciDevices": [
                {
                    "vendorId": "0x2112",
                    "deviceId": "0x1000"
                }
            ],
            "indicatorLED": "Off",
            "fruInfo": {
                "serialNumber": "System1",
                "manufacturer": "Intel Corporation",
                "modelNumber": "E5150",
                "partNumber": "29ee2220939"
            },
            "collections": [
                {
                    "name": "FastProcessors",
                    "type": "Processors"
                },
                {
                    "name": "SlowProcessors",
                    "type": "Processors"
                }
            ],
            "chassis": "none",
            "oem": {}
        })";

static std::string System6 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "biosVersion": "A20F21_A0",
            "bootOverride": "Once",
            "bootOverrideTarget": "Hdd",
            "bootOverrideSupported": [ "Hdd", "Pxe" ],
            "uefiTarget": "system_5_uefi_target",
            "powerState": "On",
            "pciDevices": [
                {
                    "vendorId": "0x2112",
                    "deviceId": "0x1000"
                }
            ],
            "indicatorLED": "Off",
            "fruInfo": {
                "serialNumber": "System1",
                "manufacturer": "Intel Corporation",
                "modelNumber": "E5150",
                "partNumber": "29ee2220939"
            },
            "collections": [
                {
                    "name": "Anything",
                    "type": "Processors"
                },
                {
                    "name": "whAtever",
                    "type": "Processors"
                }
            ],
            "chassis": "none",
            "oem": {}
        })";

