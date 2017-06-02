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

class Node extends BaseAsset {
    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);
        this.MockGetWithDynamicBody();
        this.MockBehaviours();
    }

    GetBaseUri() {
        return super.GetBaseUri('Nodes');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#Nodes/Members/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#ComposedNode.v1_1_0.ComposedNode',
            'Id': this.Id,
            'Name': 'Composed Node',
            'Description': 'Node #1',
            'SystemType': 'Physical',
            'AssetTag': 'free form asset tag',
            'Manufacturer': 'Manufacturer Name - the same as Computer System',
            'Model': 'Model Name - the same as Computer System',
            'SKU': 'SKU - the same as Computer System',
            'SerialNumber': '2M220100SL  - the same as Computer System',
            'PartNumber': 'Computer1  - the same as Computer System',
            'UUID': this.Uuid,
            'HostName': null,
            'PowerState': this.PowerState || 'On',
            'BiosVersion': 'P79 v1.00 (09/20/2013)',
            'Status': {
                'State': 'Enabled',
                'Health': 'OK',
                'HealthRollup': 'OK'
            },
            'Processors': {
                'Count': 2,
                'Model': 'Multi-Core Intel(R) Xeon(R) processor 7xxx Series',
                'Status': {
                    'State': 'Enabled',
                    'Health': 'OK'
                }
            },
            'Memory': {
                'TotalSystemMemoryGiB': 32,
                'Status': {
                    'State': 'Enabled',
                    'Health': 'OK'
                }
            },
            'ComposedNodeState': this.ComposedNodeState || 'Allocated',
            'Boot': {
                'BootSourceOverrideEnabled': 'Disabled',
                'BootSourceOverrideTarget': 'None',
                'BootSourceOverrideTarget@Redfish.AllowableValues': [
                    'None',
                    'Pxe',
                    'Hdd'
                ],
                'BootSourceOverrideMode': 'Legacy',
                'BootSourceOverrideMode@Redfish.AllowableValues': [
                    'Legacy',
                    'UEFI'
                ]
            },
            'Oem': {},
            'Links': {
                'ComputerSystem': {},
                'Processors': [],
                'Memory': [],
                'EthernetInterfaces': [],
                'LocalDrives': [],
                'RemoteDrives': [],
                'ManagedBy': [],
                'Oem': {}
            },
            'Actions': {
                '#ComposedNode.Reset': {
                    'target': '/redfish/v1/Nodes/1/Actions/ComposedNode.Reset',
                    'ResetType@Redfish.AllowableValues': [
                        'On',
                        'ForceOff',
                        'GracefulRestart',
                        'ForceRestart',
                        'Nmi',
                        'ForceOn',
                        'PushPowerButton',
                        'SoftShutdown'
                    ]
                },
                '#ComposedNode.Assemble': {
                    'target': '/redfish/v1/Nodes/1/Actions/ComposedNode.Assemble'
                }
            }
        };
    }

    MockBehaviours(){
        let self = this;

        this.mock.mockPost(self.GetBaseUri() + '/Actions/ComposedNode.Assemble',
            204,
            undefined,
            function (req, response) {
                self.ComposedNodeState = 'Assembled';
                return response;
            }
        );

        this.mock.mockPost(self.GetBaseUri() + '/Actions/ComposedNode.Reset',
            204,
            undefined,
            function (req, response) {
                self.PowerState = 'On';
                return response;
            }
        );
    }

    get ComposedNodeState() {
        return this.composedNodeState;
    }

    set ComposedNodeState(composedNodeState) {
        this.composedNodeState = composedNodeState;
    }

    get PowerState() {
        return this.powerState;
    }

    set PowerState(powerState) {
        this.powerState = powerState;
    }
}

exports.Node = Node;
