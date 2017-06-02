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
const OdataIdArray = require('../../common/odataIdArray').OdataIdArray;

class Mbp extends BaseAsset {

    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);

        this.managedBy = new OdataIdArray();

        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return super.GetBaseUri('MBPs');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#Chassis/Rack/MBPs/Members/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Intel.Oem.MBP',
            'Id': String(this.Id),
            'UUID': '25892e17-80f6-415f-9c65-7395632f0223',
            'Name': 'Management Backplane',
            'Description': 'Rack Management Plane 1',
            'CreatedDate': '2014-09-10T01:09:26+08:00',
            'UpdatedDate': '2014-09-11T11:11:16+08:00',
            'MBPHWAddress': 'usb:2-1',
            'MBPIPAddress': '192.168.1.120',
            'FirmwareState': 'Up',
            'RackLocation': {
                'RackUnits': 'OU',
                'Xlocation': 0,
                'Ulocation': 8,
                'Uheight': 8
            },
            'AssetTag': '11223344',
            'SerialNumber': '',
            'Manufacturer': '',
            'Model': '',
            'PartNumber': '',
            'Actions': {
                '#MBP.SetUartTarget': {
                    'target': this.GetBaseUri() + '/Actions/MBP.SetUartTarget',
                    'TargetTray@AllowableValues': [1, 2, 3, 4],
                    'TargetComponent@AllowableValues': ['MMP', 'SLED1', 'SLED2', 'SLED3', 'SLED4', 'CPP']
                },
                '#MBP.Update': {
                    'target': this.GetBaseUri() + '/Actions/MBP.Update',
                    'Image@AllowableValues': []
                }
            },
            'Links': {
                'ManagedBy@odata.count': this.ManagedBy.length,
                'ManagedBy': this.ManagedBy
            }
        }
    }

    get ManagedBy() {
        return this.managedBy;
    }

    AddToManagedBy(element) {
        this.managedBy.Add(element);

        return this;
    }
}

exports.Mbp = Mbp;