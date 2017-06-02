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
const Collection = require('../common/collection').Collection;
const Vlan = require('../ethernetSwitch/vlan').Vlan;

class EthernetInterface extends BaseAsset {

    constructor(mock, system, id, properties) {
        super(mock, system, id, properties);
        this.CreateCollections();
        this.MacAddress = this.properties['MACAddress'] || 'E9:47:D3:60:64:66';
        this.PermanentMacAddress = this.MacAddress;
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return super.GetBaseUri('EthernetInterfaces');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#EthernetInterface.EthernetInterface',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#EthernetInterface.v1_0_0.EthernetInterface',
            'Id': this.Id,
            'Name': 'Ethernet Interface',
            'Description': 'System NIC 1',
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'InterfaceEnabled': true,
            'PermanentMACAddress': this.PermanentMacAddress,
            'MACAddress': this.MacAddress,
            'SpeedMbps': this.SpeedMbps || null,
            'AutoNeg': true,
            'FullDuplex': this.FullDuplex || null,
            'MTUSize': 1500,
            'HostName': 'web483',
            'FQDN': 'web483.redfishspecification.org',
            'VLAN': {
                'VLANEnable': true,
                'VLANId': 101
            },
            'VLANs': {
                '@odata.id': this.GetBaseUri() + '/VLANs'
            },
            'IPv4Addresses': [{
                'Address': '192.168.0.10',
                'SubnetMask': '255.255.252.0',
                'AddressOrigin': 'Static',
                'Gateway': '192.168.0.1',
                'Oem': {}
            }],
            'IPv6AddressPolicyTable': [{
                'Prefix': '::1/128',
                'Precedence': 50,
                'Label': 0
            }],
            'IPv6StaticAddresses': [{
                    'Address': 'fe80::1ec1:deff:fe6f:1e24',
                    'PrefixLength': 16,
                    'Oem': {}
            }],
            'MaxIPv6StaticAddresses': 1,
            'IPv6DefaultGateway': 'fe80::3ed9:2bff:fe34:600',
            'IPv6Addresses': [{
                    'Address': 'fe80::1ec1:deff:fe6f:1e24',
                    'PrefixLength': 64,
                    'AddressOrigin': 'Static',
                    'AddressState': 'Preferred',
                    'Oem': {}
            }],
            'NameServers': [
                'names.redfishspecification.org'
            ],
            'Oem': {}
        };
    }

    CreateCollections() {
        this.vlanCollection = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#VLanNetworkInterfaceCollection.VLanNetworkInterfaceCollection',
            '@odata.type': '#VLanNetworkInterfaceCollection.VLanNetworkInterfaceCollection',
            '@odata.id': this.GetBaseUri() + '/VLANs',
            'Name': 'VLAN Network Interface Collection',
            'Description': 'VLAN Network Interface Collection',
            'Context': this,
            'Resource': Vlan
        });
    }

    AddVlan() {
        var vlan = this.vlanCollection.Add();
        return vlan;
    }

    set Status(val) {
        this.status = val;
    }

    get Status() {
        return this.status;
    }

    get SpeedMbps() {
        return this.speedMbps;
    }

    set SpeedMbps(speedMbps) {
        this.speedMbps = speedMbps;
    }

    get FullDuplex() {
        return this.fullDuplex;
    }

    set FullDuplex(fullDuplex) {
        this.fullDuplex = fullDuplex;
    }
}

exports.EthernetInterface = EthernetInterface;
