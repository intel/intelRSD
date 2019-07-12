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

const BaseAsset = require('../../common/baseAsset').BaseAsset;
const OdataIdArray = require('../../common/odataIdArray').OdataIdArray;

class Mbpc extends BaseAsset {
    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);

        this.managerForMBP = new OdataIdArray();

        this.MockGetWithDynamicBody();
    };

    GetBaseUri() {
        return super.GetBaseUri('Managers');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': `/redfish/v1/$metadata#Managers/MBPC${this.Id}`,
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Manager.v1_2_0.Manager',
            'Id': this.Id,
            'Name': 'MBPC1',
            'Description': 'Management Backplane Controller',
            'ManagerType': 'ManagementController',
            'UUID': '23384634-2137-3323-1720-147392915243',
            'FirmwareVersion': '1.05',
            'Status': {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'Actions': {
                '#MBPC.Reset': {
                    'target': this.GetBaseUri() + '/Actions/MBPC.Reset'
                }
            },
            'Links': {
                'ManagerForMBP@odata.count': this.ManagerForMBP.length,
                'Oem': {
                    'Intel_RackScale': {
                        'ManagerForMBP': this.ManagerForMBP
                    }
                }
            }
        }
    }

    get ManagerForMBP() {
        return this.managerForMBP;
    }

    AddMbpToManager(mbp) {
        this.managerForMBP.Add(mbp);
        mbp.AddToManagedBy(this);
        return this;
    }
}

exports.Mbpc = Mbpc;
