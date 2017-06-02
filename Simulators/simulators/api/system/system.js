/*
 * Copyright (c) 2015-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

const Processor = require('./processor').Processor;
const Memory = require('./memory').Memory;
const EthernetInterface = require('./ethernetInterface').EthernetInterface;
const SimpleStorage = require('./simpleStorage').SimpleStorage;
const Collection = require('./../common/collection').Collection;
const BaseAsset = require('../common/baseAsset').BaseAsset;
const Storage = require('./storage').Storage;
const NetworkInterface = require('./networkInterface').NetworkInterface;
const httpStatusCode = require('../../utils/httpStatusCode').httpStatusCode;
const util = require('util');
const OdataIdArray = require('../common/odataIdArray').OdataIdArray;

class System extends BaseAsset {
    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);

        this.status = {
            'State': 'Enabled',
            'Health': 'OK',
            'HealthRollup': 'OK'
        };

        this.pcieDevicesCollection = new OdataIdArray();
        this.pcieFunctionsCollection = new OdataIdArray();
        this.endpoints = new OdataIdArray();
        this.pcieConnectionIds = [];
        this.relatedItemsCollection = [];

        this.CreateCollections();
        this.MockGetWithDynamicBody();
        mock.mockPatch(this.GetBaseUri(),
            httpStatusCode.OK,
            undefined,
            (req, response) => {
                var jsonRequest = JSON.parse(req.body);
                this.AssetTag = jsonRequest.AssetTag || this.AssetTag;

                if (jsonRequest.Boot != null) {
                    this.BootSourceOverrideEnabled = jsonRequest.Boot.BootSourceOverrideEnabled || this.BootSourceOverrideEnabled;
                    this.BootSourceOverrideTarget = jsonRequest.Boot.BootSourceOverrideTarget || this.BootSourceOverrideTarget;
                    this.BootSourceOverrideMode = jsonRequest.Boot.BootSourceOverrideMode || this.BootSourceOverrideMode;
                }
                response.body = this.CreateJsonTemplate();
                return response;
            }
        );

        mock.mockPost(this.GetBaseUri() + '/Actions/ComputerSystem.Reset',
            httpStatusCode.NO_CONTENT,
            undefined,
            (req, response) => {
                var jsonRequest = JSON.parse(req.body);
                this.Reset(jsonRequest.ResetType);
                return response;
            }
        );
    }

    GetBaseUri() {
        return super.GetBaseUri('Systems');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#Systems/Members/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#ComputerSystem.v1_1_0.ComputerSystem',
            'Id': this.Id,
            'Name': this.Name || null,
            'SystemType': this.SystemType || 'Physical',
            'AssetTag': this.AssetTag || null,
            'Manufacturer': this.Manufacturer || null,
            'Model': this.Model || null,
            'SKU': this.Sku || null,
            'SerialNumber': this.SerialNumber || null,
            'PartNumber': this.PartNumber || null,
            'Description': this.Description || null,
            'UUID': this.Uuid || null,
            'HostName': 'web-srv344',
            'Status': this.Status,
            'IndicatorLED': 'Off',
            'PowerState': this.PowerState || 'On',
            'Boot': {
                '@odata.type': '#ComputerSystem.v1_2_0.Boot',
                'BootSourceOverrideEnabled': this.BootSourceOverrideEnabled || 'Once',
                'BootSourceOverrideTarget': this.BootSourceOverrideTarget || 'Pxe',
                'BootSourceOverrideTarget@Redfish.AllowableValues': [
                    'None',
                    'Pxe',
                    'Floppy',
                    'Cd',
                    'Usb',
                    'Hdd',
                    'BiosSetup',
                    'Utilities',
                    'Diags',
                    'UefiTarget'
                ],
                'BootSourceOverrideMode': this.BootSourceOverrideMode || null,
                'BootSourceOverrideMode@Redfish.AllowableValues': [
                    'Legacy',
                    'UEFI'
                ]
            },
            'BiosVersion': 'P79 v1.00 (09/20/2013)',
            'ProcessorSummary': {
                'Count': 1,
                'Model': this.ProcessorModel || null,
                'Status': {
                    'State': 'Enabled',
                    'Health': 'OK',
                    'HealthRollup': 'OK'
                }
            },
            'MemorySummary': {
                'TotalSystemMemoryGiB': this.TotalSystemMemoryGiB,
                'Status': {
                    'State': 'Enabled',
                    'Health': 'OK',
                    'HealthRollup': 'OK'
                }
            },
            'Processors': {
                '@odata.id': this.GetBaseUri() + '/Processors'
            },
            'EthernetInterfaces': {
                '@odata.id': this.GetBaseUri() + '/EthernetInterfaces'
            },
            'SimpleStorage': {
                '@odata.id': this.GetBaseUri() + '/SimpleStorage'
            },
            'Storage': {
                '@odata.id': this.GetBaseUri() + '/Storage'
            },
            'Memory': {
                '@odata.id': this.GetBaseUri() + '/Memory'
            },
            'PCIeDevices': this.PCIeDevices || [],
            'PCIeFunctions': this.PCIeFunctions || [],
            'NetworkInterfaces': {
                '@odata.id': this.GetBaseUri() + '/NetworkInterfaces'
            },
            'Links': {
                'Chassis': this.Chassis || [],
                'ManagedBy': this.ManagedBy || [],
                'Endpoints': this.Endpoints || [],
                'Oem': {}
            },
            'Actions': {
                '#ComputerSystem.Reset': {
                    'target': this.GetBaseUri() + '/Actions/ComputerSystem.Reset',
                    'ResetType@Redfish.AllowableValues': [
                        'On',
                        'ForceOff',
                        'GracefulShutdown',
                        'ForceRestart',
                        'Nmi',
                        'GracefulRestart',
                        'ForceOn',
                        'PushPowerButton'
                    ]
                },
                'Oem': {
                    'Intel_RackScale': {
                        '#ComputerSystem.StartDeepDiscovery': {
                            'target': this.GetBaseUri() + '/Actions/ComputerSystem.StartDeepDiscovery'
                        }
                    }
                }
            },
            'Oem': {
                'Intel_RackScale': {
                    '@odata.type': '#Intel.Oem.ComputerSystem',
                    'PciDevices': this.pciDevices || [],
                    'DiscoveryState': this.discoveryState || 'Basic',
                    'ProcessorSockets': 8,
                    'MemorySockets': 8,
                    'PCIeConnectionId': this.PcieConnectionIds || []
                }
            }
        };
    }

    CreateCollections() {
        this.processorsCollection = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#Systems/Members/1/Processors/#entity',
            '@odata.id': this.GetBaseUri() + '/Processors',
            '@odata.type': '#ProcessorCollection.ProcessorCollection',
            'Name': 'Processors Collection',
            'Context': this,
            'Resource': Processor
        });

        this.memoryCollection = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#Systems/Members/1/Processors/#entity',
            '@odata.id': this.GetBaseUri() + '/Memory',
            '@odata.type': '#MemoryCollection.MemoryCollection',
            'Name': 'Memory Collection',
            'Context': this,
            'Resource': Memory
        });

        this.simpleStorageCollection = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#SimpleStorageCollection.SimpleStorageCollection',
            '@odata.id': this.GetBaseUri() + '/SimpleStorage',
            '@odata.type': '#SimpleStorageCollection.SimpleStorageCollection',
            'Name': 'Simple Storage Collection',
            'Context': this,
            'Resource': SimpleStorage
        });

        this.ethernetInterfacesCollection = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#EthernetInterfaceCollection.EthernetInterfaceCollection',
            '@odata.id': this.GetBaseUri() + '/EthernetInterfaces',
            '@odata.type': '#EthernetInterfaceCollection.EthernetInterfaceCollection',
            'Name': 'Ethernet Interfaces Collection',
            'Context': this,
            'Resource': EthernetInterface
        });

        this.storageCollection = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#StorageCollection.StorageCollection',
            '@odata.id': this.GetBaseUri() + '/Storage',
            '@odata.type': '#StorageCollection.StorageCollection',
            'Name': 'Storage Collection',
            'Context': this,
            'Resource': Storage
        });

        this.networkInterfacesCollection = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#Systems/Members/1/NetworkInterfaces/#entity',
            '@odata.id': this.GetBaseUri() + '/NetworkInterfaces',
            '@odata.type': '#NetworkInterfaceCollection.NetworkInterfaceCollection',
            'Name': 'Network Interfaces Collection',
            'Context': this,
            'Resource': NetworkInterface
        });
    }

    AddComputerSystemRelatedItem(relatedItem){
        this.relatedItemsCollection.push(relatedItem);
    }

    UnlinkComputerSystemFromConnectedAssets() {
        for (var i = 0; i < this.relatedItemsCollection.length; i++) {
            this.relatedItemsCollection[i].UnlinkComputerSystem(this.GetBaseUri());
        }
    }

    get Status() {
        return this.status;
    }

    set Status(status) {
        this.status = status;
    }

    get PartNumber() {
        return this.partNumber;
    }

    set PartNumber(partNumber) {
        this.partNumber = partNumber;
    }

    get ProcessorModel() {
        return this.processorModel;
    }

    set ProcessorModel(processorModel) {
        this.processorModel = processorModel;
    }

    get PcieConnectionIds() {
        return this.pcieConnectionIds;
    }

    set PcieConnectionIds(pcieConnectionIds) {
        this.pcieConnectionIds = pcieConnectionIds;
    }

    AddPcieConnectionId(pcieConnectionId) {
        this.pcieConnectionIds.push(pcieConnectionId);
    }

    get TotalSystemMemoryGiB() {
        return this.totalSystemMemoryGiB;
    }

    set TotalSystemMemoryGiB(totalSystemMemoryGiB) {
        this.totalSystemMemoryGiB = totalSystemMemoryGiB;
    }

    get Description() {
        return this.description;
    }

    set Description(description) {
        this.description = description;
    }

    get Sku() {
        return this.sku;
    }

    set Sku(sku) {
        this.sku = sku;
    }

    get AssetTag() {
        return this.assetTag;
    }

    set AssetTag(assetTag) {
        this.assetTag = assetTag;
    }

    get SystemType() {
        return this.systemType;
    }

    set SystemType(systemType) {
        this.systemType = systemType;
    }

    get Manufacturer() {
        return this.manufacturer;
    }

    set Manufacturer(manufacturer) {
        this.manufacturer = manufacturer;
    }

    get Model() {
        return this.model;
    }

    set Model(model) {
        this.model = model;
    }

    get SerialNumber() {
        return this.serialNumber;
    }

    set SerialNumber(serialNumber) {
        this.serialNumber = serialNumber;
    }

    get PciDevices() {
        return this.pciDevices;
    }

    set PciDevices(pciDevices) {
        this.pciDevices = pciDevices;
    }

    get PCIeFunctions() {
        return this.pcieFunctionsCollection;
    }

    set PCIeFunctions(pcieFunctions) {
        this.pcieFunctionsCollection = pcieFunctions;
    }

    AddPCIeFunction(pcieFunction) {
        this.pcieFunctionsCollection.AddWithODataId(pcieFunction.GetBaseUri());
    }

    set PCIeDevices(pcieDevices) {
        this.pcieDevicesCollection = pcieDevices;
    }

    get PCIeDevices() {
        return this.pcieDevicesCollection;
    }

    AddPCIeDevice(pcieDevice) {
        this.pcieDevicesCollection.AddWithODataId(pcieDevice.GetBaseUri());
    }

    get DiscoveryState() {
        return this.discoveryState;
    }

    set DiscoveryState(discoveryState) {
        this.discoveryState = discoveryState;
    }

    get Processors() {
        return this.processorsCollection;
    }

    get EthernetInterfaces() {
        return this.ethernetInterfacesCollection;
    }

    get MemoryCollection() {
        return this.memoryCollection;
    }

    get BootSourceOverrideEnabled() {
        return this.bootSourceOverrideEnabled;
    }

    set BootSourceOverrideEnabled(bootSourceOverrideEnabled) {
        this.bootSourceOverrideEnabled = bootSourceOverrideEnabled;
    }

    get BootSourceOverrideTarget() {
        return this.bootSourceOverrideTarget;
    }

    set BootSourceOverrideTarget(bootSourceOverrideTarget) {
        this.bootSourceOverrideTarget = bootSourceOverrideTarget;
    }

    get BootSourceOverrideMode() {
        return this.bootSourceOverrideMode;
    }

    set BootSourceOverrideMode(bootSourceOverrideMode) {
        this.bootSourceOverrideMode = bootSourceOverrideMode;
    }

    AddProcessor() {
        return this.processorsCollection.Add();
    }

    AddMemory() {
        return this.memoryCollection.Add();
    }

    AddSimpleStorage() {
        return this.simpleStorageCollection.Add();
    }

    get SimpleStorage() {
        return this.simpleStorageCollection;
    }

    AddStorage() {
        return this.storageCollection.Add();
    }

    get StorageCollection() {
        return this.storageCollection;
    }

    AddNetworkInterface() {
        return this.networkInterfacesCollection.Add();
    }

    get NetworkInterfaces() {
        return this.networkInterfacesCollection;
    }

    AddEthernetInterface(macAddress) {
        return this.ethernetInterfacesCollection.Add({'MACAddress': macAddress});
    }

    get Chassis() {
        return this.chassis;
    }

    set Chassis(chassis) {
        this.chassis = {'@odata.id': chassis.GetBaseUri()};
        this.relatedItemsCollection.push(chassis);
    }

    get ManagedBy() {
        return this.managedBy;
    }

    AddManagedBy(element) {
        this.managedBy = this.managedBy || [];
        this.managedBy.push({'@odata.id': element.GetBaseUri()});
        this.relatedItemsCollection.push(element);
    }

    get Endpoints() {
        return this.endpoints;
    }

    AddEndpoint(endpoint) {
        this.endpoints.Add(endpoint);
    }

    get PowerState() {
        return this.powerState;
    }

    set PowerState(powerState) {
        this.powerState = powerState;
    }

    Reset(resetType) {
        setTimeout(() => {
            var oldPowerState = this.PowerState;
            switch (resetType) {
                case 'On':
                case 'ForceOn':
                    this.PowerState = 'On';
                    break;
                case 'ForceOff':
                case 'GracefulShutdown':
                    this.PowerState = 'Off';
                    break;
            }

            if (oldPowerState != this.PowerState) {
                this.Parent.EventTrigger.FireResourceUpdatedEvent(this.GetBaseUri()); //FIXME: dirty hack
            }
        }, 5000);
    }
}

exports.System = System;
