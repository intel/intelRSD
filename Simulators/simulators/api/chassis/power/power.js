/*
 * Copyright (c) 2016-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

const BaseAsset = require('../../common/baseAsset').BaseAsset;
const Redundancy = require('./../../common/redundancy').Redundancy;
const PowerControl = require('./powerControl').PowerControl;
const Voltage = require('./voltage').Voltage;
const PowerSupply = require('./powerSupply').PowerSupply;


class Power extends BaseAsset {
    constructor(mock, rack) {
        super(mock, rack);

        this.url = rack;
        this.powerControls = [];
        this.voltages = [];
        this.powerSupplies = [];
        this.redundancy = [];
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return this.url + '/Power';
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#Power.Power',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Power.v1_1_0.Power',
            'Id': 'Power',
            'Name': this.Name || 'RACK_POWER',
            'Description': 'PowerDescription',
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'PowerControl' : this.PowerControlJsons,
            'Voltages' : this.VoltagesJsons,
            'PowerSupplies' : this.PowerSuppliesJsons,
            'Redundancy' : this.redundancy.map(r => r.Json()),
            'Oem' : {}
        };
    }

    set Status(val) {
        this.status = val;
    }

    get Status() {
        return this.status;
    }

    get Name() {
        return this.name;
    }

    set Name(name) {
        this.name = name;
    }

    get PowerControls() {
        return this.powerControls;
    }

    get Voltages() {
        return this.voltages;
    }

    get PowerSupplies() {
        return this.powerSupplies;
    }

    get Redundancy() {
        return this.redundancy;
    }

    get PowerControlJsons() {
        return this.powerControls.map(pc => pc.Json())
    }

    get VoltagesJsons() {
        return this.voltages.map(v => v.Json())
    }

    get PowerSuppliesJsons() {
        return this.powerSupplies.map(psu => psu.Json());
    }

    AddRedundancy() {
        var redundancyItem = new Redundancy(this.GetBaseUri(), this.redundancy.length)
        this.redundancy.push(redundancyItem)
        return redundancyItem
    }

    AddPowerControl(){
        var powerControl = new PowerControl(this.GetBaseUri(), this.PowerControls.length);
        this.powerControls.push(powerControl);
        return powerControl;
    }

    AddVoltage(){
        var voltage = new Voltage(this.GetBaseUri(), this.Voltages.length);
        this.voltages.push(voltage);
        return voltage;
    }

    AddPowerSupply(){
        var powerSupply = new PowerSupply(this.GetBaseUri(), this.powerSupplies.length);
        this.powerSupplies.push(powerSupply);
        return powerSupply;
    }

}

exports.Power = Power;

