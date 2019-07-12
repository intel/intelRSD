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

class Vlan extends BaseAsset {
    constructor(mock, ethernetInterface, id) {
        super(mock, ethernetInterface, id);
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return super.GetBaseUri('VLANs');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#Managers/RMC/EthernetInterfaces/1/VLANs/1',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#VLanNetworkInterface.1.0.0.VLanNetworkInterface',
            'Id': this.Id,
            'Name': 'VLAN Network Interface',
            'Description': this.Description || 'Switch Port NIC 1 VLAN',
            'VLANEnable': this.VlanEnable != null ? this.VlanEnable : true,
            'VLANId': this.VlanId || 0,
            'Oem': {
                'Intel_RackScale': {
                    '@odata.type': '#Intel.Oem.VLanStatus',
                    'Tagged': this.Tagged != null ? this.Tagged : false,
                    'Status': {
                        'State': 'Enabled',
                        'Health': 'OK'
                    }
                }
            }
        };
    }
}

exports.Vlan = Vlan;