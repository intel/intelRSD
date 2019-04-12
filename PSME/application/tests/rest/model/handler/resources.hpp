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

static std::string Manager1 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "type": "ManagementController",
            "guid": "guid",
            "parentId": "parent",
            "allowedActions": [],
            "model": null,
            "location": null,
            "firmwareVersion": "2112",
            "dateTime": "10H23",
            "dateTimeLocalOffset": "3",
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
                "typesSupported": []
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
                "maxSessions": 1,
                "typesSupported": []
            },
            "commandShell": {
                "enabled": null,
                "maxSessions": null,
                "typesSupported": []
            },
            "collections": [
                {
                    "name": "Systems",
                    "type": "Systems",
                    "slotMask": "11"
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
            "type": "Physical",
            "biosVersion": "A20F21_A0",
            "bootOverride": "Once",
            "bootOverrideMode": "Legacy",
            "bootOverrideTarget": "Hdd",
            "bootOverrideSupported": [ "Hdd", "Pxe" ],
            "uefiTarget": "system_1_uefi_target",
            "powerState": "On",
            "assetTag": "tag",
            "cableIds": ["132"],
            "systemGuid": "guid123",
            "txtEnabled": true,
            "userModeEnabled": true,
            "sku": "123",
            "pciDevices": [
                {
                    "vendorId": "0x2112",
                    "deviceId": "0x1000"
                }
            ],
            "usbDevices": [],
            "indicatorLED": "Off",
            "fruInfo": {
                "serialNumber": "System1",
                "manufacturer": "Intel Corporation",
                "modelNumber": "E5150",
                "partNumber": "29ee2220939"
            },
            "collections": [],
            "chassis": "none",
            "oem": {},
            "currentPerformanceConfiguration": null,
            "performanceConfigurations": []
        })";


static std::string System2 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "type": "Physical",
            "biosVersion": "A20F21_A0",
            "bootOverride": "Once",
            "bootOverrideMode": "Legacy",
            "bootOverrideTarget": "Hdd",
            "bootOverrideSupported": [ "Hdd", "Pxe" ],
            "uefiTarget": "system_2_uefi_target",
            "powerState": "On",
            "assetTag": "tag",
            "cableIds": ["132"],
            "systemGuid": "guid123",
            "txtEnabled": true,
            "userModeEnabled": true,
            "sku": "123",
            "pciDevices": [
                {
                    "vendorId": "0x2112",
                    "deviceId": "0x1000"
                }
            ],
            "usbDevices": [],
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
                    "slotMask": "11"
                }
            ],
            "chassis": "none",
            "oem": {},
            "currentPerformanceConfiguration": null,
            "performanceConfigurations": []
        })";

static std::string System2Modified =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "type": "Physical",
            "biosVersion": "B20F21_A0",
            "bootOverride": "Once",
            "bootOverrideMode": "Legacy",
            "bootOverrideTarget": "Hdd",
            "bootOverrideSupported": [ "Hdd", "Pxe" ],
            "uefiTarget": "system_2_uefi_target",
            "powerState": "On",
            "assetTag": "tag",
            "cableIds": ["132"],
            "systemGuid": "guid123",
            "txtEnabled": true,
            "userModeEnabled": true,
            "sku": "123",
            "pciDevices": [
                {
                    "vendorId": "0x2112",
                    "deviceId": "0x1000"
                }
            ],
            "usbDevices": [],
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
                    "slotMask": "11"
                }
            ],
            "chassis": "none",
            "oem": {},
            "currentPerformanceConfiguration": null,
            "performanceConfigurations": []
        })";

