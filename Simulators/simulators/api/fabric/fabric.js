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
const Collection = require('../common/collection.js').Collection;
const FabricZone = require('./fabricZone').FabricZone;
const FabricSwitch = require('./fabricSwitch').FabricSwitch;
const FabricEndpoint = require('./fabricEndpoint').FabricEndpoint;

class Fabric extends BaseAsset {
    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);
        this.CreateCollections();
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return super.GetBaseUri('Fabrics');
    }

    CreateJsonTemplate() {
        var defaultJson = {
            '@odata.context': '/redfish/v1/$metadata#Fabric.Fabric',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Fabric.v1_0_0.Fabric',
            'Id': this.Id,
            'Name': this.Name || 'PCIe Fabric',
            'FabricType': 'PCIe',
            'Description': 'PCIe Fabric',
            'MaxZones': null,
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'Zones': {
                '@odata.id': this.GetBaseUri() + '/Zones'
            },
            'Endpoints': {
                '@odata.id': this.GetBaseUri() + '/Endpoints'
            },
            'Switches': {
                '@odata.id': this.GetBaseUri() + '/Switches'
            },
            'Links': {
                'Oem': {}
            },
            'Actions': {
                'Oem': {}
            },
            'Oem': {}
        };
        return defaultJson;
    }

    CreateCollections() {
        this.zoneCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#ZoneCollection.ZoneCollection',
            '@odata.id': this.GetBaseUri() + '/Zones',
            '@odata.type': '#ZoneCollection.ZoneCollection',
            'Name': 'Zones Collection',
            'Context': this,
            'Resource': FabricZone
        });
        this.endpointCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#EndpointCollection.EndpointCollection',
            '@odata.id': this.GetBaseUri() + '/Endpoints',
            '@odata.type': '#EndpointCollection.EndpointCollection',
            'Name': 'Endpoint Collection',
            'Context': this,
            'Resource': FabricEndpoint
        });
        this.switchCollection = new Collection(this.mock, {
            '@odata.context': this.GetBaseUri() + '/$metadata#SwitchCollection.SwitchCollection',
            '@odata.id': this.GetBaseUri() + '/Switches',
            '@odata.type': '#SwitchCollection.SwitchCollection',
            'Name': 'Switch Collection',
            'Context': this,
            'Resource': FabricSwitch
        });
    }

    AddZone() {
        return this.zoneCollection.Add();
    }

    AddEndpoint() {
        return this.endpointCollection.Add();
    }

    AddSwitch() {
        return this.switchCollection.Add();
    }

    get Status() {
        return this.status;
    }

    set Status(val) {
        this.status = val;
    }

    get Zones() {
        return this.zoneCollection;
    }

    get Switches() {
        return this.switchCollection;
    }

    get Endpoints() {
        return this.endpointCollection;
    }

    get Name() {
        return this.name;
    }

    set Name(name) {
        this.name = name;
    }
}


exports.Fabric = Fabric;