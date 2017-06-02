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
const Collection = require('./../common/collection').Collection;
const PCIeFunction = require('./PCIeFunction').PCIeFunction;
const OdataIdArray = require('./../common/odataIdArray').OdataIdArray;
const httpStatusCode = require('../../utils/httpStatusCode').httpStatusCode;

class PCIeDevice extends BaseAsset {
    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);
        this.MockGetWithDynamicBody();
        this.CreateCollections();

        this.managedBy = new OdataIdArray();
        this.chassis = new OdataIdArray();
        this.MockPatch();
    }

    MockPatch() {
        this.mock.mockPatch(this.GetBaseUri(),
            httpStatusCode.OK,
            undefined,
            (req, response) => {
                var jsonRequest = JSON.parse(req.body);
                this.AssetTag = jsonRequest.AssetTag || this.AssetTag;

                response.body = this.CreateJsonTemplate();
                return response;
            }
        );
    }

    GetBaseUri() {
        return super.GetBaseUri('PCIeDevices');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#Devices/Members/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#PCIeDevice.v0_9_0.PCIeDevice',
            'Id': this.Id,
            'Name': this.Name || 'PCIe Device D1',
            'AssetTag': this.AssetTag || null,
            'Manufacturer': 'Manufacturer Name',
            'Model': 'Model Name',
            'SKU': 'SKU number',
            'SerialNumber': '2M220100SL',
            'PartNumber': null,
            'Description': '850GB NVMe drive',
            'DeviceType': 'SingleFunction',
            'FirmwareRevision': '12.342-343',
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK',
                'HealthRollup': 'OK'
            },
            'Links': {
                'Chassis': this.Chassis,
                'PCIeFunctions': this.deviceFunctions.GetODataIdsCollection(),
                'Oem': {}
            },
            'Oem': {}
        };
    }

    CreateCollections() {
        this.deviceFunctions = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#PCIeFunctions/$entity',
            '@odata.id': this.GetBaseUri() + '/Functions',
            '@odata.type': '#PCIeFunctionCollection.PCIeFunctionCollection',
            'Name': 'PCIe Function Collection',
            'Description': 'PCIe Function Collection',
            'Context': this,
            'Resource': PCIeFunction
        });
    }

    get Status() {
        return this.status;
    }

    set Status(val) {
        this.status = val;
    }

    get AssetTag() {
        return this.assetTag;
    }

    set AssetTag(assetTag) {
        this.assetTag = assetTag;
    }

    get Chassis() {
        return this.chassis;
    }

    AddChassis(chassis) {
        this.chassis.Add(chassis);
    }

    AddDeviceFunction() {
        return this.deviceFunctions.Add();
    }

    get DeviceFunctions() {
        return this.deviceFunctions;
    }

    get Name() {
        return this.name;
    }

    set Name(name) {
        this.name = name;
    }
}

exports.PCIeDevice = PCIeDevice;