static std::string Processor1 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "socket": "1151",
            "capabilities": [],
            "processorType": "CPU",
            "processorArchitecture": "x86",
            "instructionSet": "x86-64",
            "manufacturer": "Intel Corporation",
            "model": "I7",
            "modelName": "Cool Model Name",
            "cpuId": {
                "vendorId": "GenuineIntel",
                "numericId": "1001001",
                "family": "6",
                "model": "5",
                "step": "4",
                "microcodeInfo": "3"
            },
            "extendedCpuId": {
                "eax00h": "test",
                "eax01h": "test",
                "eax02h": "test",
                "eax03h": "test",
                "eax04h": "test",
                "eax05h": "test",
                "eax06h": "test",
                "eax07h": "test",
                "eax09h": "test",
                "eax0ah": "test",
                "eax0bh": "test",
                "eax0dh": "test",
                "eax0fh": "test",
                "eax10h": "test",
                "eax14h": "test",
                "eax15h": "test",
                "eax16h": "test",
                "eax17hecx00h": "test",
                "eax17hecx01h": "test",
                "eax17hecx02h": "test",
                "eax17hecx03h": "test",
                "eax80000000h": "test",
                "eax80000001h": "test",
                "eax80000002h": "test",
                "eax80000003h": "test",
                "eax80000004h": "test",
                "eax80000006h": "test",
                "eax80000007h": "test",
                "eax80000008h": "test"
            },
            "fpga": {
                "type": "Integrated",
                "bitStreamVersion": "1.0",
                "hssiConfiguration": "PCIe",
                "hssiSideband": "I2C",
                "reconfigurationSlots": 2,
                "erased" : true
            },
            "maxSpeedMHz": 2400,
            "totalCores": 16,
            "enabledCores": 16,
            "totalThreads": 32,
            "enabledThreads": 32,
            "thermalDesignPowerWatt": 1.0,
            "onPackageMemory": [],
            "oem": {}
        })";

static std::string Processor2 =
        R"({
            "status": {
               "health": "OK",
               "state": "Disabled"
            },
            "socket": "1151",
            "capabilities": [],
            "processorType": "CPU",
            "processorArchitecture": "x86",
            "instructionSet": "x86-64",
            "manufacturer": "Intel Corporation",
            "model": "I5",
            "modelName": "Cool Model Name",
            "cpuId": {
                "vendorId": "GenuineIntel",
                "numericId": "1001001",
                "family": "6",
                "model": "5",
                "step": "4",
                "microcodeInfo": "3"
            },
            "extendedCpuId": {
                "eax00h": "test",
                "eax01h": "test",
                "eax02h": "test",
                "eax03h": "test",
                "eax04h": "test",
                "eax05h": "test",
                "eax06h": "test",
                "eax07h": "test",
                "eax09h": "test",
                "eax0ah": "test",
                "eax0bh": "test",
                "eax0dh": "test",
                "eax0fh": "test",
                "eax10h": "test",
                "eax14h": "test",
                "eax15h": "test",
                "eax16h": "test",
                "eax17hecx00h": "test",
                "eax17hecx01h": "test",
                "eax17hecx02h": "test",
                "eax17hecx03h": "test",
                "eax80000000h": "test",
                "eax80000001h": "test",
                "eax80000002h": "test",
                "eax80000003h": "test",
                "eax80000004h": "test",
                "eax80000006h": "test",
                "eax80000007h": "test",
                "eax80000008h": "test"
            },
            "fpga": {
                "type": "Integrated",
                "bitStreamVersion": "1.0",
                "hssiConfiguration": "PCIe",
                "hssiSideband": "I2C",
                "reconfigurationSlots": 2,
                "erased" : false
            },
            "maxSpeedMHz": 1200,
            "totalCores": 4,
            "enabledCores": 3,
            "totalThreads": 8,
            "enabledThreads": 2,
            "thermalDesignPowerWatt": 2,
            "onPackageMemory": [],
            "oem": {}
        })";

