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
const Vlan = require('./vlan').Vlan;
const httpStatusCode = require('../../utils/httpStatusCode').httpStatusCode;

class EthernetSwitchPort extends BaseAsset {
    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);
        this.CreateCollections();
        this.MockGetWithDynamicBody();
        this.EnablePatch(mock);
    }

    GetBaseUri() {
        return super.GetBaseUri('Ports');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#EthernetSwitches/Members/' + this.context.Id + '/Ports/Members/' + this.Id + '/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#EthernetSwitchPort.v1_0_0.EthernetSwitchPort',
            'Id': this.Id,
            'Name': this.Name || 'Switch Port',
            'PortId': this.PortId || 'sw' + this.context.Id + 'p' + this.Id,
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'LinkType': 'Ethernet',
            'OperationalState': 'Up',
            'AdministrativeState': this.AdministrativeState || 'Up',
            'LinkSpeedMbps': this.LinkSpeedMbps || 10000,
            'NeighborInfo': {
                'SwitchId': 'switch-' + this.context.Id,
                'PortId': 'port-' + this.Id,
                'CableId': 'CustomerWritableThing'
            },
            'NeighborMAC': this.NeighborMAC || null,
            'FrameSize': this.FrameSize || 1520,
            'Autosense': this.Autosense != null ? this.Autosense : true,
            'FullDuplex': true,
            'MACAddress': this.MACAddress || null,
            'IPv4Addresses': [
                {
                    'Address': '192.168.0.' + this.Id,
                    'SubnetMask': '255.255.252.0',
                    'AddressOrigin': 'Static',
                    'Gateway': '192.168.0.1'
                }
            ],
            'IPv6Addresses': [
                {
                    'Address': 'fe80::1ec1:deff:fe6f:1e24',
                    'PrefixLength': 64,
                    'AddressOrigin': 'Static',
                    'AddressState': 'Preferred'
                }
            ],
            'PortClass': this.PortClass || 'Physical',
            'PortMode': this.PortMode || 'Unknown',
            'PortType': this.PortType || 'Upstream',
            'Oem': {},
            'VLANs': {
                '@odata.id': this.GetBaseUri() + '/VLANs'
            },
            'Links': {
                'PrimaryVLAN': this.PrimaryVlan || null,
                'Switch': {'@odata.id': this.context.GetBaseUri()},
                'MemberOfPort': this.MemberOfPort || null,
                'PortMembers': this.PortMembers || [],
                'Oem': {}
            }
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
        var self = this;

        function postCallback (body) {
            var vlan = self.AddVlan();
            vlan.VlanId = body.VLANId;
            vlan.VlanEnable = body.VLANEnable;
            vlan.Tagged = body.Oem.Intel_RackScale.Tagged;
            vlan.Name = body.Name;
            vlan.Description = body.Description;
            return vlan;
        }

        function deleteCallback (uriOfVlanBeingDeleted) {
            if (self.PrimaryVlan && self.PrimaryVlan['@odata.id'] === uriOfVlanBeingDeleted) {
                self.PrimaryVlan = null;
            }
        }

        this.vlanCollection.EnablePostAndDelete(postCallback, deleteCallback);
    }

    EnablePatch(mock) {
        var self = this;
        mock.mockPatch(self.GetBaseUri(),
            httpStatusCode.OK,
            undefined,
            function (req, response) {
                var jsonRequest = JSON.parse(req.body);
                self.AdministrativeState = jsonRequest.AdministrativeState || self.AdministrativeState;
                self.LinkSpeedMbps = jsonRequest.LinkSpeedMbps || self.LinkSpeedMbps;
                self.FrameSize = jsonRequest.FrameSize || self.FrameSize;
                self.Autosense = (jsonRequest.Autosense != null) ? jsonRequest.Autosense : self.Autosense;
                if(jsonRequest.Links) {
                    self.PrimaryVlan = jsonRequest.Links.PrimaryVLAN || self.PrimaryVlan;
                    if (self.PortClass == "Logical") {
                        self.PortMembers = jsonRequest.Links.PortMembers || [];

                        self.PortMembers.forEach(portId => {
                            var oDataId = portId['@odata.id'];
                            var portMember = self.Parent.Ports.GetByODataId(oDataId);
                            portMember.AdministrativeState = 'Up';
                        });
                    }
                }
                response.body = self.CreateJsonTemplate();
                return response;
            }
        );
    }

    get Status() {
        return this.status;
    }

    set Status(val) {
        this.status = val;
    }

    AddVlan() {
        return this.vlanCollection.Add();
    }

    get Vlans() {
        return this.vlanCollection;
    }

    get PortType() {
        return this.portType;
    }

    set PortType(portType) {
        this.portType = portType;
    }

    get PortMode() {
        return this.portMode;
    }

    set PortMode(portMode) {
        this.portMode = portMode;
    }

    get Name() {
        return this.name;
    }

    set Name(name) {
        this.name = name;
    }

    get PortMembers() {
        return this.portMembers;
    }

    set PortMembers(portMembers) {
        this.portMembers = portMembers;
    }

    AddPortMember(portMember) {
        this.portMembers = this.portMembers || [];
        this.portMembers.push({'@odata.id': portMember.GetBaseUri()});
    }

    get MemberOfPort() {
        return this.memberOfPort;
    }

    set MemberOfPort(memberOfPort) {
        this.memberOfPort = {'@odata.id': memberOfPort.GetBaseUri()};
    }
}

exports.EthernetSwitchPort = EthernetSwitchPort;
