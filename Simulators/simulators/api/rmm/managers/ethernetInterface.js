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
const Collection = require('../../common/collection').Collection;
const Vlan = require('./vlan').Vlan;

class EthernetInterface extends BaseAsset {

    constructor(mock, system, id) {
        super(mock, system, id);
        this.CreateCollections();
        this.PermanentMacAddress = this.MacAddress;
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return super.GetBaseUri('EthernetInterfaces');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#Managers/RMC/EthernetInterfaces/Members/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#EthernetInterface.1.0.0.EthernetInterface',
            'Id': this.Id,
            'Name': 'Ethernet Interface',
            'Description': 'System NIC 1',
            'Status': {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'PermanentMACAddress': 'E9:47:D3:60:64:91',
            'MACAddress': 'E9:47:D3:60:64:91',
            'SpeedMbps': this.SpeedMbps || null,
            'AutoNeg': true,
            'FullDuplex': this.FullDuplex || null,
            'MTUSize': 1500,
            'HostName': '',
            'FQDN': '',
            'VLANs': {
                '@odata.id': this.GetBaseUri() + '/VLANs'
            },
            'IPv4Addresses': [{
                'Address': '192.168.0.254',
                'SubnetMask': '255.255.252.0',
                'AddressOrigin': 'Static',
                'Gateway': '0.0.0.0',
                'Oem': {}
            }],
            'IPv6AddressPolicyTable': [],
            'IPv6StaticAddresses': [],
            'MaxIPv6StaticAddresses': 1,
            'IPv6DefaultGateway': 'fe80::3ed9:2bff:fe34:600',
            'IPv6Addresses': [],
            'NameServers': []
        };
    }

    CreateCollections() {
        this.vlanCollection = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#Managers/RMC/EthernetInterfaces/1/VLANs',
            '@odata.type': '#VLanNetworkInterfaceCollection.1.0.0.VLanNetworkInterfaceCollection',
            '@odata.id': this.GetBaseUri() + '/VLANs',
            'Name': 'VLAN Network Interface Collection',
            'Context': this,
            'Resource': Vlan
        });
    }

    AddVlan() {
        var vlan = this.vlanCollection.Add();
        return vlan;
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
