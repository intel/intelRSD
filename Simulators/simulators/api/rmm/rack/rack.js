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

const BaseAsset = require('../../common/baseAsset').BaseAsset;
const Collection = require('../../common/collection').Collection;
const PowerZone = require('./powerZone').PowerZone;
const ThermalZone = require('./thermalZone').ThermalZone;
const Thermal = require('./../../chassis/thermal/thermal').Thermal;
const Power = require('./../../chassis/power/power').Power;
const Mbp = require('./mbp').Mbp;
const OdataIdArray = require('../../common/odataIdArray').OdataIdArray;
const httpStatusCode = require('../../../utils/httpStatusCode').httpStatusCode;

const UINT_FOUR_BYTE_MAX = 4294967295;

class Rack extends BaseAsset {
    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);
        this.managedBy = new OdataIdArray();
        this.contains = new OdataIdArray();

        this.rackPuid = 0;
        this.locationId = '0';
        this.locationPropertyVisible = true;
        this.CreateCollections();
        this.thermal = new Thermal(mock, this.GetBaseUri());
        this.power = new Power(mock, this.GetBaseUri());
        this.MockGetWithDynamicBody();
        this.EnablePatch(mock);
    }

    GetBaseUri() {
        return super.GetBaseUri('Chassis');
    }

    CreateJsonTemplate() {
        var jsonTemplate = {
            '@odata.context': '/redfish/v1/$metadata#Chassis/Rack',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Chassis.1.0.0.Chassis',
            'Id': this.Id,
            'Name': 'ChassisRack',
            'Description': this.Description || 'Rack',
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'ChassisType': this.ChassisType,
            'Oem': {
                'Intel_RackScale': {
                    '@odata.type': '#Intel.Oem.RmmRackChassis',
                    'UUID': this.Uuid,
                    'CreatedDate': '2015-10-22T06:51:06+02:00',
                    'UpdatedDate': '2015-10-22T06:51:06+02:00',
                    'ApiVersion': '1.0',
                    'RackPUID': this.RackPuid,
                    'PODDCUID': 'pod2.jf5.intel.com',
                    'PODMAddress': 'podm2.pod1.jf5.intel.com',
                    'GeoTag': 'RAM MEMORY 0x2343453434234f',
                    'RackDCUID': 'rack.jf5.intel.com-1',
                    'RackSupportsDisaggregatedPowerCooling': false,
                    'DrawerPresence': '01010000',
                    'MBPPresence': '10',
                    'PowerZones': {
                        '@odata.id': `${this.GetBaseUri()}/PowerZones`
                    },
                    'ThermalZones': {
                        '@odata.id': `${this.GetBaseUri()}/ThermalZones`
                    },
                    'MBPs': {
                        '@odata.id': `${this.GetBaseUri()}/MBPs`
                    }
                }
            },
            'Thermal': {
                '@odata.id': `${this.GetBaseUri()}/Thermal`
            },
            'Power': {
                '@odata.id': `${this.GetBaseUri()}/Power`
            },
            'Manufacturer': 'Intel',
            'Model': 'Intel',
            'SerialNumber': '1234567890',
            'PartNumber': '0987654321',
            'AssetTag': '',
            'Links': {
                'ManagedBy@odata.count': this.ManagedBy.length,
                'ManagedBy': this.ManagedBy,
                'Contains@odata.count': this.Contains.length,
                'Contains': this.Contains
            },
            'Actions': {
                'Oem': {
                    'Intel_RackScale': {
                        '#RackChassis.DrawerReset': {
                            'target': this.GetBaseUri() +'/Actions/Oem/RackChassis.DrawerReset',
                            'TargetIndex@AllowableValues': [
                                1,
                                2
                            ],
                            'ResetType@AllowableValues': [
                                'Soft',
                                'Hard'
                            ]
                        }
                    }
                }
            }
        };

        if (this.LocationPropertyVisible === true) {
            jsonTemplate['Oem']['Intel_RackScale']['Location'] = {
                'Id': this.LocationId
            };
        }

        return jsonTemplate;
    }

    EnablePatch(mock) {
        const self = this;
        mock.mockPatch(self.GetBaseUri(),
            httpStatusCode.OK,
            undefined,
            (req, response) => {
                const jsonRequest = JSON.parse(req.body);


                if (this.LocationPropertyVisible) {
                    const newRackPuid = jsonRequest.Oem.Intel_RackScale.Location.Id;

                    if (newRackPuid !== undefined) {
                        self.LocationId = newRackPuid;

                        self.RackPuid = parseInt(newRackPuid);

                        if (isNaN(self.RackPuid) || self.RackPuid < 0 || self.RackPuid > UINT_FOUR_BYTE_MAX) {
                            self.RackPuid = 0;
                        }
                    }
                }
                else {
                    const newRackPuid = jsonRequest.Oem.Intel_RackScale.RackPUID;

                    if (newRackPuid !== undefined) {
                        self.RackPuid = newRackPuid;
                    }
                }

                response.body = self.CreateJsonTemplate();
                return response;
            }
        );
    }

    get Status() {
        return this.status;
    }

    set Status(val) {
        this.status = val;
    }

    set Description(val) {
        this.description = val;
    }

    get Description() {
        return this.description;
    }

    get ChassisType() {
        return 'Rack';
    }

    CreateCollections() {
        this.powerZonesCollection = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#Chassis/Rack/PowerZones',
            '@odata.id': `${this.GetBaseUri()}/PowerZones`,
            '@odata.type': '#Intel.Oem.RmmPowerZoneCollection',
            'Name': 'Power Zone Collection',
            'Context': this,
            'Resource': PowerZone
        });

        this.thermalZonesCollection = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#Chassis/Rack/ThermalZones',
            '@odata.id': `${this.GetBaseUri()}/ThermalZones`,
            '@odata.type': '#Intel.Oem.RmmThermalZoneCollection',
            'Name': 'Thermal Zone Collection',
            'Context': this,
            'Resource': ThermalZone
        });

        this.mbps = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#Chassis/Rack/MBPs',
            '@odata.id': `${this.GetBaseUri()}/MBPs`,
            '@odata.type': '#Intel.Oem.MBPCollection',
            'Name': 'Manager Backplane Collection',
            'Context': this,
            'Resource': Mbp
        });
    }

    get LocationPropertyVisible() {
        return this.locationPropertyVisible;
    }

    set LocationPropertyVisible(visible) {
        this.locationPropertyVisible = visible;
    }

    GetThermalZoneWithId(id) {
        return this.thermalZonesCollection.GetById(id);
    }

    GetPowerZoneWithId(id) {
        return this.powerZonesCollection.GetById(id);
    }

    get PowerZones() {
        return this.powerZonesCollection;
    }

    AddPowerZone() {
        return this.powerZonesCollection.Add();
    }

    get ThermalZones() {
        return this.thermalZonesCollection;
    }

    AddThermalZone() {
        return this.thermalZonesCollection.Add();
    }

    set Thermal(thermal) {
        this.thermal = thermal;
    }

    get Thermal() {
        return this.thermal;
    }

    set Power(power) {
        this.power = power;
    }

    get Power() {
        return this.power;
    }

    get Mbps() {
        return this.mbps;
    }

    AddMbp() {
        return this.mbps.Add();
    }

    get RackPuid() {
        return this.rackPuid;
    }

    set RackPuid(rackPuid) {
        this.rackPuid = rackPuid;
    }

    get LocationId() {
        return String(this.locationId);
    }

    set LocationId(locationId) {
        this.locationId = locationId;
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

    get Contains() {
        return this.contains;
    }

    set Contains(elements) {
        this.contains.Clear();
        this.AddContains(elements);
    }

    AddContains(elements) {
        this.contains.Add(elements);
        return this;
    }
}

exports.Rack = Rack;
