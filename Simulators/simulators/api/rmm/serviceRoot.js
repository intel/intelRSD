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
const Rack = require('./rack/rack').Rack;
const Drawer = require('./rack/drawer').Drawer;
const EventService = require('../eventService/eventService').EventService;
const EventTrigger = require('../eventService/eventTrigger').EventTrigger;
const MessageRegistry = require('../messageRegistry/messageRegistry').MessageRegistry;
const Rmc = require('./managers/rmc').Rmc;
const Mbpc = require('./managers/mbpc').Mbpc;

class RmmServiceRoot extends BaseAsset {
    constructor(mock, uuid) {
        super(mock, undefined, undefined, undefined, uuid);
        this.name = 'Root Service';
        this.CreateCollections();
        this.MockGetWithDynamicBody();
        this.EventService = new EventService(mock, this.GetBaseUri(), '#EventService.1.0.0.EventService');
        this.EventTrigger = new EventTrigger(this.EventService);
        this.MessageRegistry = new MessageRegistry(mock, this.GetBaseUri(), '#MessageRegistry.1.0.0.MessageRegistry');
    }

    GetBaseUri() {
        return '/redfish/v1';
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#ServiceRoot',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#ServiceRoot.1.0.0.ServiceRoot',
            'Id': 'RootService',
            'UUID': this.Uuid,
            'Name': 'Root Service',
            'RedfishVersion': '1.0.2',
            'Chassis': {
                '@odata.id': this.GetBaseUri() + '/Chassis'
            },
            'Managers': {
                '@odata.id': this.GetBaseUri() + '/Managers'
            },
            'EventService': {
                '@odata.id': this.GetBaseUri() + '/EventService'
            },
            'MessageRegistry': {
                '@odata.id': this.GetBaseUri() + '/MessageRegistry'
            }
        };
    }

    CreateCollections() {
        this.chassisCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#Chassis',
            '@odata.id': this.GetBaseUri() + '/Chassis',
            '@odata.type': '#ChassisCollection.1.0.0.ChassisCollection',
            'Name': 'Chassis Collection',
            'Context': this,
            'Resource': BaseAsset
        });

        this.managerCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#Managers',
            '@odata.id': this.GetBaseUri() + '/Managers',
            '@odata.type': '#ManagerCollection.1.0.0.ManagerCollection',
            'Name': 'Manage Collection',
            'Context': this,
            'Resource': Mbpc
        });
    }

    AddRack() {
        var rack = this.chassisCollection.AddWithPrefixAndResource('Rack', Rack);
        return rack;
    }

    AddDrawer() {
        var drawer = this.chassisCollection.AddWithPrefixAndResource('Drawer', Drawer);
        return drawer;
    }

    get Drawers() {
        return this.chassisCollection.GetAllByProperty('ChassisType', 'Drawer');
    }

    get Managers() {
        return this.managerCollection;
    }

    AddMbpc() {
        return this.managerCollection.AddWithId('MBPC');
    }

    AddRmc() {
        return this.managerCollection.AddWithPrefixAndResource('RMC', Rmc);
    }

    RemoveDrawerWithId(id) {
        var element = this.chassisCollection.GetById(id);

        if (element != undefined && element.ChassisType === 'Drawer') {
            this.chassisCollection.RemoveWithOdataId(element.GetBaseUri());
            return element.GetBaseUri();
        }
    }

    set EventService(eventService) {
        this.eventService = eventService;
    }

    get EventService() {
        return this.eventService;
    }

    set EventTrigger(eventTrigger) {
        this.eventTrigger = eventTrigger;
    }

    get EventTrigger() {
        return this.eventTrigger;
    };

    set MessageRegistry(messageRegistry) {
        this.messageRegistry = messageRegistry;
    }

    get MessageRegistry() {
        return this.messageRegistry;
    }

    get Chassis() {
        return this.chassisCollection;
    }

    GetAllChassisByType(chassisType) {
        return this.chassisCollection.GetAllByProperty('ChassisType', chassisType);
    }

    Clear() {
        this.chassisCollection.Clear();
        this.managerCollection.Clear();
    }
}

exports.RmmServiceRoot = RmmServiceRoot;
