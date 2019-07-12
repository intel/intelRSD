/*
 * Copyright (c) 2016-2017 Intel Corporation
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
const uuid = require('uuid');
const PowerSupply = require('./powerSupply').PowerSupply;

class PowerZone extends BaseAsset {
    constructor(mock, rack, id) {
        super(mock, rack, id);
        this.powerSupplies = [];

        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return super.GetBaseUri('PowerZones');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context':  '/redfish/v1/$metadata#Chassis/Rack/PowerZones/Members/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Intel.Oem.RmmPowerZone',
            'Id': String(this.Id),
            'Name': 'power zone 1',
            'Description': this.Description || 'power zone 1',
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
            'MaxPSUsSupported': 6,
            'Presence': '111111',
            'NumberOfPSUsPresent': 6,
            'PowerConsumedWatts': 2000,
            'PowerOutputWatts': 2000,
            'PowerCapacityWatts': 3000,
            'PowerSupplies': this.PowerSuppliesJsons,
            'Actions': {
                '#PowerZone.RequestStateChange': {
                        'target': this.GetBaseUri() + '/Actions/PowerZone.RequestStateChange',
                        'TargetIndex@AllowableValues': [
                            1,2,3,4,5,6
                        ],
                        'EnabledState@AllowableValues': [
                            'Enabled',
                            'Disabled'
                        ]
                    }
                }
            }
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

    get PowerSuppliesJsons(){
        return this.PowerSupplies.map(psu => psu.Json());
    }

    get PowerSupplies(){
        return this.powerSupplies;
    }

    AddPsu(){
        var powerSupply = new PowerSupply(this.GetBaseUri(), this.PowerSupplies.length  + 1);
        this.PowerSupplies.push(powerSupply);

        return powerSupply;
    }

    RemovePsu(powerSupplyId=1){
        for (var i = 0; i < this.PowerSupplies.length; i++) {
            if (this.PowerSupplies[i].Id == powerSupplyId) {
                var uri = this.PowerSupplies[i].Uri;
                this.PowerSupplies.splice(i, 1);
                return uri;
            }
        }
    }
}

exports.PowerZone = PowerZone;
