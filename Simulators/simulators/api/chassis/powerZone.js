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

class PowerZone extends BaseAsset {
    constructor(mock, rack, id) {
        super(mock, rack, id);
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return super.GetBaseUri('PowerZones');
    }

    CreateJsonTemplate() {
        var defaultJson = {
            '@odata.context': '/redfish/v1/$metadata#Chassis/Rack/PowerZones/Members/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#PowerZone.v1_0_0.PowerZone',
            'Id': this.Id,
            'Name': 'Power Zone',
            'Description': 'Power Zone',
            'Status': {
                'State': 'Enabled',
                'Health': 'OK',
                'HealthRollup': 'OK'
            },
            'RackLocation': {
                'RackUnits': 'OU',
                'XLocation': 0,
                'ULocation': 1,
                'UHeight': 8
            },
            'MaxPSUsSupported': 6,
            'Presence': '111111',
            'NumberOfPSUsPresent': 6,
            'PowerConsumedWatts': 2000,
            'PowerOutputWatts': 2000,
            'PowerCapacityWatts': 3000,
            'PowerSupplies': this.PowerSuppliesCollection,
            'Actions': {},
            'Links': {}
        };

        return defaultJson;
    }

    get PowerSuppliesCollection() {
        return this.powerSuppliesCollection || [];
    }

    set PowerSuppliesCollection(collection) {
        this.powerSuppliesCollection = collection;
    }

    AddPowerSupply() {
        this.powerSuppliesCollection = this.powerSuppliesCollection || [];
        let id = this.powerSuppliesCollection.length + 1;

        var powerSupply = {
            'Name': `Power supply ${id}`,
            'Status': {
                'State': 'Enabled',
                'Health': 'OK',
                'HealthRollup': 'OK'
            },
            'RackLocation': {
                'RackUnits': 'OU',
                'XLocation': 0,
                'ULocation': 1,
                'UHeight': 8
            },
            'SerialNumber': '',
            'Manufacturer': '',
            'Model': '',
            'PartNumber': '',
            'FirmwareVersion': '',
            'PowerCapacityWatts': 300,
            'LastPowerOutputWatts': 48
        };

        this.powerSuppliesCollection.push(powerSupply);

        return this;
    }
}

exports.PowerZone = PowerZone;