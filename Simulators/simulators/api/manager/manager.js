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
const EthernetInterface = require('../system/ethernetInterface').EthernetInterface;
const Collection = require('../common/collection').Collection;
const NetworkProtocol = require('./networkProtocol').NetworkProtocol;
const OdataIdArray = require('../common/odataIdArray').OdataIdArray;

class Manager extends BaseAsset {
    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);
        this.SetupNetworkProtocol();
        this.CreateCollections();
        this.MockGetWithDynamicBody();

        this.managerForServers = new OdataIdArray();
        this.managerForChassis = new OdataIdArray();
        this.managerForSwitches = new OdataIdArray();
        this.managerForServices = new OdataIdArray();
        this._serialConsole = new SerialConsole();
    };

    GetBaseUri() {
        return super.GetBaseUri('Managers');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata$Manager.Manager',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Manager.v1_2_0.Manager',
            'Id': this.Id,
            'Name': this.Name ||'name-as-string',
            'ManagerType': this.ManagerType || null,
            'Description': 'description-as-string',
            'ServiceEntryPointUUID': this.context.Uuid,
            'UUID': this.Uuid,
            'Model': 'model-as-string',
            'DateTime': '2015-03-13T04:14:33+06:00',
            'DateTimeLocalOffset': '+06:00',
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'GraphicalConsole': {
                'ServiceEnabled': true,
                'MaxConcurrentSessions': 2,
                'ConnectTypesSupported': [
                    'KVMIP'
                ]
            },
            'SerialConsole': this._serialConsole,
            'CommandShell': {
                'ServiceEnabled': true,
                'MaxConcurrentSessions': 4,
                'ConnectTypesSupported': [
                    'Telnet',
                    'SSH'
                ]
            },
            'FirmwareVersion': '1.00',
            'NetworkProtocol': {
                '@odata.id': this.GetBaseUri() + '/NetworkProtocol'
            },
            'EthernetInterfaces': {
                '@odata.id': this.GetBaseUri() + '/EthernetInterfaces'
            },
            'Links': {
                'ManagerForServers': this.ManagerForServers,
                'ManagerForChassis': this.ManagerForChassis,
                'ManagerInChassis': this.ManagerInChassis || null,
                'Oem': {
                    'Intel_RackScale': {
                        '@odata.type': '#Intel.Oem.ManagerLinks',
                        'ManagerForSwitches': this.ManagerForSwitches,
                        'ManagerForServices': this.ManagerForServices
                    }
                }
            },
            'Actions': {
                '#Manager.Reset': {
                    'target': this.GetBaseUri() + '/Actions/Manager.Reset'
                },
                Oem: {}
            },
            Oem: {},
            'PowerState': 'On'
        };
    }

    SetupNetworkProtocol() {
        this.networkProtocol = new NetworkProtocol(this.mock, this);
        this.networkProtocol.Ssdp = {
            'ProtocolEnabled': true,
            'Port': 1900,
            'NotifyMulticastIntervalSeconds': 600,
            'NotifyTTL': 5,
            'NotifyIPv6Scope': 'Site'
        };
    }

    CreateCollections() {
        this.ethernetInterfacesCollection = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#EthernetInterfaceCollection.EthernetInterfaceCollection',
            '@odata.id': this.GetBaseUri() + '/EthernetInterfaces',
            '@odata.type': '#EthernetInterfaceCollection.EthernetInterfaceCollection',
            'Name': 'Ethernet Interfaces Collection',
            'Context': this,
            'Resource': EthernetInterface
        });
    }

    get Status() {
        return this.status;
    }

    set Status(val) {
        this.status = val;
    }

    get Name() {
        return this.name;
    }

    set Name(name) {
        this.name = name;
    }

    get ManagerForSwitches() {
        return this.managerForSwitches;
    }

    get ManagerForChassis() {
        return this.managerForChassis;
    }

    get ManagerForServers() {
        return this.managerForServers;
    }

    get ManagerType() {
        return this.managerType;
    }

    set ManagerType(managerType) {
        this.managerType = managerType;
    }

    get NetworkProtocol() {
        return this.networkProtocol;
    }

    AddManagerForServers(server) {
        this.managerForServers.Add(server);
        return this;
    }

    AddManagerForChassis(chassis) {
        this.managerForChassis.Add(chassis);
        return this;
    }

    AddManagerForSwitches(switches) {
        this.managerForSwitches.Add(switches);
        return this;
    }

    AddManagerForServices(service) {
        this.managerForServices.Add(service);
        return this;
    }

    UnlinkComputerSystem(odataId) {
        this.managerForServers.Remove(odataId);
    }

    get ManagerInChassis() {
        return this.managerLocation;
    }

    set ManagerInChassis(element) {
        this.managerLocation = {'@odata.id': element.GetBaseUri()};
    }

    get ManagerForServices() {
        return this.managerForServices;
    }

    get EthernetInterfacesCollection() {
        return this.ethernetInterfacesCollection;
    }

    AddEthernetInterface(macAddress) {
        this.ethernetInterfacesCollection.Add({'MACAddress': macAddress});
    }

    get serialConsole() {
        return this._serialConsole;
    }
}

function SerialConsole() {
    this.ServiceEnabled = true;
    this.MaxConcurrentSessions = 1;
    this.ConnectTypesSupported = ['Telnet', 'SSH', 'IPMI']
}

exports.Manager = Manager;
