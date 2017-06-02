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

const Collection = require('../common/collection').Collection;
const EthernetSwitchPort = require('./ethernetSwitchPort').EthernetSwitchPort;
const BaseAsset = require('../common/baseAsset').BaseAsset;
const OdataIdArray = require('../common/odataIdArray').OdataIdArray;

class EthernetSwitch extends BaseAsset {
    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);
        this.CreateCollections();
        this.MockDefaultPostAndDelete();
        this.MockGetWithDynamicBody();
        this.managedBy = new OdataIdArray();
    }

    GetBaseUri() {
        return super.GetBaseUri('EthernetSwitches');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': this.context.GetBaseUri() + '/$metadata#EthernetSwitch.EthernetSwitch',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#EthernetSwitch.v1_0_0.EthernetSwitch',
            'Id': this.Id,
            'SwitchId': this.Id,
            'Name': this.name || 'name-as-string',
            'Description' : this.Description || 'description-as-string',
            'Manufacturer': 'Quanta',
            'Model': 'ly8_rangley',
            'ManufacturingDate': '02/21/2015 00:00:00',
            'SerialNumber': '2M220100SL',
            'PartNumber': '1LY8UZZ0007',
            'FirmwareName': 'ONIE',
            'FirmwareVersion': '1.1',
            'Role': 'Drawer',
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'Oem': {},
            'Ports': {
                '@odata.id': this.GetBaseUri() + '/Ports'
            },
            'Links': {
                'ContainedBy': this.ContainedBy || null,
                'ManagedBy': this.ManagedBy || [],
                'Oem': {}
            }
        };
    }

    CreateCollections() {
        this.portsCollection = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#EthernetSwitches/Members/' + this.Id + '/Ports',
            '@odata.id': this.GetBaseUri() + '/Ports',
            '@odata.type': '#EthernetSwitchPortCollection.EthernetSwitchPortCollection',
            'Name': 'Ethernet Switch Port Collection',
            'Description': 'Ethernet Switch Port Collection',
            'Oem': {},
            'Context': this,
            'Resource': EthernetSwitchPort
        });
    }

    MockDefaultPostAndDelete() {
        var self = this;
        this.portsCollection.EnablePostAndDelete(function (body) {
            var links = body.Links || { PortMembers: [] };
            var port = self.AddLogicalPort();
            port.PortMembers = links.PortMembers;
            port.Name = body.Name;
            port.PortId = body.PortId;
            port.PortMode = body.PortMode;
            var vlan = port.AddVlan();
            vlan.VlanId = '667';
            vlan.Tagged = true;
            vlan.Name = 'vlan-667';
            vlan.Description = 'vlan-667';

            port.PortMembers.forEach(portId => {
                var oDataId = portId['@odata.id'];
                var portMember = self.Ports.GetByODataId(oDataId);
                portMember.AdministrativeState = 'Up';
            });

            return port;
        });
    }

    get Status() {
        return this.status;
    }

    set Status(val) {
        this.status = val;
    }

    AddPort(portClass, mac) {
        var newPort = this.portsCollection.Add();
        newPort.PortClass = portClass;
        newPort.MACAddress = mac;
        return newPort;
    }

    AddPhysicalPort(mac) {
        return this.AddPort('Physical', mac);
    }

    AddLogicalPort() {
        return this.AddPort('Logical');
    }

    get Ports() {
        return this.portsCollection;
    }

    get ManagedBy() {
        return this.managedBy;
    }

    set ManagedBy(elements) {
        this.managedBy = elements;
    }

    set Description(description) {
        this.description = description;
    }

    get Description() {
        return this.description;
    }

    AddManagedBy(element) {
        this.managedBy.Add(element);
        return this;
    }

    get Name() {
        return this.name;
    }

    set Name(name) {
        this.name = name;
    }

    get ContainedBy() {
        return this.containedBy;
    }

    set ContainedBy(element) {
        this.containedBy = {'@odata.id': element.GetBaseUri()};
        element.AddToSwitches(this);
    }
}

exports.EthernetSwitch = EthernetSwitch;
