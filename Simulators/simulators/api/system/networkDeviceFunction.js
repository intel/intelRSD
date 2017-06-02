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
const httpStatusCode = require('../../utils/httpStatusCode').httpStatusCode;

class NetworkDeviceFunction extends BaseAsset {
    constructor(mock, system, id) {
        super(mock, system, id);

        this.Ethernet = {
            MACAddress: 'E9:47:D3:60:64:66'
        };

        this.deviceEnabled = true;
        this.iSCSIBoot = {
            IPAddressType: 'IPv4',
            InitiatorIPAddress: '159.112.110.1',
            InitiatorName: 'Initiator Name',
            InitiatorDefaultGateway: 'Default gateway',
            InitiatorNetmask: '255.255.255.1',
            TargetInfoViaDHCP: false,
            PrimaryTargetName: 'Primary target name',
            PrimaryTargetIPAddress: '159.112.111.1',
            PrimaryTargetTCPPort: 66,
            PrimaryLUN: 10,
            PrimaryVLANEnable: true,
            PrimaryVLANId: 11,
            PrimaryDNS: '7.7.7.7',
            SecondaryTargetName: 'Secondary target name',
            SecondaryTargetIPAddress: '159.112.112.1',
            SecondaryTargetTCPPort: 77,
            SecondaryLUN: 12,
            SecondaryVLANEnable: false,
            SecondaryVLANId: 13,
            SecondaryDNS: '6.6.6.6',
            IPMaskDNSViaDHCP: false,
            RouterAdvertisementEnabled: true,
            AuthenticationMethod: 'CHAP',
            CHAPUsername: 'NDF CHAP username',
            CHAPSecret: 'nooneknows',
            MutualCHAPUsername: 'NDF Mutual CHAP username',
            MutualCHAPSecret: 'mutualnooneknows',
        }
        this.mock = mock;
        this.system = system;
        this.id = id;
        this.MockGetWithDynamicBody();
        this.MockPatch();
    }

    GetBaseUri() {
        return super.GetBaseUri('NetworkDeviceFunctions');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#Systems/Members/1/NetworkInterfaces/Members/1/NetworkDeviceFunctions/Members/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#NetworkDeviceFunction.v1_0_0.NetworkDeviceFunction',
            'Name': 'NetworkDeviceFunction',
            'Id': this.Id,
            'Description' : 'description',
            'Status': {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'DeviceEnabled': this.deviceEnabled,
            'Ethernet': this.Ethernet,
            'iSCSIBoot': this.iSCSIBoot,
            'Links': {}
        };
    }

    MockPatch() {
        this.mock.mockPatch(this.GetBaseUri(),
            httpStatusCode.OK,
            undefined,
            (req, response) => {
                var jsonRequest = JSON.parse(req.body);

                this.deviceEnabled = jsonRequest.DeviceEnabled === undefined ? this.deviceEnabled : jsonRequest.DeviceEnabled;
                if (jsonRequest.iSCSIBoot !== null) {
                    for (let property in jsonRequest.iSCSIBoot) {
                        this.iSCSIBoot[property] = jsonRequest.iSCSIBoot[property];
                    }
                }
                else {
                    for (let property in this.iSCSIBoot) {
                        this.iSCSIBoot[property] = null;
                    }
                }

                if (jsonRequest.Ethernet !== null) {
                    for (let property in jsonRequest.Ethernet) {
                        this.Ethernet[property] = jsonRequest.Ethernet[property];
                    }
                }
                else {
                    this.Ethernet.MACAddress = null;
                }

                response.body = this.CreateJsonTemplate();
                return response;
            }
        );
    }
}

exports.NetworkDeviceFunction = NetworkDeviceFunction;
