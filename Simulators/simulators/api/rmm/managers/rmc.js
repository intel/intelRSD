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
const EthernetInterface = require('./ethernetInterface').EthernetInterface;
const Collection = require('../../common/collection').Collection;
const NetworkProtocol = require('./networkProtocol').NetworkProtocol;

class Rmc extends BaseAsset {
    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);

        this.managerForChassis = this.managerForChassis || [];
        this.networkProtocol = new NetworkProtocol(this.mock, this);
        this.ethernetInterfacesCollection = undefined;

        this.CreateCollections();
        this.MockGetWithDynamicBody();
    };

    GetBaseUri() {
        return '/redfish/v1/Managers/' + this.Id;
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#Managers/Members/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Manager.1.0.0.Manager',
            'Id': this.Id,
            'UUID': '23384634-2137-3323-1720-147392915243',
            'Name': 'Manager',
            'ManagerType': 'RackManager',
            'Description': 'RMC',
            'ServiceEntryPointUUID': '92384634-2938-2342-8820-489239905423',
            'Model': 'RMC 1.0',
            'DateTime': '2015-07-12T03:12:33+06:00',
            'DateTimeLocalOffset': '+06:00',
            'FirmwareRevision': '1.03.01',
            'Actions': {
                'Oem': {
                    'Intel_RackScale': {
                        'RMC#Update': {
                            'target': this.GetBaseUri() +'/Actions/Oem/RMC.Update',
                            'ComponentType@AllowableValues': ['rmm-base'],
                            'Image@AllowableValues': []
                        },
                        'RMC#LoadFactoryDefault': {
                            'target': this.GetBaseUri() +'/Actions/Oem/RMC.LoadFactoryDefault'
                        }

                    }

                },
                '#Manager.Reset': {
                    'target': this.GetBaseUri() +'/Actions/Manager.Reset'
                }
            },
            'Status': {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'NetworkProtocol': {
                '@odata.id': this.GetBaseUri() +'/NetworkProtocol'
            },
            'EthernetInterfaces': {
                '@odata.id': this.GetBaseUri() +'/EthernetInterfaces'
            },
            'Links': {
                'ManagerForChassis@odata.count': this.ManagerForChassis.length,
                'ManagerForChassis': this.ManagerForChassis
            }
        }
    }

    CreateCollections() {
        this.ethernetInterfacesCollection = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#Managers/RMC/EthernetInterfaces',
            '@odata.id': this.GetBaseUri() + '/EthernetInterfaces',
            '@odata.type': '#EthernetInterfaceCollection.1.0.0.EthernetInterfaceCollection',
            'Name': 'Ethernet Interfaces Collection',
            'Description': 'Collection of EthernetInterfaces for this Manager',
            'Context': this,
            'Resource': EthernetInterface
        });
    }

    AddEthernetInterface() {
        return this.ethernetInterfacesCollection.Add();
    }

    get ManagerForChassis() {
        return this.managerForChassis;
    }

    AddManagerForChassis(element) {
        this.managerForChassis.push({'@odata.id': element.GetBaseUri()});
        element.AddManagedBy(this);
        return this;
    }
}

exports.Rmc = Rmc;
