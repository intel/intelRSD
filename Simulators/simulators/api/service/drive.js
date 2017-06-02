/*
 * Copyright (c) 2015-2017 Intel Corporation
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

const BaseAsset = require('../common/baseAsset').BaseAsset;

class Drive extends BaseAsset {
    constructor(mock, service, id) {
        super(mock, service, id);
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return super.GetBaseUri('Drives')
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': `/redfish/v1/$metadata#Services/Members/${this.context.Id}/Drives/Members/$entity`,
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#PhysicalDrive.v1_0_0.PhysicalDrive',
            'Id': this.Id,
            'Name': 'Simple Drive',
            'Interface': 'SATA',
            'CapacityGiB': 931,
            'Type': 'HDD',
            'RPM': 7200,
            'Manufacturer': 'Intel',
            'Model': 'ST1000NM0033-9ZM',
            'SerialNumber': 'Z1W23Q3V',
            'FirmwareVersion': '1.01',
            'Status': {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'Oem': {},
            'Links': {
                'UsedBy': this.usedBy || [],
                'ManagedBy': [],
                'Oem': {}
            }
        }
    }

    AddUsedBy(element) {
        this.usedBy = this.usedBy || [];
        this.usedBy.push({'@odata.id': element.GetBaseUri()});
        return this;
    }
}

exports.Drive = Drive;
