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
const Fan = require('./fan').Fan;
const Temperature = require('./temperature').Temperature;
const Redundancy = require('./../../common/redundancy').Redundancy;

class Thermal extends BaseAsset {
    constructor(mock, rack) {
        super(mock, rack);

        this.url = rack;
        this.fans = [];
        this.redundancy = [];
        this.temperatures = [];
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return this.url + '/Thermal';
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#Thermal.Thermal',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Thermal.v1_1_0.Thermal',
            'Id': 'Thermal',
            'Name': this.Name || 'Thermal',
            'Description': 'Thermal',
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'Temperatures': this.TemperaturesJsons,
            'Fans': this.FansJsons,
            'Redundancy': this.redundancy.map(r => r.Json())
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

    get TemperaturesJsons(){
        return this.Temperatures.map(temp => temp.Json());
    }

    get Temperatures(){
        return this.temperatures;
    }

    AddThermalTemperature(){
        var temp = new Temperature(this.GetBaseUri(), this.Temperatures.length + 1);
        this.Temperatures.push(temp);

        return temp;
    }

    RemoveThermalTemperature(temperature){
        const index = this.Temperatures.indexOf(temperature);
        if(index > -1) {
            this.Temperatures.splice(index);
        }
    }

    AddRedundancy() {
        var redundancyItem = new Redundancy(this.GetBaseUri(), this.redundancy.length)
        this.redundancy.push(redundancyItem)

        return redundancyItem
    }

    get FansJsons(){
        return this.Fans.map(fan => fan.Json());
    }

    get Fans(){
        return this.fans;
    }

    AddThermalFan(){
        var fan = new Fan(this.GetBaseUri(), this.Fans.length  + 1);
        this.Fans.push(fan);

        return fan;
    }

    RemoveThermalFan(fan){
        const index = this.Fans.indexOf(fan);
        if(index > -1) {
            this.Fans.splice(index);
        }
    }

    get Redundancy(){
        return this.redundancy;
    }

}

exports.Thermal = Thermal;

