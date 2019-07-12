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
const Collection = require('../common/collection.js').Collection;
const FabricPort = require('./fabricPort').FabricPort;
const OdataIdArray = require('./../common/odataIdArray').OdataIdArray;

var httpStatusCode = require('../../utils/httpStatusCode').httpStatusCode;

class FabricSwitch extends BaseAsset {
    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);
        this.CreateCollections();
        this.MockGetWithDynamicBody();
        this.chassis = new OdataIdArray();

        mock.mockPost(this.GetBaseUri() + '/Actions/Switch.Reset',
            httpStatusCode.BAD_REQUEST,
            undefined,
            (req, response) => {
                return response;
            }
        );
    }

    GetBaseUri() {
        return super.GetBaseUri('Switches');
    }

    CreateJsonTemplate() {
        var defaultJson = {
                '@odata.context': '/redfish/v1/$metadata#Switch.Switch',
                '@odata.id': this.GetBaseUri(),
                '@odata.type': '#Switch.v1_0_0.Switch',
                'Id': this.Id,
                'Name': 'PCIe Switch',
                'SwitchType': 'PCIe',
                'Description': 'PCIe switch description',
                'Status': this.Status || {
                    'State': 'Enabled',
                    'Health': 'OK',
                    'HealthRollUp': 'OK'
                },
                'Manufacturer': 'Manufacturer Name',
                'Model': 'Model Name',
                'SKU': 'SKU',
                'SerialNumber': '1234567890',
                'PartNumber': '997',
                'AssetTag': 'Customer Asset Tag',
                'DomainID': 1,
                'IsManaged': true,
                'TotalSwitchWidth': 97,
                'IndicatorLED': null,
                'PowerState': this.PowerState || 'On',
                'Ports': {
                    '@odata.id': this.GetBaseUri() + '/Ports'
                },
                'Redundancy': [],
                'Links': {
                    'Chassis': this.Chassis,
                    'ManagedBy': [],
                    'Oem': {}
                },
                'Actions': {
                    '#Switch.Reset': {
                        'target': this.GetBaseUri() + '/Actions/Switch.Reset',
                        'ResetType@Redfish.AllowableValues': []
                    },
                    'Oem': {}
                },
                'LogServices': {},
                'Oem': {}
            };
        return defaultJson;
    }

    CreateCollections() {
        this.portCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#PortCollection.PortCollection',
            '@odata.id': this.GetBaseUri() + '/Ports',
            '@odata.type': '#PortCollection.PortCollection',
            'Name': 'PCIe Port Collection',
            'Description': 'PCIe Port Collection',
            'Context': this,
            'Resource': FabricPort
        });
    }

    get Status() {
        return this.status;
    }

    set Status(val) {
        this.status = val;
    }

    AddPort() {
        return this.portCollection.Add();
    }

    get Ports() {
        return this.portCollection;
    }

    get PowerState() {
        return this.powerState;
    }

    set PowerState(powerState) {
        this.powerState = powerState;
    }

    get Chassis() {
        return this.chassis;
    }

    set Chassis(chassis) {
        this.chassis.Add(chassis);
    }
}


exports.FabricSwitch = FabricSwitch;