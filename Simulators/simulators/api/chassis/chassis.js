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
const Collection = require('../common/collection.js').Collection;
const PowerZone = require('./powerZone').PowerZone;
const ThermalZone = require('./thermalZone').ThermalZone;
const extend = require('util')._extend;
const OdataIdArray = require('../common/odataIdArray').OdataIdArray;
const Drive = require('./drive').Drive;
const Thermal = require('./thermal/thermal').Thermal;
const PCIeDevice = require('../pcieSwitch/PCIeDevice').PCIeDevice;
const httpStatusCode = require('../../utils/httpStatusCode').httpStatusCode;
const Power = require('./power/power').Power;

class Chassis extends BaseAsset {
    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);
        this.CreateCollections();
        this.MockGetWithDynamicBody();
        this.thermal = new Thermal(mock, this.GetBaseUri());
        this.power = new Power(mock, this.GetBaseUri());
        this.contains = new OdataIdArray();
        this.computerSystems = new OdataIdArray();
        this.switches = new OdataIdArray();
        this.managedBy = new OdataIdArray();
        this.managersInChassis = new OdataIdArray();
        this.pcieSwitches = new OdataIdArray();
        this.storages = new OdataIdArray();
        this.mock = mock;
        this.MockPatch();
    }

    MockPatch() {
        this.mock.mockPatch(this.GetBaseUri(),
            httpStatusCode.OK,
            undefined,
            (req, response) => {
                var jsonRequest = JSON.parse(req.body);
                this.AssetTag = jsonRequest.AssetTag || this.AssetTag;

                response.body = this.CreateJsonTemplate();
                return response;
            }
        );
    }

    DenyPatch() {
        this.mock.mockPatch(this.GetBaseUri(),
            httpStatusCode.NOT_IMPLEMENTED,
            undefined,
            (req, response) => {
                return response;
            }
        );
    }

    GetBaseUri() {
        return super.GetBaseUri('Chassis');
    }

    CreateJsonTemplate() {
        var defaultJson = {
            '@odata.context': '/redfish/v1/$metadata#Chassis/Members/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Chassis.v1_3_0.Chassis',
            'Id': this.Id,
            'ChassisType': this.ChassisType || 'Other',
            'Name': this.Name || 'name-as-string',
            'Description': this.Description || 'description-as-string',
            'Manufacturer': this.Manufacturer || '',
            'Model': this.Model || '',
            'SKU': 'sku-as-string',
            'SerialNumber': this.SerialNumber || '',
            'PartNumber': 'part-number-as-string',
            'AssetTag': this.AssetTag || null,
            'IndicatorLED': null,
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'Oem': {
                'Intel_RackScale': {
                    '@odata.type': '#Intel.Oem.Chassis',
                    'Location': {
                        'Id': this.Id,
                        'ParentId': this.ParentId || null
                    }
                }
            },
            'Thermal': {
                '@odata.id': this.Thermal.GetBaseUri()
            },
            'Power' : {
                '@odata.id': this.Power.GetBaseUri()
            },
            'Links': {
                'Contains': this.Contains,
                'ContainedBy': this.ContainedBy || null,
                'ComputerSystems': this.ComputerSystems,

                'ManagedBy': this.ManagedBy,
                'ManagersInChassis': this.ManagersInChassis,
                'Storage': this.Storages || [],
                'Drives': this.drivesCollection.GetODataIdsCollection(),
                'Oem': {
                    'Intel_RackScale' : {
                        '@odata.type' : '#Intel.Oem.ChassisLinks',
                        'PCIeSwitches': this.PCIeSwitches,
                        'Switches': this.Switches
                    }
                }
            }
        };

        if (this.ChassisType == 'Rack') {
            return this.addRackSpecificValues(defaultJson);
        }

        return defaultJson;
    }

    addRackSpecificValues(json) {
        var rackAdditionalJsonPart = {
            'Oem': {
                'Intel_RackScale': {
                    'RMMPresent': true,
                    'RackSupportsDisaggregatedPowerCooling': true,
                    'UUID': this.Uuid,
                    'GeoTag': '54.348103, 18.645172'
                }
            },
            'ThermalZones': {
                '@odata.id': this.GetBaseUri() + '/ThermalZones'
            },
            'PowerZones': {
                '@odata.id': this.GetBaseUri() + '/PowerZones'
            }
        };

        rackAdditionalJsonPart.Oem.Intel_RackScale = extend(rackAdditionalJsonPart.Oem.Intel_RackScale, json.Oem.Intel_RackScale);
        return extend(json, rackAdditionalJsonPart);
    }

    set Status(val) {
        this.status = val
    }

    get Status() {
        return this.status;
    }

    get ChassisType() {
        return this.chassisType;
    }

    set ChassisType(chassisType) {
        this.chassisType = chassisType;
    }

    get AssetTag() {
        return this.assetTag;
    }

    set AssetTag(assetTag) {
        this.assetTag = assetTag;
    }

    get Description() {
        return this.description;
    }

    set Description(description) {
        this.description = description;
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

    get ComputerSystems() {
        return this.computerSystems;
    }

    set ComputerSystems(elements) {
        this.computerSystems.Clear();
        this.AddToComputerSystems(elements);
    }

    AddToComputerSystems(elements) {
        this.computerSystems.Add(elements, (element) => {
            element.Chassis = this;
        });
        return this;
    }

    UnlinkComputerSystem(odataId) {
        this.computerSystems.Remove(odataId);
    }

    get Switches() {
        return this.switches;
    }

    set Switches(elements) {
        this.switches.Clear();
        this.AddToSwitches(elements);
    }

    AddToSwitches(elements) {
        this.switches.Add(elements);

        return this;
    }

    get Contains() {
        return this.contains;
    }

    set Contains(elements) {
        this.contains.Clear();
        this.AddToContains(elements);
    }

    AddToContains(elements) {
        this.contains.Add(elements, (element) => {
            element.ContainedBy = this;
            element.ParentId = this.Id;
        });

        return this;
    }

    RemoveFromContains(odataId) {
        this.contains.Remove(odataId);

        return this;
    }

    set Storages(val) {
        this.storages = val;
    }

    get Storages() {
        return this.storages;
    }

    AddStorage(val) {
        this.storages.AddWithODataId(val.GetBaseUri());
    }

    get ParentId() {
        return this.parentId;
    }

    set ParentId(id) {
        this.parentId = id;
    }

    get ContainedBy() {
        return this.containedBy;
    }

    set ContainedBy(containedBy) {
        this.containedBy = OdataIdArray.OdataIdForAsset(containedBy);
    }

    get ManagedBy() {
        return this.managedBy;
    }

    set ManagedBy(elements) {
        this.managedBy.Clear();
        this.AddManagedBy(elements);
    }

    AddManagedBy(elements) {
        this.managedBy.Add(elements);
        return this;
    }

    get PCIeSwitches() {
        return this.pcieSwitches;
    }

    AddPCIeSwitch(pcieSwitch) {
        this.pcieSwitches.Add(pcieSwitch);
        return this;
    }

    get ManagersInChassis() {
        return this.managersInChassis;
    }

    AddManagersInChassis(elements) {
        this.managersInChassis.Add(elements);
        return this;
    }

    get Drives() {
        return this.drivesCollection;
    }

    AddDrive() {
        return this.drivesCollection.AddWithPrefixAndResource('Drive', Drive);
    }

    CreateCollections() {
        this.thermalZoneCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#Chassis/Rack/ThermalZones',
            '@odata.id': this.GetBaseUri() + '/ThermalZones',
            '@odata.type': 'http://rackscale.intel.com/schema#Intel.Oem.ThermalZoneCollection',
            'Name': 'Thermal Zones Collection',
            'Context': this,
            'Resource': ThermalZone
        });
        this.powerZoneCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#Chassis/Rack/PowerZones',
            '@odata.id': this.GetBaseUri() + '/PowerZones',
            '@odata.type': 'http://rackscale.intel.com/schema#Intel.Oem.PowerZoneCollection',
            'Name': 'Power Zones Collection',
            'Context': this,
            'Resource': PowerZone
        });
        this.drivesCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#Chassis/Members/Drives/$entity',
            '@odata.id': this.GetBaseUri() + '/Drives',
            '@odata.type': '#StorageDriveCollection.StorageDriveCollection',
            'Name': 'Drive Collection',
            'Context': this,
            'Resource': BaseAsset
        });
        this.pcieDevicesCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#PCIeDevices/$entity',
            '@odata.id': this.GetBaseUri() + '/PCIeDevices',
            '@odata.type': '#PCIeDeviceCollection.PCIeDeviceCollection',
            'Name': 'PCIe Device Collection',
            'Description': 'PCIe Device Collection',
            'Context': this,
            'Resource': PCIeDevice
        });

    }

    get PCIeDevices() {
        return this.pcieDevicesCollection;
    }

    AddPCIeDevice() {
        var pcieDevice = this.pcieDevicesCollection.Add();
        pcieDevice.AddChassis(this);
        return pcieDevice;
    }

    AddThermalZone() {
        return this.thermalZoneCollection.AddWithId('thermal1');
    }

    AddPowerZone() {
        return this.powerZoneCollection.AddWithId('power1');
    }

    get Thermal() {
        return this.thermal;
    }

    get Power() {
        return this.power;
    }
}

exports.Chassis = Chassis;
