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

class SimpleStorage extends BaseAsset {
    constructor(mock, system, id) {
        super(mock, system, id);
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return super.GetBaseUri('SimpleStorage');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#SimpleStorage.SimpleStorage',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#SimpleStorage.v1_1_0.SimpleStorage',
            'Id': this.Id,
            'Name': this.Name || 'Simple Storage Controller',
            'Description': 'System SATA',
            'UEFIDevicePath': 'UEFI Device Path',
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK',
                'HealthRollup': 'OK'
            },
            'Devices': [
                {
                    'CapacityBytes': 1073741824,
                    'Name': 'Drive 1',
                    'Manufacturer': 'ACME',
                    'Model': 'Drive Model string',
                    'Status': {
                        'State': 'Enabled',
                        'Health': 'OK'
                    }
                },
                {
                    'CapacityBytes': 1073741823,
                    'Name': 'Drive 2',
                    'Manufacturer': 'SuperDuperSSD',
                    'Model': 'Drive Model string',
                    'Status': {
                        'State': 'Enabled',
                        'Health': 'OK'
                    }
                }
            ]
        };
    }

    set Status(val) {
        this.status = val;
    }

    get Status() {
        return this.status;
    }

    set Name(name) {
        this.name = name;
    }

    get Name() {
        return this.name;
    }

}

exports.SimpleStorage = SimpleStorage;
