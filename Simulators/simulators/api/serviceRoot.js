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

const BaseAsset = require('./common/baseAsset').BaseAsset;
const Collection = require('./common/collection').Collection;
const NodeCollection = require('./node/nodeCollection').NodeCollection;
const Node = require('./node/node').Node;
const Chassis = require('./chassis/chassis').Chassis;
const EventService = require('./eventService/eventService').EventService;
const EventTrigger = require('./eventService/eventTrigger').EventTrigger;
const System = require('./system/system').System;
const Manager = require('./manager/manager').Manager;
const Service = require('./service/service.js').Service;
const EthernetSwitch = require('./ethernetSwitch/ethernetSwitch').EthernetSwitch;
const randomMac = require('./../utils/random.js').randomMac;
const Fabric = require('./fabric/fabric').Fabric;

class ServiceRoot extends BaseAsset {
    constructor(mock, uuid) {
        super(mock, null, null, null, uuid);
        this.eventService = new EventService(mock, this);
        this.eventTrigger = new EventTrigger(this.eventService);
        this.CreateCollections();
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return '/redfish/v1';
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': this.GetBaseUri() + '$metadata#ServiceRoot',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#ServiceRoot.v1_0_0.ServiceRoot',
            'Id': 'RootService',
            'Name': this.Name,
            'RedfishVersion': '1.0.0',
            'UUID': this.Uuid,
            'Systems': {
                '@odata.id': this.GetBaseUri() + '/Systems'
            },
            'Chassis': {
                '@odata.id': this.GetBaseUri() + '/Chassis'
            },
            'Managers': {
                '@odata.id': this.GetBaseUri() + '/Managers'
            },
            'EventService': {
                '@odata.id': this.GetBaseUri() + '/EventService'
            },
            'Services': {
                '@odata.id': this.GetBaseUri() + '/Services'
            },
            'Nodes': {
                '@odata.id': this.GetBaseUri() + '/Nodes'
            },
            'EthernetSwitches': {
                '@odata.id': this.GetBaseUri() + '/EthernetSwitches'
            },
            'Fabrics': {
                '@odata.id': this.GetBaseUri() + '/Fabrics'
            },
            'Oem': {
                'Intel_RackScale': {
                    '@odata.type': "#Intel.Oem.ServiceRoot",
                    'ApiVersion': '2.0.0'
                }
            },
            'Links': {}
        };
    }

    CreateCollections() {
        this.chassisCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#Chassis',
            '@odata.id': this.GetBaseUri() + '/Chassis',
            '@odata.type': '#ChassisCollection.ChassisCollection',
            'Name': 'Chassis Collection',
            'Context': this,
            'Resource': Chassis
        });
        this.systemsCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#Systems',
            '@odata.id': this.GetBaseUri() + '/Systems',
            '@odata.type': '#ComputerSystemCollection.ComputerSystemCollection',
            'Name': 'Computer System Collection',
            'Context': this,
            'Resource': System,
            'OnElementAddedCallback': (systemUri) => this.eventTrigger.FireResourceAddedEvent(systemUri),
            'OnElementRemovedCallback': (systemUri) => this.eventTrigger.FireResourceRemovedEvent(systemUri)
        });

        this.managersCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#ManagerCollection.ManagerCollection',
            '@odata.id': this.GetBaseUri() + '/Managers',
            '@odata.type': '#ManagerCollection.ManagerCollection',
            'Name': 'Manager Collection',
            'Context': this,
            'Resource': Manager
        });

        this.servicesCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#Services',
            '@odata.id': this.GetBaseUri() + '/Services',
            '@odata.type': '#StorageServiceCollection.StorageServiceCollection',
            'Name': 'Storage Services Collection',
            'Context': this,
            'Resource': Service
        });

        this.nodesCollection = new NodeCollection(this.mock, {
            '@odata.context': this.GetBaseUri() + '$metadata#Nodes',
            '@odata.id': this.GetBaseUri() + '/Nodes',
            '@odata.type': '#ComposedNodeCollection.ComposedNodeCollection',
            'Name': 'Composed Nodes Collection',
            'Context': this,
            'Resource': Node
        });

        this.ethernetSwitchesCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#EthernetSwitches',
            '@odata.id': this.GetBaseUri() + '/EthernetSwitches',
            '@odata.type': '#EthernetSwitchCollection.EthernetSwitchCollection',
            'Name': 'Ethernet Switches Collection',
            'Description': 'Ethernet Switches Collection',
            'Context': this,
            'Resource': EthernetSwitch
        });

        this.fabricCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#FabricCollection.FabricCollection',
            '@odata.id': this.GetBaseUri() + '/Fabrics',
            '@odata.type': '#FabricCollection.FabricCollection',
            'Name': 'Fabric Collection',
            'Description': 'Fabric Collection',
            'Context': this,
            'Resource': Fabric
        });
    }

    get Name() {
        return this.name;
    }

    set Name(name) {
        this.name = name;
    }

    get Drawers() {
        return this.GetAllChassisByType('Drawer');
    }

    get Systems() {
        return this.systemsCollection;
    }

    get Fabrics() {
        return this.fabricCollection;
    }

    AddDrawer() {
        return this.AddChassis('Drawer');
    }

    AddEnclosureChassis() {
        return this.AddChassis('Enclosure');
    }

    get Managers() {
        return this.managersCollection;
    }

    get Sleds() {
        return this.GetAllChassisByType('Sled');
    }

    AddSled() {
        return this.AddChassis('Sled');
    }

    get Chassis() {
        return this.chassisCollection;
    }

    AddChassis(chassisType) {
        var chassis = this.chassisCollection.AddWithPrefix(chassisType.toLowerCase());
        chassis.ChassisType = chassisType;

        return chassis;
    }

    GetChassisWithId(id) {
        return this.chassisCollection.GetById(id);
    }

    GetSystemById(id) {
        return this.systemsCollection.GetById(id);
    }

    GetChassisWithOdataId(odataId) {
        var chassisUri = odataId['@odata.id'];
        var chassisId = chassisUri.split('/').pop();
        return this.GetChassisWithId(chassisId);
    }

    GetAllChassisByType(chassisType) {
        return this.chassisCollection.GetAllByProperty('ChassisType', chassisType);
    }

    GetAllChassisByDescription(chassisDescription) {
        return this.chassisCollection.GetAllByProperty('Description', chassisDescription);
    }

    RemoveChassisWithId(id) {
        var chassis = this.chassisCollection.GetById(id);

        if (chassis !== undefined) {
            var chassisBaseUri = chassis.GetBaseUri();
            if (chassis.ContainedBy != null) {
                var containedBy = this.GetChassisWithOdataId(chassis.ContainedBy);
                containedBy.RemoveFromContains(chassisBaseUri);
            }
            this.chassisCollection.RemoveWithOdataId(chassisBaseUri);
            return chassisBaseUri;
        }
    }

    get FabricModules() {
        return this.GetAllChassisByType('Module');
    }

    AddModule() {
        return this.AddChassis('Module');
    }

    get Blades() {
        return this.GetAllChassisByType('Blade');
    }

    AddBlade() {
        return this.AddChassis('Blade');
    }

    AddRack() {
        return this.AddChassis('Rack');
    }

    AddPod() {
        return this.AddChassis('Pod');
    }

    AddSystem() {
        return this.systemsCollection.Add();
    }

    AddFabric() {
        return this.fabricCollection.Add();
    }

    RemoveSystemWithId(id) {
        const system = this.GetSystemById(id);

        if (system != undefined) {
            var systemUri = system.GetBaseUri();
            if (system.Chassis != null) {
                var chassis = this.GetChassisWithOdataId(system.Chassis);
                chassis.UnlinkComputerSystem(systemUri);
            }
            this.systemsCollection.RemoveWithOdataId(systemUri);
            return systemUri;
        }
    }

    RemoveSystem(system) {
        const systemUri = system.GetBaseUri();
        system.UnlinkComputerSystemFromConnectedAssets();
        this.systemsCollection.RemoveWithOdataId(systemUri);
    }

    get EventService() {
        return this.eventService;
    }

    get EventTrigger() {
        return this.eventTrigger;
    }

    AddManagementControllerManager() {
        var manager = this.managersCollection.Add();
        manager.ManagerType = 'ManagementController';
        return manager;
    }

    AddEnclosureManager() {
        var manager = this.AddManager();
        manager.ManagerType = 'EnclosureManager';
        return manager;
    }

    AddManager() {
        var manager = this.managersCollection.Add();
        manager.AddEthernetInterface(randomMac());
        return manager;
    }

    AddService() {
        return this.servicesCollection.AddWithPrefix('service');
    }

    get Services() {
        return this.servicesCollection;
    }

    AddEthernetSwitch() {
        return this.ethernetSwitchesCollection.Add();
    }

    get EthernetSwitches() {
        return this.ethernetSwitchesCollection;
    }

    Clear() {
        this.nodesCollection.Clear();
        this.fabricCollection.Clear();
        this.systemsCollection.Clear();
        this.chassisCollection.Clear();
        this.managersCollection.Clear();
        this.servicesCollection.Clear();
        this.ethernetSwitchesCollection.Clear();
    }
}

exports.ServiceRoot = ServiceRoot;