static std::string Processor2Modified =
        R"({
            "status": {
               "health": "OK",
               "state": "Disabled"
            },
            "socket": "666",
            "capabilities": [],
            "processorType": "CPU",
            "processorArchitecture": "x86",
            "instructionSet": "x86-64",
            "manufacturer": "Intel Corporation",
            "model": "I5",
            "modelName": "Cool Model Name",
            "cpuId": {
                "vendorId": "GenuineIntel",
                "numericId": "1001001",
                "family": "6",
                "model": "5",
                "step": "4",
                "microcodeInfo": "3"
            },
            "extendedCpuId": {
                "eax00h": "test",
                "eax01h": "test",
                "eax02h": "test",
                "eax03h": "test",
                "eax04h": "test",
                "eax05h": "test",
                "eax06h": "test",
                "eax07h": "test",
                "eax09h": "test",
                "eax0ah": "test",
                "eax0bh": "test",
                "eax0dh": "test",
                "eax0fh": "test",
                "eax10h": "test",
                "eax14h": "test",
                "eax15h": "test",
                "eax16h": "test",
                "eax17hecx00h": "test",
                "eax17hecx01h": "test",
                "eax17hecx02h": "test",
                "eax17hecx03h": "test",
                "eax80000000h": "test",
                "eax80000001h": "test",
                "eax80000002h": "test",
                "eax80000003h": "test",
                "eax80000004h": "test",
                "eax80000006h": "test",
                "eax80000007h": "test",
                "eax80000008h": "test"
            },
            "fpga": {
                "type": "Integrated",
                "bitStreamVersion": "1.0",
                "hssiConfiguration": "PCIe",
                "hssiSideband": "I2C",
                "reconfigurationSlots": 2,
                "erased" : true
            },
            "maxSpeedMHz": 1200,
            "totalCores": 4,
            "enabledCores": 3,
            "totalThreads": 8,
            "enabledThreads": 2,
            "thermalDesignPowerWatt": 2,
            "onPackageMemory": [],
            "oem": {}
        })";

static std::string Manager2 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "type": "ManagementController",
            "guid": "guid",
            "parentId": "parent",
            "allowedActions": [],
            "model": null,
            "location": null,
            "firmwareVersion": "21122",
            "dateTime": "10H23",
            "dateTimeLocalOffset": "3",
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
                "typesSupported": []
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
                "maxSessions": 1,
                "typesSupported": []
            },
            "commandShell": {
                "enabled": null,
                "maxSessions": null,
                "typesSupported": []
            },
            "collections": [
                {
                    "name": "Systems",
                    "type": "Systems",
                    "slotMask": "11"
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
            "type": "Physical",
            "biosVersion": "A20F21_A0",
            "bootOverride": "Once",
            "bootOverrideMode": "Legacy",
            "bootOverrideTarget": "Hdd",
            "bootOverrideSupported": [ "Hdd", "Pxe" ],
            "uefiTarget": "system_3_uefi_target",
            "powerState": "On",
            "assetTag": "tag",
            "cableIds": ["132"],
            "systemGuid": "guid123",
            "txtEnabled": true,
            "userModeEnabled": true,
            "sku": "123",
            "pciDevices": [
                {
                    "vendorId": "0x2112",
                    "deviceId": "0x1000"
                }
            ],
            "usbDevices": [],
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
                    "slotMask": "11"
                }
            ],
            "chassis": "none",
            "oem": {},
            "currentPerformanceConfiguration": null,
            "performanceConfigurations": []
        })";

static std::string Processor1OnSystem3 =
        R"({
            "status": {
               "health": "OK",
               "state": "Disabled"
            },
            "socket": "1151",
            "capabilities": [],
            "processorType": "CPU",
            "processorArchitecture": "x86",
            "instructionSet": "x86-64",
            "manufacturer": "Intel Corporation",
            "model": "I3",
            "modelName": "Cool Model Name",
            "cpuId": {
                "vendorId": "GenuineIntel",
                "numericId": "1001001",
                "family": "6",
                "model": "5",
                "step": "4",
                "microcodeInfo": "3"
            },
            "extendedCpuId": {
                "eax00h": "test",
                "eax01h": "test",
                "eax02h": "test",
                "eax03h": "test",
                "eax04h": "test",
                "eax05h": "test",
                "eax06h": "test",
                "eax07h": "test",
                "eax09h": "test",
                "eax0ah": "test",
                "eax0bh": "test",
                "eax0dh": "test",
                "eax0fh": "test",
                "eax10h": "test",
                "eax14h": "test",
                "eax15h": "test",
                "eax16h": "test",
                "eax17hecx00h": "test",
                "eax17hecx01h": "test",
                "eax17hecx02h": "test",
                "eax17hecx03h": "test",
                "eax80000000h": "test",
                "eax80000001h": "test",
                "eax80000002h": "test",
                "eax80000003h": "test",
                "eax80000004h": "test",
                "eax80000006h": "test",
                "eax80000007h": "test",
                "eax80000008h": "test"
            },
            "fpga": {
                "type": "Integrated",
                "bitStreamVersion": "1.0",
                "hssiConfiguration": "PCIe",
                "hssiSideband": "I2C",
                "reconfigurationSlots": 2,
                "erased" : true
            },
            "maxSpeedMHz": 1200,
            "totalCores": 2,
            "enabledCores": 1,
            "totalThreads": 2,
            "enabledThreads": 2,
            "thermalDesignPowerWatt": 5.5,
            "onPackageMemory": [],
            "oem": {}
        })";

