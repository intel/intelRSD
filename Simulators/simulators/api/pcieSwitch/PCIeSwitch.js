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

const BaseAsset = require('../common/baseAsset').BaseAsset;
const Collection = require('./../common/collection').Collection;
const PCIeZone = require('./PCIeZone').PCIeZone;
const PCIePort = require('./PCIePort').PCIePort;
const OdataIdArray = require('./../common/odataIdArray').OdataIdArray;

class PCIeSwitch extends BaseAsset {
    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);
        this.CreateCollections();
        this.MockGetWithDynamicBody();

        this.managedBy = new OdataIdArray();
        this.attachedDevices = new OdataIdArray();
        this.chassis = new OdataIdArray();
    }

    GetBaseUri() {
        return super.GetBaseUri('PCIeSwitches');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#PCIeSwitches/Members/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#PCIeSwitch.v0_9_0.PCIeSwitch',
            'Id': this.Id,
            'Name': 'PCIe Switch',
            'Description': 'Storage PCIe Switch',
            'Manufacturer': 'Manufacturer Name',
            'Model': 'Model Name',
            'SerialNumber': '2M220100SL',
            'PartNumber': null,
            'SKU': 'SKU name',
            'IndicatorLED': null,
            'PowerState': 'On',
            'Status': {
                'State': 'Enabled',
                'Health': 'OK',
                'HealthRollup': 'OK'
            },

            'MaxZones': 8,
            'Zones': {
                '@odata.id': this.GetBaseUri() + '/PCIeZones'
            },
            'PCIePorts': {
                '@odata.id': this.GetBaseUri() + '/PCIePorts'
            },
            'AttachedDevices': this.AttachedDevices || [],
            'LogServices': {
                '@odata.id': this.GetBaseUri() + '/Logs'
            },
            'Links': {
                'Chassis': this.Chassis || [],
                'ManagedBy': this.ManagedBy || [],
                'PoweredBy': [],
                'CooledBy': [],
                'Oem': {}
            },
            'Actions': {
                '#PCIeSwitch.Reset': {
                    'target': this.GetBaseUri() + '/Actions/PCIeSwitch.Reset',
                    'ResetType@Redfish.AllowableValues': [] //TODO: PNC Switch reset action is not supported yet
                },
                'Oem': {}
            },
            'Oem': {}
        };
    }

    CreateCollections() {
        this.pcieZones = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#PCIeSwitches/Members/' + this.Id + '/PCIeZones$entity',
            '@odata.id': this.GetBaseUri() + '/PCIeZones',
            '@odata.type': '#PCIeZoneCollection.PCIeZoneCollection',
            'Name': 'PCIeZone Collection',
            'Description': 'PCIeZone Collection',
            'Context': this,
            'Resource': PCIeZone
        });

        this.pciePorts = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#PCIeSwitches/Members/' + this.Id + '/PCIePorts$entity',
            '@odata.id': this.GetBaseUri() + '/PCIePorts',
            '@odata.type': '#PCIePortCollection.PCIePortCollection',
            'Name': 'Port Collection',
            'Description': 'Port Collection',
            'Context': this,
            'Resource': PCIePort
        });
    }

    get ManagedBy() {
        return this.managedBy;
    }

    AddManagedBy(manager) {
        this.managedBy.Add(manager);
        return this;
    }

    get AttachedDevices() {
        return this.attachedDevices;
    }

    AddAttachedDevice(device) {
        this.attachedDevices.Add(device);
        return this;
    }

    get Chassis() {
        return this.chassis;
    }

    AddChassis(chassis) {
        this.chassis.Add(chassis);
        return this;
    }

    get PCIeZones() {
        return this.pcieZones;
    }

    AddPCIeZone() {
        return this.pcieZones.Add();
    }

    get PCIePorts() {
        return this.pciePorts;
    }

    AddPCIePort() {
        return this.pciePorts.Add();
    }
}

exports.PCIeSwitch = PCIeSwitch;