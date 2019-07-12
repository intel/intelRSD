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

const OdataIdArray = require('../common/odataIdArray').OdataIdArray;
const BaseAsset = require('../common/baseAsset').BaseAsset;

class Storage extends BaseAsset {
    constructor(mock, system, id) {
        super(mock, system, id);
        this.MockGetWithDynamicBody();
        this.drives = new OdataIdArray();
        this.enclosures = new OdataIdArray();
        this.storageControllers = [];
    }

    GetBaseUri() {
        return super.GetBaseUri('Storage');
    }

    CreateJsonTemplate() {
        return {
            "@odata.context": "/redfish/v1/$metadata#Systems/Members/1/Storage/Members/$entity",
            "@odata.id": this.GetBaseUri(),
            "@odata.type": "#Storage.v1_0_0.Storage",
            "Id": this.Id,
            "Name": this.Name || null,
            "Description": this.Description || null,
            "Status": this.Status || {
                "State": "Enabled",
                "Health": "OK",
                "HealthRollup": "OK"
            },
            "StorageControllers": this.StorageControllers || [],
            "Drives": this.Drives || [],
            "Volumes": [],
            "Links": {
                "Enclosures": this.Enclosures || []
            },
            "Actions": {}
        };
    }

    set Status(val) {
        this.status = val;
    }

    get Status() {
        return this.status;
    }

    AddEnclosure(enclosure) {
        this.enclosures.AddWithODataId(enclosure.GetBaseUri())
    }

    get Enclosures() {
        return this.enclosures;
    }

    set Enclosures(val) {
        this.enclosures = val
    }

    get Name() {
        return this.name;
    }

    set Name(val) {
        this.name = val
    }

    get Description() {
        return this.description;
    }

    set Description(val) {
        this.description = val
    }

    AddDrive(drive) {
        this.drives.AddWithODataId(drive.GetBaseUri())
    }

    get Drives() {
        return this.drives
    }

    set Drives(val) {
        this.drives = val
    }

    AddStorageController() {
        this.storageControllers = this.storageControllers || [];
        let id = this.storageControllers.length + 1;

        var storageControllerJsonTemplate = {
            '@odata.id': '/redfish/v1/Systems/1/Storage/SATA#/StorageControllers/' + id,
            '@odata.type': '#Storage.v1_0_0.StorageController',
            'MemberId': id,
            'Name': 'System SATA',
            'Description': 'System SATA (Embedded)',
            'Status': {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'Manufacturer': 'ManufacturerName',
            'Model': 'ProductModelName',
            'SKU': '666',
            'SerialNumber': '2M220100SL',
            'PartNumber': '',
            'AssetTag': 'CustomerWritableThing',
            'SpeedGbps': 6,
            'FirmwareVersion': '81769',
            'SupportedControllerProtocols': [
                'PCIe'
            ],
            'SupportedDeviceProtocols': [
                'SATA'
            ],
            'Identifiers': [{
                'DurableName': '123e4567-e89b-12d3-a456-426655440000',
                'DurableNameFormat': 'UUID'
            }]
        };

        this.storageControllers.push(storageControllerJsonTemplate);
        return this;
    }

    get StorageControllers() {
        return this.storageControllers;
    }

    set StorageControllers(val) {
        this.storageControllers = val;
    }
}

exports.Storage = Storage;
