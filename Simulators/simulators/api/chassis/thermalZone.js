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

class ThermalZone extends BaseAsset {
    constructor(mock, chassis, id) {
        super(mock, chassis, id);
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return super.GetBaseUri('ThermalZones');
    }

    CreateJsonTemplate() {
        var defaultJson = {
            '@odata.context': '/redfish/v1/$metadata#Chassis/Rack/ThermalZones/Members/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#ThermalZone.v1_0_0.ThermalZone',
            'Id': this.Id,
            'Name': 'Thermal Zone',
            'Description': 'Thermal Zone',
            'RackLocation': {
                'RackUnits': 'OU',
                'XLocation': 0,
                'ULocation': 1,
                'UHeight': 8
            },
            'Presence': '111100',
            'DesiredSpeedPWM': 50,
            'DesiredSpeedRPM': 3000,
            'MaxFansSupported': 6,
            'NumberOfFansPresent': 6,
            'VolumetricAirflow': 80,
            'Temperatures': [
                {
                    'Name': 'Inlet Temperature',
                    'Status': {
                        'State': 'Enabled',
                        'Health': 'OK',
                        'HealthRollup': null
                    },
                    'ReadingCelsius': 21,
                    'PhysicalContext': 'Intake'
                },
                {
                    'Name': 'Outlet Temperature',
                    'Status': {
                        'State': 'Enabled',
                        'Health': 'OK',
                        'HealthRollup': null
                    },
                    'ReadingCelsius': 35,
                    'PhysicalContext': 'Exhaust'
                }
            ],
            'Status': {
                'State': 'Enabled',
                'Health': 'OK',
                'HealthRollup': null
            },
            'Fans': this.FansCollection,
            'Actions': {},
            'Links': {}
        };

        return defaultJson;
    }

    get FansCollection() {
        return this.fansCollection || [];
    }

    set FansCollection(collection) {
        this.fansCollection = collection;
    }

    AddFan() {
        this.fansCollection = this.fansCollection || [];
        let id = this.fansCollection.length + 1;

        var fan = {
            'Name': `Fan ${id}`,
            'Status': {
                'State': 'Enabled',
                'Health': 'OK',
                'HealthRollup': null
            },
            'ReadingRPM': 0,
            'RackLocation': {
                'RackUnits': 'OU',
                'XLocation': 0,
                'ULocation': 1,
                'UHeight': 8
            }
        };

        this.fansCollection.push(fan);

        return this;
    }
}

exports.ThermalZone = ThermalZone;