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
const Fan = require('./fan').Fan;

class ThermalZone extends BaseAsset {
    constructor(mock, rack, id) {
        super(mock, rack, id);
        this.fans = [];
        
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return super.GetBaseUri('ThermalZones');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#Chassis/Rack/ThermalZones/Members/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Intel.Oem.RmmThermalZone',
            'Id': String(this.Id),
            'Name': 'thermal zone 1',
            'Description': this.Description || 'thermal zone 1',
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'UUID': this.Uuid,
            'CreatedDate': '2014-09-11T01:15:16+08:00',
            'UpdatedDate': '2014-09-11T02:15:11+08:00',
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
                    '@odata.id': this.GetBaseUri() + '/ThermalZone#/Temperatures/0',
                    'Name': 'Inlet Temperature',
                    'Status': {
                        'State': 'Enabled',
                        'Health': 'OK'
                    },
                    'ReadingCelsius': 21,
                    'PhysicalContext': 'Intake',
                    'RelatedItem': [
                        {
                            '@odata.id': this.GetBaseUri()
                        }
                    ]
                },
                {
                    '@odata.id': this.GetBaseUri() + '/ThermalZone#/Temperatures/1',
                    'Name': 'Outlet Temperature',
                    'Status': {
                        'State': 'Disabled',
                        'Health': 'OK'
                    },
                    'ReadingCelsius': 0,
                    'PhysicalContext': 'Exhaust',
                    'RelatedItem': [
                        {
                            '@odata.id': this.GetBaseUri()
                        }
                    ]
                }
            ],
            'Fans': this.FansJsons,
            'Actions': {
                'Oem': {
                    '#ThermalZone.SetDesiredSpeedPWM': {
                        'target': this.GetBaseUri() + '/Actions/ThermalZone.SetDesiredSpeedPWM',
                        'DesiredSpeedPWM@AllowableValues': []
                    }
                }
            }
        };
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

    get FansJsons(){
        return this.Fans.map(fan => fan.Json());
    }

    get Fans(){
        return this.fans;
    }

    AddFan(){
        var fan = new Fan(this.GetBaseUri(), this.Fans.length  + 1);
        this.Fans.push(fan);

        return fan;
    }

    RemoveFan(fanId=1){
        for (var i = 0; i < this.Fans.length; i++) {
            if (this.Fans[i].Id == fanId) {
                var uri = this.Fans[i].Uri;
                this.Fans.splice(i, 1);
                return uri;
            }
        }
    }
}

exports.ThermalZone = ThermalZone;