static std::string Processor1OnSystem3Modified =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "socket": "1151",
            "capabilities": [],
            "processorType": "CPU",
            "processorArchitecture": "x86",
            "instructionSet": "x86-64",
            "manufacturer": "Intel Corporation",
            "model": "I3",
            "modelName": "Cool Model Name",
            "cpuId": {
                "vendorId": "GenuineIntel",
                "numericId": "1001001",
                "family": "6",
                "model": "5",
                "step": "4",
                "microcodeInfo": "3"
            },
            "extendedCpuId": {
                "eax00h": "test",
                "eax01h": "test",
                "eax02h": "test",
                "eax03h": "test",
                "eax04h": "test",
                "eax05h": "test",
                "eax06h": "test",
                "eax07h": "test",
                "eax09h": "test",
                "eax0ah": "test",
                "eax0bh": "test",
                "eax0dh": "test",
                "eax0fh": "test",
                "eax10h": "test",
                "eax14h": "test",
                "eax15h": "test",
                "eax16h": "test",
                "eax17hecx00h": "test",
                "eax17hecx01h": "test",
                "eax17hecx02h": "test",
                "eax17hecx03h": "test",
                "eax80000000h": "test",
                "eax80000001h": "test",
                "eax80000002h": "test",
                "eax80000003h": "test",
                "eax80000004h": "test",
                "eax80000006h": "test",
                "eax80000007h": "test",
                "eax80000008h": "test"
            },
            "fpga": {
                "type": "Integrated",
                "bitStreamVersion": "1.0",
                "hssiConfiguration": "PCIe",
                "hssiSideband": "I2C",
                "reconfigurationSlots": 2,
                "erased" : true
            },
            "maxSpeedMHz": 1200,
            "totalCores": 2,
            "enabledCores": 1,
            "totalThreads": 2,
            "enabledThreads": 2,
            "thermalDesignPowerWatt": 5.5,
            "onPackageMemory": [],
            "oem": {}
        })";

static std::string System4 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "type": "Physical",
            "biosVersion": "A20F21_A0",
            "bootOverride": "Once",
            "bootOverrideMode": "Legacy",
            "bootOverrideTarget": "Hdd",
            "bootOverrideSupported": [ "Hdd", "Pxe" ],
            "uefiTarget": "system_4_uefi_target",
            "powerState": "On",
            "assetTag": "tag",
            "cableIds": ["132"],
            "systemGuid": "guid123",
            "txtEnabled": true,
            "userModeEnabled": true,
            "sku": "123",
            "pciDevices": [
                {
                    "vendorId": "0x2112",
                    "deviceId": "0x1000"
                }
            ],
            "usbDevices": [],
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
                    "slotMask": "11"
                }
            ],
            "chassis": "none",
            "oem": {},
            "currentPerformanceConfiguration": null,
            "performanceConfigurations": []
        })";

