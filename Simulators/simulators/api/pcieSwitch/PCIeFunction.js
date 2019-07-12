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
const OdataIdArray = require('./../common/odataIdArray').OdataIdArray;

class PCIeFunction extends BaseAsset {
    constructor(mock, pcieDevice, id) {
        super(mock, pcieDevice, id);
        this.MockGetWithDynamicBody();
        this.drives = new OdataIdArray();
    }

    GetBaseUri() {
        return super.GetBaseUri('Functions');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#PCIeDevices/Members/D1/PCIeFunctions/Members$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#PCIeFunction.v1_0_0.PCIeFunction',
            'Id': this.Id,
            'Name': this.Name || 'NVMe Drive',
            'Description': '850GB NVMe drive',
            'FunctionId': 0,
            'DeviceClass': 'MassStorageController',
            'FunctionType': 'Physical',
            'DeviceId': '0xABCD',
            'VendorId': '0xABCD',
            'ClassCode': '0x10802',
            'RevisionId': '0x00',
            'SubsystemId': '0xABCD',
            'SubsystemVendorId': '0xABCD',
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK',
                'HealthRollup': null
            },
            'Links': {
                'Drives': this.Drives,
                'PCIeDevice': this.PCIeDevice,
                'StorageControllers': [],
                'EthernetInterfaces': []
            }
        };
    }

    get Status() {
        return this.status;
    }

    set Status(val) {
        this.status = val;
    }

    get Drives() {
        return this.drives;
    }

    set Drives(drive) {
        this.drives.Add(drive);
    }

    get PCIeDevice() {
        return this.pcieDevice;
    }

    set PCIeDevice(pcieDevice) {
        this.pcieDevice = {'@odata.id': pcieDevice.GetBaseUri()};
    }

    get Name() {
        return this.name;
    }

    set Name(name) {
        this.name = name;
    }
}

exports.PCIeFunction = PCIeFunction;