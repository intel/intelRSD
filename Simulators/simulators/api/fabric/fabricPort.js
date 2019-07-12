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
const httpStatusCode = require('../../utils/httpStatusCode').httpStatusCode;

class FabricPort extends BaseAsset {
    constructor(mock, pcieSwitch, id) {
        super(mock, pcieSwitch, id);
        this.pcieConnectionIds = [];
        this.associatedEndpoints = new OdataIdArray();
        this.MockGetWithDynamicBody();

        mock.mockPost(this.GetBaseUri() + '/Actions/Port.Reset',
            httpStatusCode.BAD_REQUEST,
            undefined,
            (req, response) => {
                return response;
            }
        );
    }

    GetBaseUri() {
        return super.GetBaseUri('Ports');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#Port.Port',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Port.v1_0_0.Port',
            'Id': this.Id,
            'Name': this.Name || 'PCIe Upstream Port 1',
            'Description': 'PCIe Port ' + this.Id,
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'PortId': '1',
            'PortProtocol': 'PCIe',
            'PortType': this.PortType || 'UpstreamPort',
            'CurrentSpeedGbps': 32,
            'Width': 4,
            'MaxSpeedGbps': 64,
            'Actions': {
                '#Port.Reset': {
                    'target': this.GetBaseUri() + '/Actions/Port.Reset',
                    'ResetType@Redfish.AllowableValues': []
                },
                'Oem': {}
            },
            'Links': {
                'AssociatedEndpoints': this.AssociatedEndpoints || [],
                'ConnectedSwitches': [],
                'ConnectedSwitchPorts': []
            },
            'Oem': {
                'Intel_RackScale': {
                    '@odata.type': '#Intel.Oem.Port',
                    'PCIeConnectionId': this.pcieConnectionIds || []
                }
            }
        };
    }

    get Status() {
        return this.status;
    }

    set Status(val) {
        this.status = val;
    }

    get PcieConnectionIds() {
        return this.pcieConnectionIds;
    }

    set PcieConnectionIds(pcieConnectionIds) {
        this.pcieConnectionIds = pcieConnectionIds;
    }

    AddPcieConnectionId(cableId) {
        this.pcieConnectionIds.push(cableId);
    }

    get Name() {
        return this.name;
    }

    set Name(name) {
        this.name = name;
    }

    get PortType() {
        return this.portType;
    }

    set PortType(portType) {
        this.portType = portType;
    }

    get AssociatedEndpoints() {
        return this.associatedEndpoints
    }

    set AssociatedEndpoints(elements) {
        this.associatedEndpoints.Add(elements);
    }
}

exports.FabricPort = FabricPort;