static std::string Processor1OnSystem4 =
        R"({
            "status": {
               "health": "OK",
               "state": "Disabled"
            },
            "socket": "1158",
            "capabilities": [],
            "processorType": "CPU",
            "processorArchitecture": "x86",
            "instructionSet": "x86-64",
            "manufacturer": "Intel Corporation",
            "model": "I3",
            "modelName": "Cool Model Name",
            "cpuId": {
                "vendorId": "GenuineIntel",
                "numericId": "1001001",
                "family": "6",
                "model": "5",
                "step": "4",
                "microcodeInfo": "3"
            },
            "extendedCpuId": {
                "eax00h": "test",
                "eax01h": "test",
                "eax02h": "test",
                "eax03h": "test",
                "eax04h": "test",
                "eax05h": "test",
                "eax06h": "test",
                "eax07h": "test",
                "eax09h": "test",
                "eax0ah": "test",
                "eax0bh": "test",
                "eax0dh": "test",
                "eax0fh": "test",
                "eax10h": "test",
                "eax14h": "test",
                "eax15h": "test",
                "eax16h": "test",
                "eax17hecx00h": "test",
                "eax17hecx01h": "test",
                "eax17hecx02h": "test",
                "eax17hecx03h": "test",
                "eax80000000h": "test",
                "eax80000001h": "test",
                "eax80000002h": "test",
                "eax80000003h": "test",
                "eax80000004h": "test",
                "eax80000006h": "test",
                "eax80000007h": "test",
                "eax80000008h": "test"
            },
            "fpga": {
                "type": "Integrated",
                "bitStreamVersion": "1.0",
                "hssiConfiguration": "PCIe",
                "hssiSideband": "I2C",
                "reconfigurationSlots": 2,
                "erased" : false
            },
            "maxSpeedMHz": 1200,
            "totalCores": 2,
            "enabledCores": 1,
            "totalThreads": 2,
            "enabledThreads": 2,
            "thermalDesignPowerWatt": 1,
            "onPackageMemory": [],
            "oem": {}
        })";

static std::string Manager3 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "type": "ManagementController",
            "guid": "guid",
            "parentId": "parent",
            "allowedActions": [],
            "model": null,
            "location": null,
            "firmwareVersion": "2112",
            "dateTime": "10H23",
            "dateTimeLocalOffset": "3",
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
                "typesSupported": []
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
                "maxSessions": 1,
                "typesSupported": []
            },
            "commandShell": {
                "enabled": null,
                "maxSessions": null,
                "typesSupported": []
            },
            "collections": [
                {
                    "name": "FastSystems",
                    "type": "Systems",
                    "slotMask": "11"
                },
                {
                    "name": "SlowSystems",
                    "type": "Systems",
                    "slotMask": "11"
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
            "type": "Physical",
            "biosVersion": "A20F21_A0",
            "bootOverride": "Once",
            "bootOverrideMode": "Legacy",
            "bootOverrideTarget": "Hdd",
            "bootOverrideSupported": [ "Hdd", "Pxe" ],
            "uefiTarget": "system_4_uefi_target",
            "powerState": "On",
            "assetTag": "tag",
            "cableIds": ["132"],
            "systemGuid": "guid123",
            "txtEnabled": true,
            "userModeEnabled": true,
            "sku": "123",
            "pciDevices": [
                {
                    "vendorId": "0x2112",
                    "deviceId": "0x1000"
                }
            ],
            "usbDevices": [],
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
                    "type": "Processors",
                    "slotMask": "11"
                },
                {
                    "name": "SlowProcessors",
                    "type": "Processors",
                    "slotMask": "11"
                }
            ],
            "chassis": "none",
            "oem": {},
            "currentPerformanceConfiguration": null,
            "performanceConfigurations": []
        })";

static std::string System6 =
        R"({
            "status": {
               "health": "OK",
               "state": "Enabled"
            },
            "type": "Physical",
            "biosVersion": "A20F21_A0",
            "bootOverride": "Once",
            "bootOverrideMode": "Legacy",
            "bootOverrideTarget": "Hdd",
            "bootOverrideSupported": [ "Hdd", "Pxe" ],
            "uefiTarget": "system_5_uefi_target",
            "powerState": "On",
            "assetTag": "tag",
            "cableIds": ["132"],
            "systemGuid": "guid123",
            "txtEnabled": true,
            "userModeEnabled": true,
            "sku": "123",
            "pciDevices": [
                {
                    "vendorId": "0x2112",
                    "deviceId": "0x1000"
                }
            ],
            "usbDevices": [],
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
                    "type": "Processors",
                    "slotMask": "11"
                },
                {
                    "name": "whAtever",
                    "type": "Processors",
                    "slotMask": "11"
                }
            ],
            "chassis": "none",
            "oem": {},
            "currentPerformanceConfiguration": null,
            "performanceConfigurations": []
        })";

static std::string Task1 =
    R"({
        "status": {
           "health": "OK",
           "state": "Enabled"
        },
        "name" : "test task",
        "startTime" : "then",
        "endTime" : "now",
        "state" : "Exception",
        "messages" : [],
        "oem": {}
    })";
