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
const OdataIdArray = require('../common/odataIdArray').OdataIdArray;

class FabricEndpoint extends BaseAsset {
    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);
        this.MockGetWithDynamicBody();
        this.ports = new OdataIdArray();
        this.connectedEntities = [];
    }

    GetBaseUri() {
        return super.GetBaseUri('Endpoints');
    }

    CreateJsonTemplate() {
        var defaultJson = {
            '@odata.context': '/redfish/v1/$metadata#Endpoint.Endpoint',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Endpoint.v1_0_0.Endpoint',
            'Id': this.Id,
            'Name': this.Name || 'Upstream Endpoint',
            'Description': 'The PCIe Physical function of an 850GB NVMe drive',
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK',
                'HealthRollUp': 'OK'
            },
            'EndpointProtocol': 'PCIe',
            'PciId': {
                'DeviceId': '0xABCD',
                'VendorId': '0xABCD',
                'SubsystemId': '0xABCD',
                'SubsystemVendorId': '0xABCD'
            },
            'Identifiers': [
                {
                    'DurableNameFormat': 'UUID',
                    'DurableName': '00000000-0000-0000-0000-000000000000'
                }
            ],
            'ConnectedEntities': this.connectedEntities,
            'Redundancy': [],
            'HostReservationMemoryBytes': null,
            'Links': {
                'Ports': this.Ports
            },
            'Oem': {},
            'Actions': {
                'Oem': {}
            }
        };
        return defaultJson;
    }

    get Status() {
        return this.status;
    }

    set Status(val) {
        this.status = val;
    }

    get Ports() {
        return this.ports;
    }

    set Ports(elements) {
        this.AddToPorts(elements);
    }

    get Name() {
        return this.name;
    }

    set Name(name) {
        this.name = name;
    }

    AddToPorts(elements) {
        this.ports.Add(elements);
        return this;
    }

    get ConnectedEntities(){
        return this.connectedEntities;
    }

    AddConnectedEntity(connectedEntity) {
        this.ConnectedEntities.push(connectedEntity);
    }
}

exports.FabricEndpoint = FabricEndpoint;
