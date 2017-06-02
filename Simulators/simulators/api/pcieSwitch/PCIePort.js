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

class PCIePort extends BaseAsset {
    constructor(mock, pcieSwitch, id) {
        super(mock, pcieSwitch, id);
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return super.GetBaseUri('PCIePorts');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#PCIeSwitches/Members/1/PCIePorts/Members$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#PCIePort.v1_0_0.PCIePort',
            'Id': this.Id,
            'Name': 'PCIe Port',
            'Description': 'Upstream Port 1',
            'Status': {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'PortId': '1',
            'PortType': this.PortType || null,
            'SpeedGBps': 8,
            'Width': 4,
            'MaxSpeedGBps': 16,
            'MaxWidth': 4,

            'Oem': {
                'Intel_RackScale': {
                    '@odata.type': '#Intel.Oem.PCIePort',
                    'CableId': this.CableId || [],
                    'NeighborPort': []
                }
            },

            'Actions': {
                '#PCIePort.ForcedReset': {
                    'target': this.GetBaseUri() + '/Actions/PCIePort.ForcedReset',
                    'ResetType@Redfish.AllowableValues': [] //TODO: PNC Switch Port reset action is not supported yet
                },
                'Oem': {}
            }
        };
    }

    get CableId() {
        return this.cableId;
    }

    set CableId(cableId) {
        this.cableId = cableId;
    }

    get PortType() {
        return this.portType;
    }

    set PortType(portType) {
        this.portType = portType;
    }
}

exports.PCIePort = PCIePort;