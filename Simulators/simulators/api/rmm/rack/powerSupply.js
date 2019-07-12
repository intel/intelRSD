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
const uuid = require('uuid');

class PowerSupply {
    constructor(baseUri, id, uLocation=1, xlocation = 1){
        this.baseUri = baseUri;
        this.id = id;
        this.uuid = uuid.v1();
        this.ulocation = uLocation;
        this.xlocation = xlocation;
    }

    get Id() {
        return this.id;
    }

    get Uri() {
        return this.baseUri + '/PowerZone#/PowerSupplies/' + this.Id;
    }

    Json(){
        return {
            '@odata.id': this.Uri,
            '@odata.type': '#RmmPowerZone.PowerSupply',
            'Name': 'Power supply 1',
            'Status': {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'UUID': this.uuid,
            'RackLocation': {
                'RackUnits': 'OU',
                'XLocation': this.ulocation,
                'ULocation': this.xlocation,
                'UHeight': 8
            },
            'SerialNumber': '',
            'Manufacturer': '',
            'Model': 'model.56',
            'PartNumber': 'model.56',
            'FirmwareVersion': 'rev.789',
            'PowerCapacityWatts': 300,
            'LastPowerOutputWatts':48
        }
    }
}

exports.PowerSupply = PowerSupply;
